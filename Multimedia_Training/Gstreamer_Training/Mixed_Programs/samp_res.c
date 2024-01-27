/*an application for changing the resolution of any video(mp4) to 1080p*/
/*in this videoscale element is key element to resize the video frames 
 * to desired resolution. to set the resolution we use capsfilter element*/


#include<stdio.h>
#include<gst/gst.h>

GstElement *pipeline , *source ,*decodebin,*vconvert,*filter,*vscale,*sink;
/*callbaack function to link pads*/

static void pad_add(GstElement* element ,GstPad *pad , gpointer data)
{
	GstPad *sinkpad = gst_element_get_static_pad(vconvert,"sink");
/*	if(!(gst_pad_link(pad, sinkpad)))
	{
		g_print("failed to link the pads\n");
		return;
	}*/
gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
	gst_pad_link(pad, sinkpad);
}
/*bus callback function for message check*/

static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data)
{
	GMainLoop *loop = (GMainLoop *)data;

	switch (GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_ERROR:
			g_print("Received error message:\n");
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("Error: %s\n", err->message);
			g_main_loop_quit(loop);
			break;
		case GST_MESSAGE_EOS:
			g_print("End of stream reached.\n");
			g_main_loop_quit(loop);
			break;
		default:
			break;
	}
}
int main(int args , char*argv[])
{
	GstBus *bus;
	GMainLoop *loop;
	GstCaps *caps;

	/*initializing the gstreamer*/

	gst_init(&args ,&argv);

	loop = g_main_loop_new(NULL,FALSE);
	
	/*creating elements*/

	pipeline = gst_pipeline_new("mypipeline");
	source = gst_element_factory_make("filesrc","source");
	decodebin = gst_element_factory_make("decodebin","decoder");
	vconvert = gst_element_factory_make("videoconvert","vconverter");
	filter = gst_element_factory_make("capsfilter","filter");
//	vscale = gst_element_factory_make("videoscale","vscale");
	sink = gst_element_factory_make("autovideosink","sink");

	if (!pipeline || !source || !filter || !sink || !vconvert || !decodebin) {
		g_printerr("One or more elements could not be created. Exiting.\n");
		return -100;
	}
	g_object_set(source ,"location",argv[1],NULL);

	caps = gst_caps_new_simple("video/x-raw",
			"width", G_TYPE_INT, 1920,
			"height", G_TYPE_INT, 1080,
			NULL);
	g_object_set(filter ,"caps",caps,NULL); 		//setting the capabilites

	bus = gst_element_get_bus(pipeline);			//bus watch
	gst_bus_add_watch(bus, bus_callback, loop);
	gst_object_unref(bus);
	
	gst_bin_add_many(GST_BIN(pipeline),source , sink,filter,vconvert,decodebin,NULL);

	gboolean ret = gst_element_link(source , decodebin);
	if(ret==FALSE)
	{
		g_print("failed to link elements source->decodebin\n");
		return -100;
	}
	g_signal_connect(decodebin ,"pad-added",G_CALLBACK(pad_add),loop);

	gboolean retu = gst_element_link_many(vconvert,filter,sink,NULL);
	if(retu==FALSE)
	{
		g_print("failed to link elements vconvert->sink\n");
		return -101;
	}

	gst_element_set_state(pipeline , GST_STATE_PLAYING);

	g_main_loop_run(loop);
	
	gst_element_set_state(pipeline,GST_STATE_NULL);
	g_object_unref(pipeline);
	g_main_loop_quit(loop);
	return 0;

}
