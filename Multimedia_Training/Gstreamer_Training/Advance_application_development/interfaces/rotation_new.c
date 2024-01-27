#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *filter, *sink;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the pipeline
    pipeline = gst_pipeline_new("video-rotation-pipeline");

    // Create elements
    source = gst_element_factory_make("videotestsrc", "videosource");
    filter = gst_element_factory_make("videoflip", "videoflip");
    sink = gst_element_factory_make("autovideosink", "videosink");

    if (!pipeline || !source || !filter || !sink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set the rotation property (90 degrees clockwise)

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);

    // Link elements
    if (!gst_element_link_many(source, filter, sink, NULL)) {
        g_printerr("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to the playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the GMain Loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Clean up resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);
    gst_deinit();

    return 0;
}

