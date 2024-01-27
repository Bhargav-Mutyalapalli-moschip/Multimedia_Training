#include <gst/gst.h>

int main() 
{
    gst_init(NULL, NULL);  // Initialize GStreamer

    // Create a pipeline and elements
    GstElement *pipeline = gst_pipeline_new("mypipeline");
    GstElement *source = gst_element_factory_make("videotestsrc", "mysource");
    GstElement *sink = gst_element_factory_make("autovideosink", "mysink");

    if (!pipeline || !source || !sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Set a property of the source element using gst_util_set_object_arg()
    GstCaps *caps = gst_caps_from_string("video/x-raw, width=1080, height=720");
    const gchar *caps_string = gst_caps_to_string(caps);
    gst_util_set_object_arg(G_OBJECT(source), "pattern", "ball");
    gst_util_set_object_arg(G_OBJECT(source), "caps", caps_string);

    // Link elements and set the pipeline to the playing state
    if (gst_element_link(source, sink) == FALSE) {
        g_print("Elements could not be linked. Exiting.\n");
        return -1;
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the GMainLoop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

