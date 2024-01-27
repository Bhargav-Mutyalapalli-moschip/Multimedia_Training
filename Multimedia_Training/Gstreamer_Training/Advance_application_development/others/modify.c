#include<gst/gst.h>

// Callback function to handle pad-added signals
static void on_pad_added1(GstElement *element, GstPad *pad, gpointer data);
static void on_pad_added2(GstElement *element, GstPad *pad, gpointer data);

//elements 
GstElement *pipeline;
GstElement *filesrc1;
GstElement *filesrc2;
GstElement *video_queue;
GstElement *audio_queue;
GstElement *video_encoder;
GstElement *audio_encoder;
GstElement *video_decoder;
GstElement *audio_decoder;
GstElement *mux;
GstElement *demux1;
GstElement *demux2;
GstElement *filesink;

//pads
GstPad *video_src_pad;
GstPad *video_sink_pad;
GstPad *audio_src_pad;
GstPad *audio_sink_pad;

int main(int argc, char *argv[]) {

	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create the GStreamer pipeline and elements
	pipeline = gst_pipeline_new("video-player");

	filesrc1 = gst_element_factory_make("filesrc", "file-source1");
	filesrc2 = gst_element_factory_make("filesrc", "file-source2");

	demux1 = gst_element_factory_make("qtdemux", "demux1");
	demux2 = gst_element_factory_make("qtdemux", "demux2");

	video_queue = gst_element_factory_make("queue", "video-queue");
	audio_queue = gst_element_factory_make("queue", "audio-queue");

	video_decoder = gst_element_factory_make("avdec_h264","h264-decoder");
	audio_decoder = gst_element_factory_make("faad", "aac-decoder");

	video_encoder = gst_element_factory_make("x264enc", "vencoder");
	audio_encoder = gst_element_factory_make("voaacenc", "aencoder");

	mux = gst_element_factory_make("mp4mux", "muxer");

	filesink = gst_element_factory_make("filesink", "file-sink");

	if(!pipeline)
	{
		g_printerr("failed to create pipeline\n");
		return -1;
	}
	if(!filesrc1)
	{
		g_printerr("failed to create filesrc1\n");
		return -1;
	}
	if(!filesrc2)
	{
		g_printerr("failed to create filesrc2\n");
		return -1;
	}
	if(!demux1)
	{
		g_printerr("failed to create demux1\n");
		return -1;
	}
	if(!demux2)
	{
		g_printerr("failed to create demux2\n");
		return -1;
	}
	if(!video_queue)
	{
		g_printerr("failed to create video_queue\n");
		return -1;
	}
	if(!audio_queue)
	{
		g_printerr("failed to create audio_queue\n");
		return -1;
	}
	if(!video_decoder)
	{
		g_printerr("failed to create video_decoder\n");
		return -1;
	}
	if(!audio_decoder)
	{
		g_printerr("failed to create audio_decoder\n");
		return -1;
	}
	if(!video_encoder)
	{
		g_printerr("failed to create video_encoder\n");
		return -1;
	}
	if(!audio_encoder)
	{
		g_printerr("failed to create audio_encoder\n");
		return -1;
	}
	if(!mux)
	{
		g_printerr("failed to create mux\n");
		return -1;
	}
	if(!filesink)
	{
		g_printerr("failed to create filesink\n");
		return -1;
	}

	// Set the input file path
	g_object_set(filesrc1, "location", "/home/pranav/Documents/gstreamer_basic/output_video.mp4", NULL);
	g_object_set(filesrc2, "location", "/home/pranav/Documents/gstreamer_basic/kgf2_telugu.mp4", NULL);

	// Set the output file path
	g_object_set(filesink, "location", "merged.mp4", NULL);

	// Add all elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), filesrc1,demux1,audio_queue,audio_decoder,audio_encoder,NULL);
	gst_bin_add_many(GST_BIN(pipeline), filesrc2,demux2,video_queue,video_decoder,video_encoder,NULL);
	gst_bin_add_many(GST_BIN(pipeline), mux,filesink,NULL);

	// Link the elements
	if (!gst_element_link(filesrc1, demux1)) {
		g_printerr("File source and demuxer could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link_many(audio_queue,audio_decoder,audio_encoder,NULL)) {
		g_printerr("audio queue,audio decoder and audio encoder could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link(filesrc2, demux2)) {
		g_printerr("File source2 and demuxer2 could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link_many(video_queue,video_decoder,video_encoder,NULL)) {
		g_printerr("audio queue,audio decoder and audio encoder could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	if (!gst_element_link(mux,filesink)) {
		g_printerr("File source and demuxer could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	//Generates dot to see the pipeline
	//gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	// Connect pad-added signals
	g_signal_connect(demux1, "pad-added", G_CALLBACK(on_pad_added1), NULL);
	g_signal_connect(demux2, "pad-added", G_CALLBACK(on_pad_added2), NULL);



	//on request pad for encoders and mux element
	video_src_pad=gst_element_get_static_pad(video_encoder,"src");
	if(!video_src_pad)
		g_print("failed to create video decoder source pad\n");
	audio_src_pad=gst_element_get_static_pad(audio_encoder,"src");
	if(!audio_src_pad)
		g_print("failed to create audio decoder source pad\n");
	audio_sink_pad= gst_element_request_pad_simple(mux,"audio_%u");
	if(!audio_sink_pad)
		g_print("failed to create muxer audio  pad\n");
	video_sink_pad=gst_element_request_pad_simple(mux,"video_%u");
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



//first pad added function
// Callback function to handle pad-added signals
static void on_pad_added1(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);
	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "audio_1")) {
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

//second pad added function
// Callback function to handle pad-added signals
static void on_pad_added2(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);
	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "video")) {
		GstPad *sinkpad = gst_element_get_static_pad(video_queue, "sink");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("video pad link failed.\n");
		} else {
			g_print("Linked video pad.\n");
		}
	}
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	g_free(padname);
}
