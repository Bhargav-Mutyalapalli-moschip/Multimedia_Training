#include <gst/gst.h>

static GstPadProbeReturn my_probe_function(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    // This is your custom probe function that can manipulate the sample data.
    // You can modify the sample data in 'info' if needed.
	// Create a sample

	// Convert the sample
	gint x, y;
	GstMapInfo map;
	guint16 *ptr, t;
	GstBuffer *buffer;

	buffer = GST_PAD_PROBE_INFO_BUFFER (info);

	buffer = gst_buffer_make_writable (buffer);

	/* Making a buffer writable can fail (for example if it
	 * cannot be copied and is used more than once)
	 */
	if (buffer == NULL)
		return GST_PAD_PROBE_OK;

	GstCaps *caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "YV12",
                               "width", G_TYPE_INT, 1920,
                               "height", G_TYPE_INT, 1080,
                               NULL);
	GstCaps *curcaps=gst_pad_get_current_caps(pad);
	GstClockTime timeout = GST_CLOCK_TIME_NONE;
	GError *error = NULL;

	GstSample *converted_sample = gst_sample_new (buffer,curcaps,NULL,NULL);

        GstSample *new_sample=gst_video_convert_sample ((GstSample *)converted_sample,caps,timeout,error);
	GstBuffer *buf=gst_sample_get_buffer (new_sample);
	gst_pad_set_caps(pad,caps);
	 GST_PAD_PROBE_INFO_DATA (info) = buf;
	return GST_PAD_PROBE_OK; // Return value indicating the probe was successful.
}

int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create a pipeline
	GstElement *pipeline = gst_pipeline_new("video-convert-pipeline");

	// Create elements for the pipeline, e.g., source, videoconvert, and sink
	GstElement *src = gst_element_factory_make("videotestsrc", NULL);
	GstElement *convert = gst_element_factory_make("videoconvert", NULL);
	GstElement *sink = gst_element_factory_make("autovideosink", NULL);

	// Add elements to the pipeline
	gst_bin_add(GST_BIN(pipeline), src);
	gst_bin_add(GST_BIN(pipeline), convert);
	gst_bin_add(GST_BIN(pipeline), sink);

	// Link the elements
	if (!gst_element_link_many(src, convert, sink, NULL)) {
		g_error("Failed to link elements");
		return -1;
	}

	// Create a probe on the convert element's source pad
	GstPad *convert_src_pad = gst_element_get_static_pad(convert, "src");
	gst_pad_add_probe(convert_src_pad, GST_PAD_PROBE_TYPE_BUFFER, my_probe_function, NULL, NULL);

	// Set the desired caps for the videoconvert element
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
	// Cleanup
	gst_object_unref(convert_src_pad);
	gst_object_unref(pipeline);

	// Uninitialize GStreamer
	gst_deinit();

	return 0;
}

