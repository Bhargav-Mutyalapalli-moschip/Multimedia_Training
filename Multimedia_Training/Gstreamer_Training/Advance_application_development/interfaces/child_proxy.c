#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *filter, *sink;
    GstChildProxy *childProxy;
    GError *error = NULL;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the pipeline
    pipeline = gst_pipeline_new("my-pipeline");

    // Create a source element (videotestsrc)
    source = gst_element_factory_make("videotestsrc", "source");
    
    // Create a filter element (videoflip)
    filter = gst_element_factory_make("videoflip", "filter");

    // Create a display sink element (xvimagesink)
    sink = gst_element_factory_make("xvimagesink", "sink");

    // Add the elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);

    // Link the elements: source -> filter -> sink
    if (!gst_element_link(source, filter) || !gst_element_link(filter, sink)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get the GstChildProxy interface for the filter element
    childProxy = GST_CHILD_PROXY(filter);

    // Set the properties of the filter element using GstChildProxy
    g_object_set(childProxy, "method", 0, NULL); // Set the "method" property of the filter element

    // Set the pipeline to the "playing" state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    
    // Free resources
    if (msg != NULL) {
        gst_message_unref(msg);
    }
    
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

