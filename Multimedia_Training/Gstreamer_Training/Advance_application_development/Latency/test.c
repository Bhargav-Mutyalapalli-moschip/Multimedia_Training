#include <gst/gst.h>
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *pipeline, *filesrc, *decoder, *audioconvert, *autoaudiosink;
GMainLoop *loop;
static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer user_data) {
	g_print("bus callback invoked\n");
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


		// Inside your message handler function
		case GST_MESSAGE_BUFFERING: {
						    gint percent = 0;
						    gst_message_parse_buffering(msg, &percent);
						    g_print("Buffering: %d%%\n", percent);

						    if (percent < 100) {
							    // Keep the pipeline in the PAUSED state
							    gst_element_set_state(pipeline, GST_STATE_PAUSED);
						    } else {
							    // Start playback when buffering is complete
							    gst_element_set_state(pipeline, GST_STATE_PLAYING);
						    }
						    break;
					    }

		default:
					    break;
	}
	return TRUE;
}

int main(int argc, char *argv[]) {
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean is_live;
	GstClockTime min_latency, max_latency;

	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create the pipeline and elements
	pipeline = gst_pipeline_new("audio-pipeline");
	filesrc = gst_element_factory_make("souphttpsrc", "file-source");
	decoder = gst_element_factory_make("decodebin", "decoder");
	audioconvert = gst_element_factory_make("audioconvert", "audio-converter");
	autoaudiosink = gst_element_factory_make("autoaudiosink", "audio-sink");

	if (!pipeline || !filesrc || !decoder || !audioconvert || !autoaudiosink) {
		g_print("One or more elements could not be created. Exiting.\n");
		return -1;
	}

	// Set the input file location
	g_object_set(G_OBJECT(filesrc), "location", "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm", NULL);

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

	//Create the main loop
	loop=g_main_loop_new(NULL,FALSE);

	// Set up a bus to watch for messages
	bus = gst_element_get_bus(pipeline);
	gst_bus_add_watch(bus, bus_callback, NULL);
	gst_object_unref(bus);

	// Set the pipeline to the playing state
	  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	  if (ret == GST_STATE_CHANGE_FAILURE) {
	  g_print("Failed to set pipeline to playing state. Exiting.\n");
	  gst_object_unref(pipeline);
	  return -1;
	  }

	// Query latency information from the sink element
	GstQuery *latency_query = gst_query_new_latency();
	if (gst_element_query(autoaudiosink, latency_query)) {
		gst_query_parse_latency(latency_query, &is_live, &min_latency, &max_latency);

		if (is_live) {
			g_print("Element is live.\n");
		} else {
			g_print("Element is not live.\n");
		}

		g_print("Minimum Latency: %" GST_TIME_FORMAT " (%" G_GUINT64_FORMAT " ns)\n",
				GST_TIME_ARGS(min_latency), min_latency);
		g_print("Maximum Latency: %" GST_TIME_FORMAT " (%" G_GUINT64_FORMAT " ns)\n",
				GST_TIME_ARGS(max_latency), max_latency);

		// Calculate and print the overall latency
		GstClockTime overall_latency = max_latency - min_latency;
		g_print("Overall Latency: %" GST_TIME_FORMAT " (%" G_GUINT64_FORMAT " ns)\n",
				GST_TIME_ARGS(overall_latency), overall_latency);
	} else {
		g_print("Failed to query latency information from the sink element.\n");
	}
	gst_query_unref(latency_query);



	g_main_loop_run(loop);

	//Run the main loop
	GstClockTime last_buffer_running_time = GST_CLOCK_TIME_NONE;

	GstBuffer *buffer = NULL;
	GstSample *sample = NULL;


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
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	g_free(padname);
}
