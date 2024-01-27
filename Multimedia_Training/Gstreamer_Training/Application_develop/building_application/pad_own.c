#include <gst/gst.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a pipeline
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create a video source element (e.g., a videotestsrc)
    GstElement *source = gst_element_factory_make("videotestsrc", "source");

    // Create a video sink element (e.g., autovideosink)
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    // Create a caps object with video properties
    GstCaps *caps = gst_caps_new_simple(
        "video/x-raw",
        "format", G_TYPE_STRING, "I420",
        "width", G_TYPE_INT, 384,
        "height", G_TYPE_INT, 288,
        "framerate", GST_TYPE_FRACTION, 25, 1,
        NULL
    );

    // Check if caps creation was successful
    if (!caps) {
        g_print("Failed to create caps\n");
        return 1;
    }

    // Set the caps on the source element
    g_object_set(G_OBJECT(source), "caps", caps, NULL);

    // Add elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Link the source and sink elements
    if (gst_element_link(source, sink)) {
        g_print("Elements linked successfully\n");
    } else {
        g_print("Failed to link elements\n");
        return 1;
    }

    // Set the pipeline to the playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the GMainLoop (you may need to implement your own)
    // This loop will keep the application running until you stop it manually.

    // Clean up
    g_main_loop_run(g_main_loop_new(NULL, FALSE));

    // Clean up GStreamer
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    gst_caps_unref(caps);
    gst_deinit();

    return 0;
}

