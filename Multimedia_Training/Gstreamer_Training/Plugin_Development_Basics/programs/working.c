#include <gst/gst.h>
#include <gst/video/video.h>
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);

static GstPadProbeReturn cb_have_data (GstPad *pad,GstPadProbeInfo *info,gpointer user_data)
{
	gint x, y;
	GstMapInfo map;
	GstVideoInfo video_info;
	GstVideoFrame vframe;
	guint16 *ptr, t;
	GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);


	/* Making a buffer writable can fail (for example if it
	 * cannot be copied and is used more than once)
	 */
	 if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) && gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE))
	{
                guint8 *pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0);
		guint planes=GST_VIDEO_FRAME_N_PLANES(&vframe);
		g_print("no of planes:%d\n",planes);
		guint comp_planes=GST_VIDEO_FRAME_N_COMPONENTS(&vframe);
		g_print("no of composite planes:%d\n",comp_planes);


	}
	 gst_video_frame_unmap(&vframe);


	return GST_PAD_PROBE_OK;
}




GstElement *pipeline, *filesrc, *demuxer, *video_queue, *video_parse, *video_decode, *video_sink, *audio_queue, *audio_parse, *audio_decode, *audio_sink;

int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create the GStreamer pipeline and elements

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

	// Set the input file path
	g_object_set(filesrc, "location", "/home/pranav/Documents/gstreamer_basic/kgf2_telugu.mp4", NULL);

	// Add all elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), filesrc, demuxer, video_queue, video_parse, video_decode, video_sink, audio_queue, audio_parse, audio_decode, audio_sink, NULL);

	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
	// Link the elements
	if (!gst_element_link(filesrc, demuxer)) {
		g_printerr("File source and demuxer could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	if (!gst_element_link_many(video_queue, video_parse, video_decode, video_sink, NULL)) {
		g_printerr("Video elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	if (!gst_element_link_many(audio_queue, audio_parse, audio_decode, audio_sink, NULL)) {
		g_printerr("Audio elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	// Connect pad-added signals
	g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added),NULL);
	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	GstPad *pad = gst_element_get_static_pad (filesrc, "src");
	gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) cb_have_data, NULL, NULL);
	gst_object_unref (pad);


	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

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
	} 
	else if (g_str_has_prefix(padname, "audio")) {
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

