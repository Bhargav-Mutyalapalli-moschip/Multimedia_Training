#include <gst/gst.h>

static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer user_data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", (debug_info) ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        }
        case GST_MESSAGE_EOS:
            g_print("End of stream reached.\n");
            break;
        default:
            break;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the pipeline and elements
    GstElement *pipeline, *src, *sink1, *sink2, *filter;
    pipeline = gst_pipeline_new("latency-compensation-pipeline");
    src = gst_element_factory_make("videotestsrc", "source");
    sink1 = gst_element_factory_make("autovideosink", "sink1");
    sink2 = gst_element_factory_make("autovideosink", "sink2");
    filter = gst_element_factory_make("identity", "filter");

    if (!pipeline || !src || !sink1 || !sink2 || !filter) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, filter, sink1, sink2, NULL);

    if (!gst_element_link(src, filter) || !gst_element_link(filter, sink1) || !gst_element_link(filter, sink2)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set up a bus to watch for messages
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_callback, NULL);
    gst_object_unref(bus);

    // Set the pipeline to the playing state
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_print("Failed to set pipeline to playing state. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Perform a latency query on all sinks
    guint64 max_latency = 0;
    GstElement *sinks[] = {sink1, sink2};
    for (int i = 0; i < G_N_ELEMENTS(sinks); i++) {
        GstQuery *latency_query = gst_query_new_latency();
        if (gst_element_query(sinks[i], latency_query)) {
            guint64 latency = 0;
            gst_query_parse_latency(latency_query, NULL, &latency);
            if (latency > max_latency) {
                max_latency = latency;
            }
        }
        gst_query_unref(latency_query);
    }

    // Configure the pipeline with a LATENCY event
    GstClockTime latency_time = max_latency * GST_MSECOND; // Convert to nanoseconds
    GstEvent *latency_event = gst_event_new_latency(latency_time);
    gst_element_send_event(pipeline, latency_event);

    // Run the GStreamer main loop
    GstClockTime last_buffer_running_time = GST_CLOCK_TIME_NONE;
    g_main_loop_run(g_main_loop_new(NULL, FALSE));

    // Free resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

