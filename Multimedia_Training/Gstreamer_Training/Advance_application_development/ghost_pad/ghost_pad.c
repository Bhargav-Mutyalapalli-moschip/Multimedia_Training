#include <gst/gst.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create elements
    GstElement *source1 = gst_element_factory_make("videotestsrc", "source1");
    GstElement *source2 = gst_element_factory_make("videotestsrc", "source2");
    GstElement *videosink = gst_element_factory_make("autovideosink", "videosink");
    GstElement *pipeline = gst_pipeline_new("video-mixer-pipeline");

    if (!source1 || !source2 || !videosink || !pipeline) {
        g_printerr("One or more elements could not be created.\n");
        return -1;
    }

    // Create a custom bin
    GstElement *videomixer = gst_bin_new("videomixer");

    if (!videomixer) {
        g_printerr("Custom bin 'videomixer' could not be created.\n");
        return -1;
    }

    // Add elements to the pipeline and custom bin
    gst_bin_add(GST_BIN(pipeline), source1);
    gst_bin_add(GST_BIN(pipeline), source2);
    gst_bin_add(GST_BIN(pipeline), videomixer);
    gst_bin_add(GST_BIN(pipeline), videosink);

    // Link source1 to videomixer via a ghost pad
    GstPad *srcpad1 = gst_element_get_static_pad(source1, "src");
    GstPad *ghostpad1 = gst_ghost_pad_new("ghostpad1", srcpad1);
    gst_element_add_pad(GST_ELEMENT(videomixer), ghostpad1);

    // Link source2 to videomixer via a ghost pad
    GstPad *srcpad2 = gst_element_get_static_pad(source2, "src");
    GstPad *ghostpad2 = gst_ghost_pad_new("ghostpad2", srcpad2);
    gst_element_add_pad(GST_ELEMENT(videomixer), ghostpad2);

    //set the properties
    g_object_set(source2,"pattern",1,NULL);
    // Link videomixer to videosink
    GstPad *sinkpad = gst_element_get_static_pad(videosink, "sink");
    gst_pad_link(ghostpad1, sinkpad);
    gst_pad_link(ghostpad2, sinkpad);

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
    // Wait for the pipeline to finish
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_poll(bus, GST_MESSAGE_EOS, -1);

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    while(1);
    return 0;
}

