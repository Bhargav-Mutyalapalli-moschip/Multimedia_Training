#include <gst/gst.h>

int main(int argc, char *argv[])
{
	GstElement *pipeline;
    // ... Initialize GStreamer and create elements as before

    // Create the DOT file
    GstDebugCategory *debug = gst_debug_category_new("pipeline-dot-example", GST_DEBUG_FG_YELLOW, "Pipeline DOT Example");
    GST_DEBUG_CATEGORY_SET_THRESHOLD(debug, GST_LEVEL_DEBUG);
    gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    // Start playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    {
        // ... Handle errors as before
    }

    // Wait until error or EOS
    // ...

    // Free resources
    // ...

    return 0;
}

