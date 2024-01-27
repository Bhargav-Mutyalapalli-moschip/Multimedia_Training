#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the main pipeline
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create two bins: bin1 and bin2
    GstElement *bin1 = gst_bin_new("bin1");
    GstElement *bin2 = gst_bin_new("bin2");

    // Create elements for bin1 and bin2 (e.g., a source and a sink)
    GstElement *source_element = gst_element_factory_make("videotestsrc", "source");
    GstElement *sink_element = gst_element_factory_make("autovideosink", "sink");

    // Add elements to bin1 and bin2
    gst_bin_add(GST_BIN(bin1), source_element);
    gst_bin_add(GST_BIN(bin2), sink_element);

    // Create a ghost pad in bin1
    GstPad *src_pad = gst_element_get_static_pad(source_element, "src");
    if(!src_pad)
    {
	    g_print("failed to retrieve its specific pad \n");
	    return -1;
    }
    GstPad *ghost_pad = gst_ghost_pad_new("ghost-src-pad", src_pad);
    if(!ghost_pad)
    {
	    g_print("failed to retrieve its specific pad which is ghost pad \n");
	    return -1;
    }
    gst_pad_set_active(ghost_pad, TRUE);

    // Add the ghost pad to bin1
    gst_element_add_pad(GST_ELEMENT(bin1), ghost_pad);

    //Genarated dot files
    gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    // Link the ghost pad to the sink element in bin2
    GstPad *sink_pad = gst_element_get_static_pad(sink_element, "sink");
    if(!sink_pad)
    {
	    g_print("failed to retrieve its specific pad which is sink pad \n");
	    return -1;
    }
    if (gst_pad_link(ghost_pad, sink_pad) != GST_PAD_LINK_OK) {
	    g_printerr("Failed to link ghost pad to sink pad.\n");
	    return -1;
    }

    // Add bins to the main pipeline
    gst_bin_add(GST_BIN(pipeline), bin1);
    gst_bin_add(GST_BIN(pipeline), bin2);

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);


    // Wait until the user quits the application
    g_print("Press Enter to quit...\n");
    getchar();

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

