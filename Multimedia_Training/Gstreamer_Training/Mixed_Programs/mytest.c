#include<stdio.h>
#include<gst/gst.h>

GstElement *source ,*vconvert,*vscale ,*sink,*pipeline;

static void pad_add(GstElement *element ,GstPad *pad ,gpointer data)
{
	/*GstPad *sinkPad = gst_element_get_static_pad(vconvert, "sink");
	GstCaps *caps1 = gst_pad_get_current_caps(sinkPad);
	gchar *capsString = gst_caps_to_string(caps1);*/
	/*GstPad *sink_pad = gst_element_get_static_pad(vconvert, "sink");
	GstCaps *caps = gst_pad_query_caps(sink_pad, NULL);
	gchar *caps_string = gst_caps_to_string(caps);
	g_print("Video caps: %s\n", caps_string);*/
	g_print("callback\n");
	GstPad *sinkpad;
	sinkpad = gst_element_get_static_pad(vconvert,"sink");

	if(gst_pad_link(pad ,sinkpad))
	{
		g_print("failed to link the pads\n");
	}
	return;
}
void main(int args , char *argv[])
{
	GMainLoop *loop;
	gst_init(&args , &argv);
	GstCaps *caps;
	
	loop = g_main_loop_new(NULL,FALSE);

	pipeline = gst_pipeline_new("mypipeline");
	source = gst_element_factory_make("uridecodebin","source");
	vconvert = gst_element_factory_make("videoconvert","videoconvert");
	vscale = gst_element_factory_make("videorate","vscale");
//	sink = gst_element_factory_make("appsink","sink");

	sink = gst_element_factory_make("autovideosink","sink");

	if(!pipeline || !source || !vconvert || !vscale || !sink)
	{
		g_print("failed to create the elements\n");
		return;
	}
	g_object_set(source ,"uri","file:///home/bhargav/Downloads/kgf.mp4",NULL);

	gst_bin_add_many(GST_BIN(pipeline),source ,vconvert,vscale,sink,NULL);

	gboolean ret = gst_element_link_many(vconvert ,vscale,sink,NULL);

	if(ret!=TRUE)
	{
		g_print("failed to link the elements\n");
		return;
	}

	g_signal_connect(source , "pad-added",G_CALLBACK(pad_add),NULL);

	gst_element_set_state(pipeline,GST_STATE_PLAYING);
	

	GstPad *sinkPad = gst_element_get_static_pad(sink, "sink");
	/*GstCaps *caps1 = gst_pad_get_current_caps(sinkPad);
	gchar *capsString = gst_caps_to_string(caps1);
	g_print("Video caps: %s\n", capsString);*/
	// Get the capabilities of the pad
	/*caps = gst_pad_query_caps(sinkPad, NULL);
	gchar *pad_name=gst_pad_get_name(sinkPad);

	if (caps) {
		gchar *caps_str = gst_caps_to_string(caps);
		g_print("Pad '%s' capabilities: %s\n", pad_name, caps_str);
		g_free(caps_str);
		gst_caps_unref(caps);
	} else {
		g_print("Failed to query pad capabilities.\n");
	}*/
	// Query the current capabilities of the sink pad
	GstCaps *currentCaps = NULL;
	GstQuery *query = gst_query_new_caps(NULL);

	if (gst_pad_peer_query(sinkPad, query))
	{
		gst_query_parse_caps(query, &currentCaps);
		if (currentCaps)
		{
			gchar *capsString = gst_caps_to_string(currentCaps);
			g_print("Current Video Caps: %s\n", capsString);

			// Free the caps and caps string
			gst_caps_unref(currentCaps);
			g_free(capsString);
		}
	}
	else
	{
		g_print("Failed to retrieve current video capabilities.\n");
	}

	gst_query_unref(query);


	g_main_loop_run(loop);

	g_object_unref(pipeline);

	g_main_loop_quit(loop);
}
