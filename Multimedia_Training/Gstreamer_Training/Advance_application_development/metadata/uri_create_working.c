#include <gst/gst.h>
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);

GstElement *pipeline, *source, *sink,*decoder;
int main (int argc, char *argv[]) {

	/* Initialize GStreamer */
	gst_init (&argc, &argv);

	/* Create a pipeline */
	pipeline = gst_pipeline_new ("my-pipeline");

	/* Create a source element from the given URI */
	source = gst_element_make_from_uri (GST_URI_SRC,"file:///home/bhargav/Documents/gstreamer_sample/video.mp4",NULL,NULL);

	/* Create a sink element */
	sink = gst_element_factory_make ("autovideosink", NULL);
	decoder = gst_element_factory_make ("decodebin", NULL);

	/* Add the source and sink elements to the pipeline */
	gst_bin_add (GST_BIN (pipeline), source);
	gst_bin_add (GST_BIN (pipeline), decoder);
	gst_bin_add (GST_BIN (pipeline), sink);

	/* Link the source element's pad to the sink element's pad */
	gst_element_link (source, decoder);


	// Connect pad-added signals
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), NULL);

	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Listen for messages on the bus
	GstBus *bus = gst_element_get_bus(pipeline);
	GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	// Parse bus messages
	if (msg != NULL) {
		GError *err = NULL;
		gchar *debug_info = NULL;

		switch (GST_MESSAGE_TYPE(msg)) {
			case GST_MESSAGE_ERROR:
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
				g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
				g_clear_error(&err);
				g_free(debug_info);
				break;
			case GST_MESSAGE_EOS:
				g_print("End of stream reached.\n");
				break;
			default:
				g_printerr("Unexpected message received.\n");
		}

		gst_message_unref(msg);
	}

	// Free resources
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;

}
//Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(sink, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Video pad link failed.\n");
		} else {
			g_print("Linked video pad.\n");
		}
	}
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	g_free(padname);
}
