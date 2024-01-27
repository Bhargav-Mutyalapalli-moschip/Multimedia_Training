#include<gst/gst.h>

//Element declarations
GstElement *pipeline;
GstElement *source;
GstElement *convert;
GstElement *sink;
GMainLoop *loop;
GstCaps *caps;
GstPad *pad;
gint width, height;


//caps retieve
static void read_video_props (GstCaps *caps)
{
	const GstStructure *str;

	//g_return_if_fail (gst_caps_is_fixed (caps));

	str = gst_caps_get_structure (caps, 0);
	gst_structure_has_field(str,"width");
	gst_structure_has_field(str,"height");
	if (!gst_structure_get_int (str, "width", &width) ||
			!gst_structure_get_int (str, "height", &height)) {
		g_print ("No width/height available\n");
		return;
	}

	g_print ("The video size of this set of capabilities is %dx%d\n",
			width, height);
}
int main(int argc,char *argv[])
{
	//initialize the gstreamer
	gst_init(&argc,&argv);

	//create main loop
	loop=g_main_loop_new(NULL,FALSE);
	g_assert(loop);


	//create a pipeline
	pipeline=gst_pipeline_new("test-pipeline");
	//create an elements 
	source=gst_element_factory_make("videotestsrc","test-source");
	convert=gst_element_factory_make("capsfilter","video-convert");
	sink=gst_element_factory_make("autovideosink","auto-sink");

	g_assert(pipeline);
	g_assert(source);
	g_assert(convert);
	g_assert(sink);

	gst_bin_add_many(GST_BIN(pipeline),source,convert,sink,NULL);

	gst_element_link_many(source,convert,sink,NULL);

	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);


	// Get a pad from the element (use "src" or "sink" as appropriate)
	pad = gst_element_get_static_pad(convert, "src");
	if(!pad)
	{
		g_print("failed to retrieve its pads\n");
	}
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);


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
				g_main_loop_quit(loop);
				g_clear_error(&err);
				g_free(debug_info);
				break;
			case GST_MESSAGE_EOS:
				g_print("End of stream reached.\n");
				g_main_loop_quit(loop);
				break;
			default:
				g_printerr("Unexpected message received.\n");
		}

		gst_message_unref(msg);
	}
	// Query and retrieve the caps from the pad
	caps = gst_pad_get_current_caps(pad);
	if(caps)
	{
		g_message("success to retrieve its caps\n");
	}
	else
		g_message("failed to retrieve its caps\n");

	read_video_props(caps);
	g_main_loop_run(loop);

	// Free resources
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);

	return 0;
}







