#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstElement *pipeline = gst_pipeline_new("playback-pipeline");
    GstElement *src = gst_element_factory_make("uridecodebin", "source");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    g_object_set(src, "uri", "file:///home/bhargav/Documents/gstreamer_sample/video.mp4", NULL);

    gst_bin_add_many(GST_BIN(pipeline), src, sink, NULL);

    // Link "pad-added" signal to dynamically link the decoding elements
    g_signal_connect(src, "pad-added", G_CALLBACK(gst_element_link_pads), sink);

    // Set pipeline to playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop or handle events and messages

    // Clean up resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

