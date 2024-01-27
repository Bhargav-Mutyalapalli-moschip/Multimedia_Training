#include<gst/gst.h>

//own data elements
typedef struct custom
{
	GstElement *pipeline;
	GstElement *source;
	GstElement *decoder;
	GstElement *sink;
	GstPad *pad_src;
	GstPad *pad_sink;
}Customdata;

int main(int argc,char *argv[])
{
	Customdata data;

	//initialization function
	gst_init(&argc,&argv);

	//create a pipeline
	data.pipeline=gst_pipeline_new("test-pipeline");

	if(!data.pipeline)
	{
		g_print("Failed to create pipeline\n");
		return -1;
	}

	//create the elements
	data.source=gst_element_factory_make("videotestsrc","source");
	data.sink=gst_element_factory_make("autovideosink","sink");

	if(!data.source || !data.sink)
	{
		g_print("failed to create the elemenets\n");
		return -1;
	}
	
	//add the elements to the pipeline
	 gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.sink, NULL);

	 //link the source and decodebin
	 gst_element_link(data.source,data.sink);

	 //set the Gobject properties
	 g_object_set(data.source,"pattern",18,NULL);
	 g_object_set(data.source,"background-color","0x00ff0000",NULL);

	 // Set the pipeline state to playing
	 gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	 // Listen for messages on the bus
	 /*GstBus *bus = gst_element_get_bus(data.pipeline);
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
	 gst_element_set_state(data.pipeline, GST_STATE_NULL);
	 gst_object_unref(data.pipeline);*/
	 while(1);

	 return 0;


}


