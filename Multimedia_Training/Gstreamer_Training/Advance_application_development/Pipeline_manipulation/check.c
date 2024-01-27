#include <gst/gst.h>

// Callback function to handle the "handoff" signal
static void handoff_cb(GstElement *identity, GstBuffer *buffer, gpointer user_data) {
    // Create a new buffer with the same properties as the incoming buffer
    GstBuffer *new_buffer = gst_buffer_copy(buffer);

    // Map the new buffer to modify its data
    GstMapInfo map;
    if (gst_buffer_map(new_buffer, &map, GST_MAP_WRITE)) {
        guint8 *data = map.data;
        gint size = map.size;

        // Modify the data in the new buffer (e.g., invert pixel values)
        for (gint i = 0; i < size; i++) {
            data[i] = 255 - data[i];
        }

        // Unmap the buffer to release it
        gst_buffer_unmap(new_buffer, &map);

        // Push the modified buffer downstream
        GstPad *srcpad = gst_element_get_static_pad(identity, "src");
        GstFlowReturn ret = gst_pad_push(srcpad, new_buffer);
        gst_object_unref(srcpad);

        if (ret != GST_FLOW_OK) {
            g_warning("Failed to push buffer downstream.");
        }
    } else {
        g_warning("Failed to map the buffer.");
    }
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the GStreamer pipeline
    GstElement *pipeline = gst_pipeline_new("buffer-manipulation-pipeline");
    GstElement *src = gst_element_factory_make("videotestsrc", "source");
    GstElement *identity = gst_element_factory_make("identity", "identity");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !identity || !sink) {
        g_error("Failed to create elements.");
        return -1;
    }

    // Set the silent property to FALSE to enable last-message output
    g_object_set(identity, "silent", FALSE, NULL);

    // Add elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), src);
    gst_bin_add(GST_BIN(pipeline), identity);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Link the elements
    if (!gst_element_link(src, identity) || !gst_element_link(identity, sink)) {
        g_error("Elements could not be linked.");
        gst_object_unref(pipeline);
        return -1;
    }

    // Connect to the "handoff" signal of the identity element
    g_signal_connect(identity, "handoff", G_CALLBACK(handoff_cb), NULL);

    // Set the pipeline to the playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // After setting the pipeline to the PLAYING state, add this code to listen for messages:
GstBus *bus = gst_element_get_bus(pipeline);
GstMessage *msg;
while ((msg = gst_bus_pop(bus))) {
    GError *err = NULL;
    gchar *debug_info = NULL;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", (debug_info) ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            // Handle other messages as needed
            break;
    }

    gst_message_unref(msg);
}

gst_object_unref(bus);


    // Cleanup
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

