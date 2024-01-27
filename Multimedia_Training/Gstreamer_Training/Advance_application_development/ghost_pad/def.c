#include <gst/gst.h>

int main(int argc, char *argv[]) {
	gst_init(&argc, &argv);

	GstElement *pipeline, *src, *sink;
	GstPad *srcpad, *sinkpad;

	// Create a pipeline
	pipeline = gst_pipeline_new("my-pipeline");

	// Create a source element (e.g., a video source)
	src = gst_element_factory_make("videotestsrc", "source");

	// Create a sink element (e.g., a video sink)
	sink = gst_element_factory_make("autovideosink", "sink");

	// Add elements to the pipeline
	gst_bin_add(GST_BIN(pipeline), src);
	gst_bin_add(GST_BIN(pipeline), sink);

	// Link the source to the sink using ghost pads
	srcpad = gst_element_get_static_pad(src, "src");
	sinkpad = gst_element_get_static_pad(sink, "sink");

	// Create ghost pads
	GstPad *ghost_srcpad = gst_ghost_pad_new("ghost-src", srcpad);
	GstPad *ghost_sinkpad = gst_ghost_pad_new("ghost-sink", sinkpad);

	// Add ghost pads to elements
	gst_element_add_pad(src, ghost_srcpad);
	gst_element_add_pad(sink, ghost_sinkpad);
	// Link the ghost pads
	if (gst_pad_link(ghost_srcpad, ghost_sinkpad) != GST_PAD_LINK_OK) {
		g_print("Failed to link ghost pads.\n");
		return -1;
	}
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Run the GMainLoop (you need to have a running GLib main loop for GStreamer)
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);

	return 0;
}

