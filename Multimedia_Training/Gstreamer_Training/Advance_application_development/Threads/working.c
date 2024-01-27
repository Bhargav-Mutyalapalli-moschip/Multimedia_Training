#include <gst/gst.h>
#include "testrtpool.h"

// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *pipeline; 
GstElement *bin; 
GstElement*filesrc; 
GstElement*demuxer; 
GstElement*video_queue; 
GstElement*video_parse; 
GstElement*video_decode; 
GstElement*video_sink; 
GstElement*audio_queue; 
GstElement*audio_parse; 
GstElement*audio_decode; 
GstElement*audio_sink;
static GMainLoop* loop;
GstBus *bus;
GstStreamStatusType type;
GstElement *owner;
const GValue *val;
GstStateChangeReturn ret;
static void on_stream_status (GstBus *bus,GstMessage *message,gpointer user_data)
{
	GstTask *task = NULL;

	gst_message_parse_stream_status (message, &type, &owner);

	val = gst_message_get_stream_status_object (message);

  /* see if we know how to deal with this object */
  if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
    task = g_value_get_object (val);
  }

  switch (type) {
    case GST_STREAM_STATUS_TYPE_CREATE:
      if (task) {
        GstTaskPool *pool;

        pool = test_rt_pool_new();

        gst_task_set_pool (task, pool);
      }
      break;
    default:
      break;
  }
}
static void on_error (GstBus *bus,GstMessage *message,gpointer user_data)
{
  g_message ("received ERROR");
  g_main_loop_quit (loop);
}

static void on_eos (GstBus     *bus,GstMessage *message,gpointer user_data)
{
  g_main_loop_quit (loop);
}
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
	g_object_set (filesrc, "num-buffers", 50, NULL);

	if (!pipeline || !filesrc || !demuxer || !video_queue || !video_parse || !video_decode || !video_sink || !audio_queue || !audio_parse || !audio_decode || !audio_sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	// Set the input file path
	g_object_set(filesrc, "location", "/home/bhargav/Downloads/kgf.mp4", NULL);

	// Add all elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), filesrc, demuxer, video_queue, video_parse, video_decode, video_sink, audio_queue, audio_parse, audio_decode, audio_sink, NULL);

	// Link the elements
	if (!gst_element_link(filesrc, demuxer)) {
		g_printerr("File source and demuxer could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (!gst_element_link_many(video_queue, video_parse, video_decode, video_sink, NULL)) {
		g_printerr("Video elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (!gst_element_link_many(audio_queue, audio_parse, audio_decode, audio_sink, NULL)) {
		g_printerr("Audio elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Connect pad-added signals
	g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), NULL);

	// Set the pipeline state to playing
	//gst_element_set_state(pipeline, GST_STATE_PLAYING);



	loop = g_main_loop_new (NULL, FALSE);
	// Listen for messages on the bus
	GstBus *bus = gst_element_get_bus(pipeline);
	GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* get the bus, we need to install a sync handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (bin));
	gst_bus_enable_sync_message_emission (bus);
	gst_bus_add_signal_watch (bus);

	g_signal_connect (bus, "sync-message::stream-status",(GCallback) on_stream_status, NULL);
	g_signal_connect (bus, "message::error",(GCallback) on_error, NULL);
	g_signal_connect (bus, "message::eos",(GCallback) on_eos, NULL);

	/* start playing */
	ret = gst_element_set_state (bin, GST_STATE_PLAYING);
	if (ret != GST_STATE_CHANGE_SUCCESS) {
		g_message ("failed to change state");
		return -1;
	}

	/* Run event loop listening for bus messages until EOS or ERROR */
	g_main_loop_run (loop);


	/*// Listen for messages on the bus
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
	}*/

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
	} else if (g_str_has_prefix(padname, "audio")) {
		GstPad *sinkpad = gst_element_get_static_pad(audio_queue, "sink");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Audio pad link failed.\n");
		} else {
			g_print("Linked audio pad.\n");
		}
	}
	g_free(padname);
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
}

