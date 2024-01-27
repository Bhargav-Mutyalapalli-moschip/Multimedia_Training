#include <gst/gst.h>

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *autovideosink;
int main(int argc, char *argv[]) {
	gst_init(&argc, &argv);

	if (argc < 2) {
		g_print("Usage: %s <video file>\n", argv[0]);
		return -1;
	}

	const gchar *videoFile = argv[1];

	// Create a GStreamer pipeline
	GstElement *pipeline = gst_pipeline_new("video-pipeline");
	GstElement *filesrc = gst_element_factory_make("filesrc", "file-source");
	GstElement *decodebin = gst_element_factory_make("decodebin", "decoder");
	autovideosink = gst_element_factory_make("autovideosink", "video-sink");

	if (!pipeline || !filesrc || !decodebin || !autovideosink) {
		g_print("One or more elements could not be created. Exiting.\n");
		return -1;
	}

	// Set the input video file
	g_object_set(G_OBJECT(filesrc), "location", videoFile, NULL);

	// Add elements to the pipeline
	gst_bin_add(GST_BIN(pipeline), filesrc);
	gst_bin_add(GST_BIN(pipeline), decodebin);
	gst_bin_add(GST_BIN(pipeline), autovideosink);

	// Link elements
	if (!gst_element_link(filesrc, decodebin)) {
		g_print("Elements could not be linked. Exiting.\n");
		return -1;
	}

	// Connect pad-added signals
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), NULL);

	// Get the color balance interface from the video sink
	GstElement *videosink = gst_bin_get_by_name(GST_BIN(pipeline), "video-sink");
	GstColorBalance *colorbalance = GST_COLOR_BALANCE(videosink);

	if (!colorbalance) {
		g_print("Video sink does not support color balance. Exiting.\n");
		return -1;
	}

	// Set the brightness value (adjust as needed)
	g_object_set(G_OBJECT(colorbalance), "brightness", 0.5, NULL);

	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Create a GLib main loop and run it
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);

	return 0;
}
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(autovideosink, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Video pad link failed.\n");
		} else {
			g_print("Linked video pad.\n");
		}
	}

	g_free(padname);
}

