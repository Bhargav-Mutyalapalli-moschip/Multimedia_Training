#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *videotestsrc,*sink;
    GstPad *pad;
    GstCaps *caps;
    gchar *caps_string;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a pipeline
    pipeline = gst_pipeline_new("test-pipeline");

    // Create the videotestsrc element
    videotestsrc = gst_element_factory_make("videotestsrc", "source");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !videotestsrc) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Add videotestsrc to the pipeline
    gst_bin_add(GST_BIN(pipeline), videotestsrc);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Link the elements
    if (!gst_element_link(videotestsrc, sink)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get the source pad of videotestsrc
    pad = gst_element_get_static_pad(videotestsrc, "src");

    // Query the current capabilities of the pad
    caps = gst_pad_query_caps(pad, NULL);

    // Convert the capabilities to a string
    caps_string = gst_caps_to_string(caps);

    g_print("Capabilities of videotestsrc:\n%s\n", caps_string);

    // Clean up
    g_free(caps_string);
    gst_caps_unref(caps);
    gst_object_unref(pad);
    gst_object_unref(pipeline);

    return 0;
}

