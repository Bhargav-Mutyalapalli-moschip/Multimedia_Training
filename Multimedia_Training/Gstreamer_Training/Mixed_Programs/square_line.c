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
/*static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
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
}*/
static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
	GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);
	GstVideoInfo video_info;
	GstVideoFrame vframe;
	if (!gst_buffer_make_writable(buffer)) {
		g_print("Failed to make the buffer writable\n");
		gst_buffer_unref(buffer);
		return -1;
	}


	if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
			gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {
		guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
		guint8 *uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)

		guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
		guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride
		guint pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

		gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
		gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);


		/*--------------Check to transfer -------*/
		// Create a new buffer
		GstBuffer *buffer_new = NULL;
		GstVideoInfo video_info_new;
		GstVideoFrame vframe_new;
		GstMapInfo map;


		/*-----------our own caps-----------*/
		guint width1 = 640; // Set your desired width
		guint height1 = 480; // Set your desired height
		guint bpp = 24; // Bits per pixel
				// Define the coordinates and size of the square
		gint square_center_x = 100;
		gint square_center_y = 100;
		gint square_size = 640;

		/*------------buffer creation----------------*/
		guint8 val = 0;
		guint size = width1*height1*1.5; // Specify the size of the buffer in bytes
		guint offset=size;
		g_print("The size:%d\n",size);

		// Create a caps structure to describe the frame format
		GstCaps *caps = gst_caps_new_simple("video/x-raw",
				"format", G_TYPE_STRING, "NV12",
				"width", G_TYPE_INT, width1,
				"height", G_TYPE_INT, height1,
				"bpp", G_TYPE_INT, 12,
				NULL);
		// Allocate a buffer with the specified size
		buffer_new= gst_buffer_new_allocate(NULL, size, NULL );
		if (!gst_buffer_make_writable(buffer_new)) {
			g_print("Failed to make the buffer writable\n");
			gst_buffer_unref(buffer_new);
			return -1;
		}
		if(gst_video_info_from_caps(&video_info_new, caps))
		{
			g_print("Successfully videoinfo is mapped\n");
		}
		else
		{
			g_print("failure videoinfo is mapped\n");
		}

		if(gst_video_frame_map(&vframe_new, &video_info_new, buffer_new, GST_MAP_WRITE))
		{
			g_print("Successfully videoframe is mapped\n");
		}
		else
		{
			g_print("failure videoframe is mapped\n");
		}
		// Now you can work with the buffer as needed
		// Don't forget to free the buffer when you're done with it
		guint8 *y_pixels_new = GST_VIDEO_FRAME_PLANE_DATA(&vframe_new, 0); // Y plane
		guint8 *uv_pixels_new = GST_VIDEO_FRAME_PLANE_DATA(&vframe_new, 1); // UV plane (interleaved)

		guint y_stride_new = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_new, 0); // Y plane stride
		guint uv_stride_new = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_new, 1); // UV plane stride
		guint pixel_stride_new = 2; // In NV12, UV values are interleaved every 2 bytes

		gint width2 = GST_VIDEO_FRAME_WIDTH(&vframe_new);
		gint height2 = GST_VIDEO_FRAME_HEIGHT(&vframe_new);

		g_print("The width:%d    The height:%d\n",width2,height2);
		g_print("The y stride:%d    The y pixel stride:%d\n",y_stride_new,pixel_stride_new);
		g_print("The pixel stride:%d    The uv stride:%d\n",uv_stride_new,pixel_stride_new);



		// Calculate the coordinates of the top-left and bottom-right corners
		gint square_top_left_x = square_center_x - square_size / 2;
		gint square_top_left_y = square_center_y - square_size / 2;
		gint square_bottom_right_x = square_center_x + square_size / 2;
		gint square_bottom_right_y = square_center_y + square_size / 2;

		// Loop through the entire frame
		guint h, w;
		for (h = square_top_left_y; h < square_bottom_right_y; ++h) 
		{
			for (w = square_top_left_x; w < square_bottom_right_x; ++w) 
			{
				guint8 *y_pixel = y_pixels + h * y_stride + w;
				guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;
				guint8 *y_pixel_new = y_pixels_new + h * y_stride_new + w;
				guint8 *uv_pixel_new = uv_pixels_new + h / 2 * uv_stride_new + (w / 2) * pixel_stride_new;

				// Fill the entire square with the desired color
				//y_pixel[0] = 0;     // Y component
				//uv_pixel[0] = 128; // U component (or the first byte of UV)
				//uv_pixel[1] = 128; // V component (or the second byte of UV)
				y_pixel_new[0]=y_pixel[0];
				uv_pixel_new[0]=uv_pixel[0];
				uv_pixel_new[1]=uv_pixel[1];
				//y_pixel_new[0] = 0;     // Y component
				//uv_pixel_new[0] = 128; // U component (or the first byte of UV)
				//uv_pixel_new[1] = 128; // V component (or the second byte of UV)

			}

		}

		gst_buffer_copy_into(buffer, buffer_new, GST_MEMORY_FLAG_WAS_ALLOCATED, 0, size);
		gst_video_frame_unmap(&vframe);
		gst_buffer_unref(buffer_new);
	}

	return GST_PAD_PROBE_OK;
}
/*static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
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
gint square_center_x = 500;
gint square_center_y = 500;
gint square_size = 100;

// Create a new buffer with the same size as the original buffer
GstBuffer *buffer_new = gst_buffer_copy(buffer);

// Map the new buffer
GstMapInfo map;
if (gst_buffer_map(buffer_new, &map, GST_MAP_WRITE)) {
// Calculate the coordinates of the top-left and bottom-right corners of the square
gint square_top_left_x = square_center_x - square_size / 2;
gint square_top_left_y = square_center_y - square_size / 2;
gint square_bottom_right_x = square_center_x + square_size / 2;
gint square_bottom_right_y = square_center_y + square_size / 2;

// Loop through the entire frame
guint h, w;
for (h = square_top_left_y; h < square_bottom_right_y; ++h) {
for (w = square_top_left_x; w < square_bottom_right_x; ++w) {
guint8 *y_pixel = y_pixels + h * y_stride + w;
guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;

// Copy the data from the original buffer to the new buffer within the square region
//guint offset = h * y_stride + w;
//map.data[offset] = y_pixel[0];
//map.data[width * height + offset] = uv_pixel[0];
//map.data[width * height + width * height / 4 + offset / 2] = uv_pixel[1];
}
}

// Unmap the new buffer
gst_buffer_unmap(buffer_new, &map);

// Replace the original buffer with the new buffer
gst_buffer_replace(&buffer, buffer_new);
}

// Unmap the original frame
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
	GstElement *filter = gst_element_factory_make("capsfilter", "filter-caps");
	GstElement *sink = gst_element_factory_make("filesink", "sink");

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
	g_object_set(G_OBJECT(sink), "location", "output.nv12", NULL);


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

