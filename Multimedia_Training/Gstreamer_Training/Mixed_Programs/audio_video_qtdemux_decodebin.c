#include<gst/gst.h>

GMainLoop *loop;
GstElement *pipeline, *source, *demuxer;
GstElement *audio_queue, *audio_decoder, *audio_converter, *audio_sink;
GstElement *video_queue, *video_decoder, *video_converter, *video_sink;
GstBus *bus;
guint bus_watch_id;

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = (GMainLoop *) data;

	switch(GST_MESSAGE_TYPE(msg))
	{
		case GST_MESSAGE_EOS: g_print("End of Stream\n");
				      g_main_loop_quit(loop);
				      break;
		case GST_MESSAGE_ERROR:
				      {
					      gchar *debug;
					      GError *error;

					      gst_message_parse_error(msg, &error, &debug);
					      g_free(debug);

					      g_print("Error: %s\n", error->message);
					      g_error_free(error);

					      g_main_loop_quit(loop);
					      break;
				      }
		default:
				      g_print("Unknown Error\n");
				      break;
	}
	return TRUE;
}

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) 
{
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	if (g_str_has_prefix(padname, "video")) 
	{
		GstPad *sinkpad = gst_element_get_static_pad(video_queue, "sink");

		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");

		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) 
		{
			g_printerr("Video pad link failed.\n");
		} 
		else 
		{
			g_print("Linked video pad.\n");
		}
	} 
	else if (g_str_has_prefix(padname, "audio")) 
	{
		GstPad *sinkpad = gst_element_get_static_pad(audio_queue, "sink");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) 
		{
			g_printerr("Audio pad link failed.\n");
		} 
		else 
		{
			g_print("Linked audio pad.\n");
		}
	}

	g_free(padname);
}

int main(int argc, char *argv[])
{
	gst_init(&argc, &argv);

	if(argc!=2)
	{
		g_print("Kindly give file name too in CLA\n\t./a.out <filename>\n");
		return -1;
	}

	loop = g_main_loop_new(NULL, FALSE);

	pipeline = gst_pipeline_new("my-pipeline");

	source = gst_element_factory_make("filesrc", "source");
	demuxer = gst_element_factory_make("qtdemux", "demuxer");
	audio_queue = gst_element_factory_make("queue", "audio_queue");
	audio_decoder = gst_element_factory_make("decodebin", "audio_decoder");
	audio_converter = gst_element_factory_make("audioconvert", "audio_converter");
	audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
	video_queue = gst_element_factory_make("queue", "video_queue");
	video_decoder = gst_element_factory_make("decodebin", "video_decoder");
	video_converter = gst_element_factory_make("videoconvert", "video_converter");
	video_sink = gst_element_factory_make("autovideosink", "video_sink");

	if(!pipeline || !source || !demuxer || !audio_queue || !audio_decoder || !audio_converter || !audio_sink || !video_queue || !video_decoder || !video_converter || !video_sink)
	{
		g_print("Unable to create one of the element... Exiting...\n");
		return -1;
	}

	g_object_set(source, "location", argv[1], NULL);

	gst_bin_add_many(GST_BIN(pipeline), source, demuxer, audio_queue, audio_decoder, audio_converter, audio_sink, video_queue, video_decoder, video_converter, video_sink, NULL);

	if(!gst_element_link(source, demuxer))
	{
		g_print("Unable to link demuxer and source\n");
		return -1;
	}
	if(!gst_element_link_many(audio_queue, audio_decoder, audio_converter, audio_sink, NULL))
	{
		g_print("Unable to link the elements which are related to audio\n");
		return -1;
	}
	if(!gst_element_link_many(video_queue, video_decoder, video_converter, video_sink, NULL))
	{
		g_print("Unable to link the elements which are related to video\n");
		return -1;
	}

	g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	bus = gst_element_get_bus(pipeline);
	bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
	gst_object_unref(bus);

	g_main_loop_run(loop);

	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;
}
