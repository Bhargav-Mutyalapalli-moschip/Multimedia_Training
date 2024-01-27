#include <gst/gst.h>
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *pipeline, *filesrc, *decoder, *audioconvert, *autoaudiosink;
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
					break;
		default:
					break;
	}
	return TRUE;
}

int main(int argc, char *argv[]) {
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	// Initialize GStreamer
	gst_init(&argc, &argv);

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


	// Set up a bus to watch for messages
	bus = gst_element_get_bus(pipeline);
	//gst_bus_add_watch(bus, bus_callback, NULL);
	gst_object_unref(bus);

	// Set the pipeline to the playing state
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to set pipeline to playing state. Exiting.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Run the GStreamer main loop
	GstPad *sinkpad = gst_element_get_static_pad(autoaudiosink, "sink");
	GstElement *parent = gst_pad_get_parent_element(sinkpad);
	GstClockTime last_buffer_running_time = GST_CLOCK_TIME_NONE;

	GstBuffer *buffer = NULL;
	GstSample *sample = NULL;

	while (TRUE) {
		if (gst_element_get_state(parent, NULL, NULL, GST_CLOCK_TIME_NONE) == GST_STATE_NULL) {
			break;
		}

		msg = gst_bus_poll(bus, GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_STATE_CHANGED, GST_CLOCK_TIME_NONE);

		if (msg != NULL) {
			if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STATE_CHANGED) {
				// State change message
				GstState old_state, new_state, pending_state;
				gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
				if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline) && new_state == GST_STATE_PLAYING) {
					g_print("Pipeline is now in the PLAYING state.\n");
				}
			} else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
				// Error message
				GError *err = NULL;
				gchar *debug_info = NULL;
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
				g_printerr("Debugging information: %s\n", (debug_info) ? debug_info : "none");
				g_clear_error(&err);
				g_free(debug_info);
				break;
			} else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
				// End of stream message
				g_print("End of stream reached.\n");
				break;
			}
			gst_message_unref(msg);
		}

		// Get the buffer running time
		if (gst_pad_pull_range(sinkpad, 0, -1, &buffer) == GST_FLOW_OK) {
			sample = gst_sample_new(buffer, NULL, NULL, NULL);
			if (sample != NULL) {
				GstClockTime buffer_running_time = gst_segment_to_running_time(GST_ELEMENT_CLOCK(pipeline), NULL, sample);
				if (buffer_running_time != last_buffer_running_time) {
					g_print("Buffer Running Time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer_running_time));
					last_buffer_running_time = buffer_running_time;
				}
				gst_sample_unref(sample);
			}
		}

		if (buffer != NULL) {
			gst_buffer_unref(buffer);
		}
	}

	// Free resources
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
