#include <gst/gst.h>
#include <glib.h>
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *pipeline, *filesrc, *decoder, *audioconvert, *autoaudiosink;
GMainLoop *loop;
guint64 position;

static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer user_data) {
	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR: {
						GError *err;
						gchar *debug_info;
						gst_message_parse_error(msg, &err, &debug_info);
						g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
						g_printerr("Debugging information: %s\n", (debug_info) ? debug_info : "none");
						g_clear_error(&err);
						g_free(debug_info);
						break;
					}
		case GST_MESSAGE_EOS:
					g_print("End of stream reached.\n");
					g_main_loop_quit(loop);
					break;
		default:
					break;
	}
	return TRUE;
}


static gboolean cb_print_position (GstElement *pipeline)
{

  if (gst_element_query_position (pipeline, GST_FORMAT_TIME, &position)) {
    g_print ("Time: %" GST_TIME_FORMAT  "\r",GST_TIME_ARGS (position));
  }

  /* call me again */
  return TRUE;
}

int main(int argc, char *argv[]) {
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	GstSegment segment;
	GstFormat format = GST_FORMAT_TIME;

	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Initialize the segment with your desired values
	gst_segment_init(&segment, format);

	// Create the pipeline and elements
	pipeline = gst_pipeline_new("audio-pipeline");
	filesrc = gst_element_factory_make("filesrc", "file-source");
	decoder = gst_element_factory_make("decodebin", "decoder");
	audioconvert = gst_element_factory_make("audioconvert", "audio-converter");
	autoaudiosink = gst_element_factory_make("autoaudiosink", "audio-sink");

	if (!pipeline || !filesrc || !decoder || !audioconvert || !autoaudiosink) {
		g_print("One or more elements could not be created. Exiting.\n");
		return -1;
	}

	// Set the input file location
	g_object_set(G_OBJECT(filesrc), "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/varma.mp3", NULL);

	// Build the pipeline
	gst_bin_add_many(GST_BIN(pipeline), filesrc, decoder, audioconvert, autoaudiosink, NULL);
	if (!gst_element_link_many(filesrc, decoder, NULL)) {
		g_print("Elements could not be linked. Exiting.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link_many(audioconvert, autoaudiosink, NULL)) {
		g_print("Elements could not be linked. Exiting.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Connect pad-added signals
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), NULL);


	// Create a GMainLoop
	loop = g_main_loop_new(NULL, FALSE);

	// Set up a bus to watch for messages
	bus = gst_element_get_bus(pipeline);
	gst_bus_add_watch(bus, bus_callback, loop);
	gst_object_unref(bus);

	// Set the pipeline to the playing state
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to set pipeline to playing state. Exiting.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	/* run pipeline */
	g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
	// Start the GMainLoop
	g_main_loop_run(loop);

	// Calculate the running time using gst_segment_to_running_time()
	guint64 running_time = gst_segment_to_running_time(&segment, format, position);

	// Print the running time
	g_print("Running Time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(running_time));




	// Run the GStreamer main loop
	GstClockTime last_buffer_running_time = GST_CLOCK_TIME_NONE;

	// Free resources
	g_main_loop_unref(loop);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;
}
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(audioconvert, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Audio pad link failed.\n");
		} else {
			g_print("Linked Audio pad.\n");
		}
	}

	g_free(padname);
}
