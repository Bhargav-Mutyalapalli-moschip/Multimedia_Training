#include <gst/gst.h>

int main (int   argc,char *argv[])
{
	GstElement *bin, *pipeline, *source, *sink,*filter,*capsfilter;

	/* init */
	gst_init (&argc, &argv);

	/* create */
	pipeline = gst_pipeline_new ("my_pipeline");
	bin = gst_bin_new ("my_bin");
	source = gst_element_factory_make ("videotestsrc", "source");
	capsfilter = gst_element_factory_make ("capsfilter", "caps");
	filter = gst_element_factory_make ("croptech", "filter");
	sink = gst_element_factory_make ("autovideosink", "sink");

	// Set properties for videotestsrc
        g_object_set(G_OBJECT(filter), "width", 320, "height", 240, "xco", 0, "yco", 0, NULL);

	// Set the caps property for capsfilter
        GstCaps *caps = gst_caps_from_string("video/x-raw, width=640, height=480,format=YV12");
        g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
	/* First add the elements to the bin */
	gst_bin_add_many (GST_BIN (bin), source, capsfilter,filter,sink, NULL);
	/* add the bin to the pipeline */
	gst_bin_add (GST_BIN (pipeline), bin);

	/* link the elements */
	gst_element_link_many (source, capsfilter,filter,sink,NULL);
	/* Start playing */
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	GstBus *bus;
	GstMessage *msg;
	/* Wait until error or EOS */
	bus = gst_element_get_bus (pipeline);
	msg =gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* See next tutorial for proper error message handling/parsing */
	if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
		g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
				"variable set for more details.");
	}

	/* Free resources */
	gst_message_unref (msg);
	gst_object_unref (bus);
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (pipeline);
	return 0;
}




