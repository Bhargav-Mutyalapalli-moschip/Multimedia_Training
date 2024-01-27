#include <gst/gst.h>

// This callback function will be called when the identity element emits the "handoff" signal.
static void handoff_callback(GstElement *identity, GstBuffer *buffer, gpointer user_data) {
    // You can perform your buffer inspection here.
    //g_print("Received a buffer with size %u\n", GST_BUFFER_SIZE(buffer));
    // If you want to see the buffer data, you can uncomment the following line.
    // g_print("Buffer data: %s\n", GST_BUFFER_DATA(buffer));
     g_print("Received a buffer with size %lu\n", gst_buffer_get_size(buffer));
    // If you want to see the buffer data, you can uncomment the following line.
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a pipeline
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create elements
    GstElement *src = gst_element_factory_make("videotestsrc", "source");
    GstElement *identity = gst_element_factory_make("identity", "my-identity");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !identity || !sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, identity, sink, NULL);

    // Link elements
    if (!gst_element_link_many(src, identity, sink, NULL)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the "silent" property of the identity element to FALSE
    g_object_set(identity, "silent", FALSE, NULL);

    // Connect the "handoff" signal to the callback function
    g_signal_connect(identity, "handoff", G_CALLBACK(handoff_callback), NULL);

    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    gst_message_unref(msg);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}

