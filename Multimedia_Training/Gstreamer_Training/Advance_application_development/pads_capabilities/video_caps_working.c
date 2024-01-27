#include<gst/gst.h>

//callback for dynamic pads
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);

//ELment declarations
GstElement *source;
GstElement *decoder;
GstElement *vconvert;
GstElement *sink;
GstElement *pipeline;
GstElement *filter;
GMainLoop *loop;
GstCaps *caps;
GstPad *pad;
//function to print caps
static void read_video_props (GstCaps *caps)
{
  gint width, height;
  const GstStructure *str;

  g_return_if_fail (gst_caps_is_fixed (caps));

  str = gst_caps_get_structure (caps, 0);
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
	source=gst_element_factory_make("filesrc","test-source");
	decoder=gst_element_factory_make("decodebin","test-decoder");
	vconvert=gst_element_factory_make("videoconvert","video-convert");
	filter=gst_element_factory_make("capsfilter","caps-filter");
	sink=gst_element_factory_make("autovideosink","auto-sink");

	g_assert(pipeline);
	g_assert(source);
	g_assert(decoder);
	g_assert(vconvert);
	g_assert(filter);
	g_assert(sink);

	//set the properties
	g_object_set(source,"location","/home/pranav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4",NULL);

	//add it into pipeline
	gst_bin_add_many(GST_BIN(pipeline),source,decoder,vconvert,filter,sink,NULL);

	//link the elements
	gst_element_link_many(source,decoder,NULL);
	gst_element_link_many(vconvert,filter,sink,NULL);

	//loop creation
	loop=g_main_loop_new(NULL,FALSE);

	//emit the signal
	g_signal_connect(decoder,"pad-added",G_CALLBACK(on_pad_added),NULL);

	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Retrieve the current caps from the capsfilter
	//g_object_get(G_OBJECT(filter), "caps", &caps, NULL);

	//read_video_props(caps);

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
				exit(1);
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
	g_main_loop_run(loop);

	// Free resources
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);

	return 0;

}
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);
	gchar *caps_str=NULL;
	caps = gst_pad_get_current_caps(pad);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(vconvert, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Video pad link failed.\n");
			// Retrieve the negotiated caps

			if (caps) {
			}
			caps_str = gst_caps_to_string(caps);
			g_print("Negotiated caps:\n%s\n", caps_str);

			read_video_props(caps);
			gst_caps_unref(caps);
		} else {
			caps_str = gst_caps_to_string(caps);
			g_print("Negotiated caps:\n%s\n", caps_str);

			read_video_props(caps);
			g_print("Linked video pad.\n");
		}
	}
	g_free(padname);
}
