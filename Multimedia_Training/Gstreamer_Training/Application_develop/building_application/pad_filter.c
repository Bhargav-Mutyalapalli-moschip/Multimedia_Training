#include <gst/gst.h>

int main(int argc, char *argv[]) 
{
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create a pipeline
	GstElement *pipeline = gst_pipeline_new("my-pipeline");

	// Create a bin to hold the elements
	GstElement *bin = gst_bin_new("my-bin");

	// Create two elements (e.g., source and sink)
	GstElement *element1 = gst_element_factory_make("videotestsrc", "source");
	GstElement *element2 = gst_element_factory_make("autovideosink", "sink");

	// Create a caps object with video properties
	GstCaps *caps = gst_caps_new_simple(
			"video/x-raw",
			"format", G_TYPE_STRING, "I420",
			"width", G_TYPE_INT, 1920,
			"height", G_TYPE_INT, 1080,
			"framerate", GST_TYPE_FRACTION, 25, 1,
			NULL
			);

	// Check if caps creation was successful
	if (!caps) 
	{
		g_print("Failed to create caps\n");
		return 1;
	}

	// Add elements to the bin
	gst_bin_add(GST_BIN(bin), element1);
	gst_bin_add(GST_BIN(bin), element2);

	// Link the elements in the bin using filtered caps
	gboolean link_ok = gst_element_link_pads_filtered(element1, "src",element2, "sink",caps);
	if (link_ok) 
	{
		g_print("Elements linked successfully\n");
	}
	else 
	{
		g_print("Failed to link elements\n");
		return 1;
	}
	// Add the bin to the pipeline
	gst_bin_add(GST_BIN(pipeline), bin);

	// Set the pipeline to the playing state
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Run the GMainLoop (you may need to implement your own)
	// This loop will keep the application running until you stop it manually.

	// Clean up
	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	// Clean up GStreamer
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	gst_caps_unref(caps);
	gst_deinit();

	return 0;
}

