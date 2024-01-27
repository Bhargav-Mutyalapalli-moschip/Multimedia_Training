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
	data.source=gst_element_factory_make("filesrc","source");
	data.decoder=gst_element_factory_make("decodebin","decoder");
	data.sink=gst_element_factory_make("autovideosink","sink");

	if(!data.source || !data.decoder || !data.sink)
	{
		g_print("failed to create the elemenets\n");
		return -1;
	}
	
	//add the elements to the pipeline
	 gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.decoder, data.sink, NULL);

	 //link the source and decodebin
	 gst_element_link(data.source,data.decoder);

	 //set the Gobject properties
	 g_object_set(data.source,"location","/home/pranav/Documents/gstreamer_basic/kgf2_telugu.mp4",NULL);

	 //create a pad to link the decoder and sink element
	 data.pad_sink=gst_element_get_static_pad(data.sink,"sink");
	 if(!data.pad_sink)
	 {
		 g_print("failed to retrieve the sink pad template capabilities to an element\n");
	 }
	 // Set the pipeline state to playing
	 gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

	 gst_debug_bin_to_dot_file_with_ts(GST_BIN(data.pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
	 data.pad_src=gst_element_get_static_pad(data.decoder,"src_%u");
	 if(!data.pad_src)
	 {
		 g_print("failed to retrieve the source pad template capabilities to an element\n");
	 }



}


