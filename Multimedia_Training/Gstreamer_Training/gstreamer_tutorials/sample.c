#include <gst/gst.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
int main(int argc,char *argv[])
{
	GstElement *pipeline, *source, *sink;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	bool num;

	/* Initialize GStreamer */
	gst_init (&argc, &argv);
	printf("Successfully Gstreamer is Initialized\n");

	/* Create the elements */
	source = gst_element_factory_make ("filesrc", "source");
	if(source == NULL)
	{
		printf("Failed to create the source element\n");
		exit(0);
	}
	sink = gst_element_factory_make ("autovideosink", "sink");
	if(sink == NULL)
	{
		printf("Failed to create the source element\n");
		exit(0);
	}
	printf("Successfully Elements are created\n");
	/* Create the empty pipeline */
	pipeline = gst_pipeline_new ("video-pipeline");
	/* Modify the source's properties */
	g_object_set (G_OBJECT(source), "location", "/home/bhargav/Downloads/video1.mp4", NULL);
	gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
	num=gst_element_link(source,sink);
	if (num != TRUE) 
	{
		g_printerr ("Elements could not be linked.\n");
		gst_object_unref (pipeline);
		return -1;
	}

	/* Start playing */
	ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the playing state.\n");
		gst_object_unref (pipeline);
		return -1;
	}
	/* Wait until error or EOS */
	bus = gst_element_get_bus (pipeline);
	msg =
		gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
				GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* Parse message */
	if (msg != NULL) {
		GError *err;
		gchar *debug_info;

	/*	switch (GST_MESSAGE_TYPE (msg)) {
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
				// We should not reach here because we only asked for ERRORs and EOS 
				g_printerr ("Unexpected message received.\n");
				break;
		}*/
		gst_message_unref (msg);
	}

	/* Free resources */
	gst_object_unref (bus);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (pipeline);
	return 0;


}

