#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create a source element (e.g., videotestsrc)
    GstElement *source = gst_element_factory_make("videotestsrc", "my-source");

    // Create a multi-sink element (e.g., ximagesink)
    GstElement *multiSink = gst_element_factory_make("ximagesink", "my-multi-sink");

    // Add the source and multi-sink elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), multiSink);

    // Link the source to the multi-sink
    if (!gst_element_link(source, multiSink)) {
        g_print("Elements could not be linked.\n");
        return -1;
    }

    // Cast the multi-sink element to GstChildProxy interface
    GstChildProxy *childProxy = GST_CHILD_PROXY(multiSink);

    // Get references to child sinks by index
    GObject *sink1 = gst_child_proxy_get_child_by_index(childProxy, 0);
    GObject *sink2 = gst_child_proxy_get_child_by_index(childProxy, 1);

    // Set properties on the child sinks (e.g., window title for ximagesink)
    g_object_set(sink1, "window-title", "XImageSink", NULL);
    g_object_set(sink2, "enable-last-sample", TRUE, NULL); // Enable last-sample property for autovideosink

    // Create a GStreamer bus to handle messages
    GstBus *bus = gst_element_get_bus(pipeline);

    // Set the pipeline to PLAYING state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait for EOS (End of Stream) or error message
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Clean up
    gst_message_unref(msg);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}

