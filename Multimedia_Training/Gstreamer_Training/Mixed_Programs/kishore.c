#include<stdio.h>
#include<gst/gst.h>
GstElement *filesrc , *decodebin,*vconvert ,*sink,*pipeline;
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(vconvert, "sink");
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




int main(int args , char * argv[])
{



	GstBus *bus;
	GstCaps *cap;
	GstMessage *msg;



	gst_init(&args , &argv);



	pipeline = gst_pipeline_new("mypipeline");
	filesrc = gst_element_factory_make("filesrc","source");
	decodebin = gst_element_factory_make("decodebin","decoder");
	vconvert = gst_element_factory_make("videoconvert","converter");
	sink = gst_element_factory_make("autovideosink","sink");



	gst_bin_add_many(GST_BIN(pipeline),filesrc,decodebin,vconvert,sink,NULL);

	g_object_set(filesrc, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4", NULL);
	gst_element_link_many(filesrc, decodebin,NULL);

	gst_element_link_many(vconvert,sink,NULL);
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), NULL); 

	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");




	gst_element_set_state (pipeline, GST_STATE_PLAYING);



	bus = gst_element_get_bus (pipeline);
	msg =
		gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
				GST_MESSAGE_ERROR | GST_MESSAGE_EOS);



	/* Parse message */
	if (msg != NULL) {
		GError *err;
		gchar *debug_info;



		switch (GST_MESSAGE_TYPE (msg)) {
			case GST_MESSAGE_ERROR:
				gst_message_parse_error (msg, &err, &debug_info);
				g_printerr ("Error received from element %s: %s\n",
						GST_OBJECT_NAME (msg->src), err->message);
				g_printerr ("Debugging information: %s\n",
						debug_info ? debug_info : "none");
				g_clear_error (&err);
				g_free (debug_info);
				break;
			case GST_MESSAGE_EOS:
				g_print ("End-Of-Stream reached.\n");
				break;
			default:
				/* We should not reach here because we only asked for ERRORs and EOS */
				g_printerr ("Unexpected message received.\n");
				break;
		}
		gst_message_unref (msg);
	}



	gst_object_unref (bus);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (pipeline);
	return 0;



}
