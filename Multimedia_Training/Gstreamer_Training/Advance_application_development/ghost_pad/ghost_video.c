#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *src, *sink, *filter;
    GstPad *srcpad, *ghostpad,*sinkpad;
    GstBus *bus;
    GstMessage *msg;
    GError *error = NULL;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    pipeline = gst_pipeline_new("my-pipeline");

    // Create source, filter, and sink elements
    src = gst_element_factory_make("videotestsrc", "source");
    filter = gst_element_factory_make("autovideoconvert", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !filter || !sink) {
        g_print("One element could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, filter, sink, NULL);

    // Link source and filter using a ghost pad
    srcpad = gst_element_get_static_pad(src, "src");
    ghostpad = gst_ghost_pad_new("ghost-pad", srcpad);
    gst_element_add_pad(GST_ELEMENT(filter), ghostpad);
    sinkpad=gst_element_get_static_pad(filter,"sink");
    gst_pad_link(ghostpad,sinkpad);
    gst_object_unref(srcpad);


    // Link filter and sink
    if (!gst_element_link(filter, sink)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait for the pipeline to finish or an error to occur
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Release resources
    if (msg != NULL) {
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

