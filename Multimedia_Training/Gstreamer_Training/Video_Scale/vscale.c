#include <gst/gst.h>
#include <gst/video/video.h>

int main(int argc, char *argv[]) 
{
	GMainLoop *loop;
	GstElement *pipeline, *source, *videosink, *scale;
	GstBus *bus;
	GstMessage *msg;

	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create the main loop
	loop = g_main_loop_new(NULL, FALSE);

	// Create the pipeline
	pipeline = gst_pipeline_new("video-scaler-pipeline");

	// Create the video source element (replace 'videotestsrc' with your source)
	source = gst_element_factory_make("videotestsrc", "source");

	// Create the video scaler element
	GstVideoScaler *scaler = gst_video_scaler_new(GST_VIDEO_RESAMPLER_METHOD_NEAREST, GST_VIDEO_SCALER_FLAG_NONE, 0, 320, 1080, NULL);

	// Create the video sink element (replace 'autovideosink' with your sink)
	scale = gst_element_factory_make("videoscale", "video-scale");
	videosink = gst_element_factory_make("autovideosink", "videosink");

	    g_object_set(G_OBJECT(scale), "scaler", scaler, NULL);


	if (!pipeline || !source || !scale || !videosink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	// Set up the pipeline
	gst_bin_add(GST_BIN(pipeline), source);
	gst_bin_add(GST_BIN(pipeline), scale);
	gst_bin_add(GST_BIN(pipeline), videosink);

	if (!gst_element_link(source, scale) || !gst_element_link(scale, videosink)) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Start the pipeline
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Run the main loop
	g_print("Running...\n");
	g_main_loop_run(loop);

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);

	return 0;
}

