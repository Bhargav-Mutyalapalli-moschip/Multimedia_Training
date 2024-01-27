#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *src, *filter, *sink;
    GstBus *bus;
    GstMessage *msg;
    gboolean is_live;
    GstClockTime min_latency, max_latency;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the pipeline and elements
    pipeline = gst_pipeline_new("latency-calc-pipeline");
    src = gst_element_factory_make("videotestsrc", "source");
    filter = gst_element_factory_make("identity", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !filter || !sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, filter, sink, NULL);

    if (!gst_element_link(src, filter) || !gst_element_link(filter, sink)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to the playing state
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_print("Failed to set pipeline to playing state. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Query latency information from the sink element
    GstQuery *latency_query = gst_query_new_latency();
    if (gst_element_query(sink, latency_query)) {
        gst_query_parse_latency(latency_query, &is_live, &min_latency, &max_latency);

        if (is_live) {
            g_print("Element is live.\n");
        } else {
            g_print("Element is not live.\n");
        }

        g_print("Minimum Latency: %" GST_TIME_FORMAT " (%" G_GUINT64_FORMAT " ns)\n",
                GST_TIME_ARGS(min_latency), min_latency);
        g_print("Maximum Latency: %" GST_TIME_FORMAT " (%" G_GUINT64_FORMAT " ns)\n",
                GST_TIME_ARGS(max_latency), max_latency);

        // Calculate and print the overall latency
        GstClockTime overall_latency = max_latency - min_latency;
        g_print("Overall Latency: %" GST_TIME_FORMAT " (%" G_GUINT64_FORMAT " ns)\n",
                GST_TIME_ARGS(overall_latency), overall_latency);
    } else {
        g_print("Failed to query latency information from the sink element.\n");
    }
    gst_query_unref(latency_query);

    // Create a bus to watch for messages
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, (GstBusFunc)gst_bus_pop, NULL);

    // Run the GStreamer main loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Free resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

