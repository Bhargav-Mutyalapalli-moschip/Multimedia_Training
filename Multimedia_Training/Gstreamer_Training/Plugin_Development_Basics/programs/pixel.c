#include <gst/gst.h>
#include <gst/video/video.h>

static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data) 
{
	GMainLoop *loop = (GMainLoop *)data;

	switch (GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_ERROR: {
						gchar *error_msg = NULL;
						gst_message_parse_error(message, NULL, &error_msg);
						g_print("Error: %s\n", error_msg);
						g_free(error_msg);
						g_main_loop_quit(loop);
						break;
					}
		case GST_MESSAGE_EOS:
					g_print("End of Stream\n");
					g_main_loop_quit(loop);
					break;
		default:
					break;
	}

	return TRUE;
}

static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
	GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);
	GstVideoInfo video_info;
	GstVideoFrame vframe;
	// Get the GstVideoFormatInfo for a specific video format
	/*GstVideoFormat format = GST_VIDEO_FORMAT_NV12; // Replace with your desired format
	const GstVideoFormatInfo *format_info = gst_video_format_get_info(format);

	if (format_info) {
		guint num_planes = format_info->n_planes;
		g_print("Number of planes for format %s: %u\n", gst_video_format_to_string(format), num_planes);
	} else {
		g_printerr("Failed to get format info. Check if the format is valid.\n");
	}*/

	if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
			gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {
		guint8 *pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0);
		//gint n_planes = gst_video_info_get_n_planes(&video_info);

		//g_print("Number of planes: %d\n", n_planes);

		guint stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0);
		g_print("The stride:%u\n",stride);
		guint pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe, 1);
		g_print("The p stride:%u\n",pixel_stride);
	        guint planes=GST_VIDEO_FRAME_N_PLANES(&vframe);
		g_print("no of planes:%d\n",planes);
		guint comp_planes=GST_VIDEO_FRAME_N_COMPONENTS(&vframe);
		g_print("no of composite planes:%d\n",comp_planes);

		//gint width = video_info.width/2;
		//gint height = video_info.height/2;

		gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
		gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);

		guint h, w;
		for (h = 0; h < height; ++h) {
			for (w = 0; w < width; ++w) {
				//guint8 *u_pixel = pixels + h * stride + w * pixel_stride;
				//guint8 *v_pixel = u_pixel + 1;
				//guint8 *pixel = pixels + h * stride + w;
				//guint8 *pixel = pixels + h * stride + w * pixel_stride * (pixel_stride / 8);


				// Example: Set pixel values to red (assuming RGB format)
				//u_pixel[0] = 0; // Red
				//v_pixel[0] = 0; // blue
				//pixel[0] = 0;
				guint8 *pixel = pixels + h * stride + w * pixel_stride;

				memset (pixel, 0, pixel_stride);

			}
		}

		gst_video_frame_unmap(&vframe);
	}

	return GST_PAD_PROBE_OK;
}

/*static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
  GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);
  GstVideoInfo video_info;
  GstVideoFrame vframe;

  if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
  gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {

  guint8 *u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // U plane
  guint8 *v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 2); // V plane

  guint stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // U and V planes have the same stride
  guint pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe, 1); // Pixel stride for U and V planes

  gint width = video_info.width / 2; // Chroma planes have half the width of the luma plane
  gint height = video_info.height / 2; // Chroma planes have half the height of the luma plane

  guint h, w;
  for (h = 0; h < height; ++h) {
  for (w = 0; w < width; ++w) {
  guint8 *u_pixel = u_pixels + h * stride + w * pixel_stride;
  guint8 *v_pixel = v_pixels + h * stride + w * pixel_stride;

// Example: Set U and V values to 128 (neutral gray)
u_pixel[0] = 0; // U component
v_pixel[0] = 0; // V component
}
}

gst_video_frame_unmap(&vframe);
}

return GST_PAD_PROBE_OK;
}*/

int main(int argc, char *argv[]) 
{
	// Initialize GStreamer
	gst_init(&argc, &argv);

	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	// Create the pipeline
	GstElement *pipeline = gst_pipeline_new("video-pipeline");
	GstElement *src = gst_element_factory_make("videotestsrc", "source");
	GstElement *sink = gst_element_factory_make("autovideosink", "sink");

	if (!pipeline || !src || !sink) {
		g_printerr("Elements could not be created. Exiting.\n");
		return -1;
	}

	// Set up the pipeline
	gst_bin_add(GST_BIN(pipeline), src);
	gst_bin_add(GST_BIN(pipeline), sink);
	gst_element_link(src, sink);

	// Set up a bus watch to handle messages from the pipeline
	GstBus *bus = gst_element_get_bus(pipeline);
	gst_bus_add_watch(bus, bus_callback, loop);
	gst_object_unref(bus);

	// Add a probe to the sink pad of the autovideosink element
	GstPad *sinkpad = gst_element_get_static_pad(sink, "sink");
	gst_pad_add_probe(sinkpad, GST_PAD_PROBE_TYPE_BUFFER, frame_probe, NULL, NULL);
	gst_object_unref(sinkpad);

	// Start the pipeline
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Run the main loop
	g_main_loop_run(loop);

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);
	gst_deinit();

	return 0;
}
