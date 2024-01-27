#include <gst/gst.h>
#include <string.h>

// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
static void on_pad_added1(GstElement *element, GstPad *pad, gpointer data);
GstElement *pipeline, *filesrc, *demuxer, *video_queue, *video_parse, *video_decode, *video_sink, *audio_queue, *audio_parse, *audio_decode, *audio_sink,*filesrc1,*demuxer1,*video_queue1,*video_decoder1,*video_sink1,*muxer,*video_encoder,*audio_encoder;
GstPad *video_sink_pad,*audio_sink_pad,*video_src_pad,*audio_src_pad;
int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create the GStreamer pipeline and elements

	pipeline = gst_pipeline_new("video-player");
	filesrc = gst_element_factory_make("filesrc", "file-source");
	filesrc1 = gst_element_factory_make("filesrc", "file-source1");
	demuxer = gst_element_factory_make("qtdemux", "demuxer");
	demuxer1 = gst_element_factory_make("qtdemux", "demuxer1");
	video_encoder = gst_element_factory_make("x264enc", "vencoder");
	audio_encoder = gst_element_factory_make("voaacenc", "aencoder");
	muxer = gst_element_factory_make("qtmux", "muxer");
	video_queue = gst_element_factory_make("queue", "video-queue");
	video_queue1 = gst_element_factory_make("queue", "video-queue1");
	video_parse = gst_element_factory_make("h264parse", "h264-parser");
	video_decode = gst_element_factory_make("avdec_h264", "h264-decoder");
	video_decoder1 = gst_element_factory_make("avdec_h264", "h264-decoder1");
	video_sink = gst_element_factory_make("filesink", "video-sink");
	audio_queue = gst_element_factory_make("queue", "audio-queue");
	audio_parse = gst_element_factory_make("aacparse", "aac-parser");
	audio_decode = gst_element_factory_make("faad", "aac-decoder");
	audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");
	/*video_sink = gst_element_factory_make("autovideosink", "video-sink");*/

	if (!pipeline || !filesrc || !demuxer || !video_queue || !video_parse || !video_decode || !video_sink || !audio_queue || !audio_parse || !audio_decode || !audio_sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	// Set the input file path
	g_object_set(filesrc, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/vlc/output_video.mp4", NULL);
	g_object_set(filesrc1, "location", "/home/bhargav/Documents/gstreamer_sample/advance_application_development/kgf2_telugu.mp4", NULL);
	g_object_set(video_sink, "location", "output.mp4", NULL);

	// Add all elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), filesrc, demuxer, video_queue, video_parse, video_decode, video_sink, audio_queue, audio_parse, audio_decode, audio_sink,filesrc1,demuxer1,video_queue1,video_decoder1,muxer,video_encoder,audio_encoder, NULL);

	// Link the elements
	if (!gst_element_link(filesrc, demuxer)) {
		g_printerr("File source and demuxer could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link(filesrc1, demuxer1)) {
		g_printerr("File source and demuxer could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (!gst_element_link_many(video_queue, video_parse, video_decode, NULL)) {
		g_printerr("Video elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link_many(video_queue1, video_decoder1,video_encoder,NULL)) {
		g_printerr("Video elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link_many(muxer, video_sink, NULL)) {
		g_printerr("Video elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (!gst_element_link_many(audio_queue, audio_parse, audio_decode, audio_encoder,NULL)) {
		g_printerr("Audio elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	// Connect pad-added signals
	g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), NULL);
	g_signal_connect(demuxer1, "pad-added", G_CALLBACK(on_pad_added1), NULL);

	
	video_src_pad=gst_element_get_static_pad(video_encoder,"src");
	if(!video_src_pad)
		g_print("failed to create video decoder source pad\n");
	audio_src_pad=gst_element_get_static_pad(audio_encoder,"src");
	if(!audio_src_pad)
		g_print("failed to create audio decoder source pad\n");
	audio_sink_pad= gst_element_request_pad_simple(muxer,"audio_%u");
	if(!audio_sink_pad)
		g_print("failed to create muxer audio  pad\n");
	video_sink_pad=gst_element_request_pad_simple(muxer,"video_%u");
	if(!video_sink_pad)
		g_print("failed to create muxer video pad\n");

	// Link the compatible pads to the queue elements
	if (gst_pad_link(video_src_pad, video_sink_pad) != GST_PAD_LINK_OK) {
		g_printerr("Failed to link video pads.\n");
	}

	if (gst_pad_link(audio_src_pad, audio_sink_pad) != GST_PAD_LINK_OK) {
		g_printerr("Failed to link audio pads.\n");
	}

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

// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);
	if(strcmp(padname,"audio_1") == 0)
	{

		// Check the pad's direction (source or sink) and link it accordingly
		if (g_str_has_prefix(padname, "audio")) {
			GstPad *sinkpad = gst_element_get_static_pad(audio_queue, "sink");
			if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
				g_printerr("Audio pad link failed.\n");
			} else {
				g_print("Linked audio pad.\n");
			}
		}
		gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

		g_free(padname);
	}
}
static void on_pad_added1(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "video")) {
		GstPad *sinkpad = gst_element_get_static_pad(video_queue1, "sink");
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

