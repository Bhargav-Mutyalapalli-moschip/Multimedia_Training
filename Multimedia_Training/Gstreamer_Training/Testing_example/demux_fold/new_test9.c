#include <gst/gst.h>
// Callback function to handle pad-added signals
GstElement *pipeline, *filesrc, *demuxer, *video_queue, *video_parse, *video_decode, *video_sink, *audio_queue, *audio_parse, *audio_decode, *audio_sink;
/*static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) 
{
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "video")) {
		GstPad *sinkpad = gst_element_get_static_pad(video_queue, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Video pad link failed.\n");
		} else {
			g_print("Linked video pad.\n");
		}
	} else if (g_str_has_prefix(padname, "audio")) {
		GstPad *sinkpad = gst_element_get_static_pad(audio_queue, "sink");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Audio pad link failed.\n");
		} else {
			g_print("Linked audio pad.\n");
		}
	}
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	g_free(padname);
}*/


int main(int argc, char *argv[]) {
	gst_init(&argc, &argv);

	GstBus *bus;
	GstMessage *msg;
	GstPad *video_src_pad, *audio_src_pad;
	GstPad *video_sink_pad, *audio_sink_pad;

	pipeline = gst_pipeline_new("video-player");
	filesrc = gst_element_factory_make("filesrc", "file-source");
	demuxer = gst_element_factory_make("qtdemux", "demuxer");
	video_queue = gst_element_factory_make("queue", "video-queue");
	video_parse = gst_element_factory_make("h264parse", "h264-parser");
	video_decode = gst_element_factory_make("avdec_h264", "h264-decoder");
	video_sink = gst_element_factory_make("autovideosink", "video-sink");
	audio_queue = gst_element_factory_make("queue", "audio-queue");
	audio_parse = gst_element_factory_make("aacparse", "aac-parser");
	audio_decode = gst_element_factory_make("faad", "aac-decoder");
	audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");

	if (!pipeline || !filesrc || !demuxer || !video_queue || !video_parse || !video_decode || !video_sink || !audio_queue || !audio_parse || !audio_decode || !audio_sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	g_object_set(filesrc, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4", NULL);

	gst_bin_add_many(GST_BIN(pipeline), filesrc, demuxer, video_queue, video_parse, video_decode, video_sink, audio_queue, audio_parse, audio_decode, audio_sink, NULL);

	if (!gst_element_link(filesrc, demuxer) ||
			!gst_element_link_many(video_queue, video_parse, video_decode, video_sink, NULL) ||
			!gst_element_link_many(audio_queue, audio_parse, audio_decode, audio_sink, NULL)) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
	// Connect pad-added signals
	// g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), NULL);
	// Get sink pads from the queue elements
	video_src_pad = gst_element_get_static_pad(demuxer, "src");
	if(!video_src_pad)
		g_print("failed to retrieve source pad\n");
	/*video_sink_pad = gst_element_get_static_pad(video_queue, "sink");
	  audio_sink_pad = gst_element_get_static_pad(audio_queue, "sink");

	  if (!video_sink_pad || !audio_sink_pad) {
	  g_printerr("Failed to retrieve static pads for the queue elements.\n");
	  gst_object_unref(pipeline);
	  return -1;
	  }

	  gst_element_set_state(pipeline, GST_STATE_PAUSED); // Pause the pipeline to allow pads to be created

	// Use gst_element_get_compatible_pad to get compatible pads
	video_src_pad = gst_element_get_compatible_pad(demuxer, NULL, video_sink_pad);
	audio_src_pad = gst_element_get_compatible_pad(demuxer, NULL, audio_sink_pad);
	if (!video_src_pad || !audio_src_pad) {
	g_printerr("Failed to retrieve the video or audio src pads from demuxer.\n");
	gst_object_unref(pipeline);
	return -1;
	}

	// Link the pads
	if (gst_pad_link(video_src_pad, video_sink_pad) != GST_PAD_LINK_OK) {
	g_printerr("Failed to link video pads.\n");
	}

	if (gst_pad_link(audio_src_pad, audio_sink_pad) != GST_PAD_LINK_OK) {
	g_printerr("Failed to link audio pads.\n");
	}*/
	/*video_src_pad = gst_element_get_compatible_pad(demuxer, NULL, "video/x-h264");
	  audio_src_pad = gst_element_get_compatible_pad(demuxer, NULL, "audio/mpeg");
	  if (!video_src_pad || !audio_src_pad) {
	  g_printerr("Failed to retrieve video or audio pads from demuxer.\n");
	// Handle the error as needed.
	}*/
	/*video_src_pad = gst_element_request_pad_by_type(demuxer, "video/x-h264", NULL, GST_PAD_SRC, FALSE);
	if (!video_src_pad) {
		g_printerr("Failed to retrieve the video src pads from demuxer.\n");
		// Handle the error as needed.
	}
	audio_src_pad = gst_element_request_pad_by_type(demuxer, "audio/mpeg", NULL, GST_PAD_SRC, FALSE);
	if (!video_src_pad || !audio_src_pad) {
		g_printerr("Failed to retrieve the video or audio src pads from demuxer.\n");
		// Handle the error as needed.
	}*/


	gst_object_unref(video_src_pad);
	gst_object_unref(audio_src_pad);
	gst_object_unref(video_sink_pad);
	gst_object_unref(audio_sink_pad);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

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

	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;
}

