#include<gst/gst.h>


GstElement *pipeline;
GstElement *source;
GstElement *sink;
GstElement *filter;
GstElement *bin1,*bin2;
GstPad *src_pad,*sink_pad;

int main(int argc,char *argv[])
{
	//initialize the gstreamer
	gst_init(&argc,&argv);

	//create a pipeline
	pipeline=gst_pipeline_new("my own pipeline");

	//creating elements 
	source=gst_element_factory_make("videotestsrc","videotestsrc1");
	filter=gst_element_factory_make("videoconvert","videoconvert1");
	sink=gst_element_factory_make("autovideosink","videosink1");

	if(!source)
	{
		g_print("failed to create a source element\n");
		return -1;
	}
	if(!filter)
	{
		g_print("failed to create a filter element\n");
		return -1;
	}
	if(!sink)
	{
		g_print("failed to create a sink element\n");
		return -1;
	}


	bin1=gst_bin_new("my first bin");
	bin2=gst_bin_new("my second bin");

	gst_bin_add_many(GST_BIN(bin1),source,NULL);
	//gst_bin_add_many(GST_BIN(bin2),filter,sink,NULL);
	gst_bin_add_many(GST_BIN(pipeline),bin1,bin2,filter,sink,NULL);
	gst_element_link(filter,sink);


	src_pad=gst_element_get_static_pad(source,"src");
	GstPad *ghostpad=gst_ghost_pad_new("ghost-src",src_pad);
	gst_pad_set_active(ghostpad,TRUE);
	gst_element_add_pad(bin1,ghostpad);

	sink_pad=gst_element_get_static_pad(filter,"sink");

	if(gst_pad_link(ghostpad,sink_pad)!= GST_PAD_LINK_OK)
	{
		g_print("failed to link ghostpad and filter sink\n");
		return -1;
	}

	// Set the pipeline state to playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	//Generate the dot file
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
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



