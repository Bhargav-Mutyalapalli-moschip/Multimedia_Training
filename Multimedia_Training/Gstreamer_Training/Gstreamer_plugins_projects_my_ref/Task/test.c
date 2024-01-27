#include <gst/gst.h>
#include <gst/video/video.h>
GstElement *sink;
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

  if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
  gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {
  guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
  guint8 *uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)

  guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
  guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride
  guint pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

        gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
        gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);

        // Define the coordinates and size of the square
        gint square_center_x = 100;
        gint square_center_y = 100;
	gint square_size = 100;

	// Calculate the coordinates of the top-left and bottom-right corners
	gint square_top_left_x = square_center_x - square_size / 2;
	gint square_top_left_y = square_center_y - square_size / 2;
	gint square_bottom_right_x = square_center_x + square_size / 2;
	gint square_bottom_right_y = square_center_y + square_size / 2;

	// Loop through the entire frame
	guint h, w;
	for (h = 0; h < height; ++h) {
		for (w = 0; w < width; ++w) {
                guint8 *y_pixel = y_pixels + h * y_stride + w;
                guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;

                // Check if the pixel is inside the square
                //if (w >= square_center_x - square_size / 2 && w <= square_center_x + square_size / 2 &&
                  //  h >= square_center_y - square_size / 2 && h <= square_center_y + square_size / 2) {

		 // Check if the pixel is on the border of the square
                if ((w == square_top_left_x || w == square_bottom_right_x) &&
                    (h >= square_top_left_y && h <= square_bottom_right_y) ||
                    (h == square_top_left_y || h == square_bottom_right_y) &&
                    (w >= square_top_left_x && w <= square_bottom_right_x)) {
                    // Set Y, U, and V values to white
                    y_pixel[0] = 0; // Y component
                    uv_pixel[0] = 128; // U component (or the first byte of UV)
                    uv_pixel[1] = 128; // V component (or the second byte of UV)
                }
            }
        }

        gst_video_frame_unmap(&vframe);
    }

    return GST_PAD_PROBE_OK;
}
int main(int argc, char *argv[]) 
{
    // Initialize GStreamer
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    // Create the pipeline
    GstElement *pipeline = gst_pipeline_new("video-pipeline");
    GstElement *src = gst_element_factory_make("videotestsrc", "source");
    GstElement *filter = gst_element_factory_make("capsfilter", "filter-caps");
    sink = gst_element_factory_make("filesink", "sink");

    if (!pipeline || !src || !sink) {
        g_printerr("Elements could not be created. Exiting.\n");
        return -1;
    }

    GstCaps *caps;
    // Set up the pipeline
    gst_bin_add(GST_BIN(pipeline), src);
    gst_bin_add(GST_BIN(pipeline), filter);
    gst_bin_add(GST_BIN(pipeline), sink);
    gst_element_link_many(src,filter,sink,NULL);

// Set the caps on the videoconvert element
    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "NV12",
                               "width", G_TYPE_INT, 1920,
                               "height", G_TYPE_INT, 1080,
                               "framerate", GST_TYPE_FRACTION, 24, 1,
                               NULL);
    g_object_set(G_OBJECT(filter), "caps", caps, NULL);

     // Set the output file location for filesink
    g_object_set(G_OBJECT(sink), "location", "output.yuv", NULL);


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

