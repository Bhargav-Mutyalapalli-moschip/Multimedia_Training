#include<gst/gst.h>

int main(int argc,char *argv[])
{
	GstElement *pipeline, *source, *decode, *sink,*convert,*parse;
	GstBus *bus;
	GstMessage *msg;
	GError *error = NULL;

	/****** gstreamer initialization *****/
	gst_init(&argc, &argv);

	if(argc != 2)
	{
		g_print("give the location of the audio file while running\n");
		return -1;
	}


	//creating pipeline.
	pipeline = gst_pipeline_new("pipeline_to_play_audio");
	if(!pipeline) {
		g_print("failed to create pipeline\n");
		return -1;
	}

	//creating sink element.
	source = gst_element_factory_make("filesrc", "file-source");
	if(!source) {
		g_print("failed to create the source element\n");
		return -1;
	}

	//creating a decodebin.
	//decodebin is the flexible element it selesct the specific decoder which is sutaible for the file.
	decode = gst_element_factory_make("avdec_mp3", "mp3-decoder");
	if(!decode) {
		g_print("failed to create the decode element\n");
		return -1;
	}

	//creating the sink element.
	sink = gst_element_factory_make("autoaudiosink","audio-sink");
	if(!sink) {
		g_print("failed rto create the sink element\n");
		return -1;
	}
	convert = gst_element_factory_make("audioconvert","converter");
	if(!convert) {
		g_print("failed rto create the sink element\n");
		return -1;
	}
	parse = gst_element_factory_make("mpegaudioparse","parser");
	if(!parse) {
		g_print("failed rto create the sink element\n");
		return -1;
	}

	//adding the elements to the pipeline.
	gst_bin_add_many(GST_BIN(pipeline), source, decode, sink,convert,parse, NULL);
	g_object_set(source, "location", argv[1], NULL);

	/*//linking the elements.
	if (!gst_element_link(source, decode)) {
		g_print("1.Elements could not be linked.\n");
		return -1;
	}*/

	if (!gst_element_link_many(source,parse,decode,NULL)) {
		g_print("1.Elements could not be linked.\n");
		return -1;
	}
	if (!gst_element_link_many(decode, convert,sink,NULL)) {
		g_print("2.Elements could not be linked.\n");
		return -1;
	}


	//setting the pipeline state to playing.
	gst_element_set_state(pipeline, GST_STATE_PLAYING);


	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	if(msg !=NULL) {
		GError *err = NULL;
		gchar *debug_info = NULL;

		if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
			gst_message_parse_error(msg, &err, &debug_info);
			g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
			g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
			g_clear_error(&err);
			g_free(debug_info);
			return -1;
		}
		else if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
			g_print("reached to the end of the stream\n");
			return -1;
		}

		else {
			g_print("unkown message recevied\n");
			return -1;
		}
		gst_message_unref(msg);
	}

	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	gst_object_unref(bus);

	return 0;
}
