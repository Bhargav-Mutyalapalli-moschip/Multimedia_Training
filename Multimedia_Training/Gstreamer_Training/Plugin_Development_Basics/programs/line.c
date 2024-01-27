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

    if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
            gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {
        guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
        guint8 *u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // U plane
        guint8 *v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 2); // V plane

        guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
        guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // U and V planes have the same stride
        guint pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe, 1); // Pixel stride for U and V planes

        gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
        gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);

        guint h, w;
        for (h = 0; h < height; ++h) {
            for (w = 0; w < width; ++w) {
                // Set Y, U, and V values to specific values (e.g., full white)
                guint8 *y_pixel = y_pixels + h * y_stride + w * pixel_stride;
                guint8 *u_pixel = u_pixels + h / 2 * uv_stride + w / 2 * pixel_stride;
                guint8 *v_pixel = v_pixels + h / 2 * uv_stride + w / 2 * pixel_stride;

                // Set Y, U, and V values to full white
		if(w == h)
		{
			y_pixel[0] = 255; // Y component
			u_pixel[0] = 64; // U component
			v_pixel[0] = 64; // V component
		}
		if(h == h/4)
			break;
		/*else
		{
			y_pixel[0] = 0; // Y component
			u_pixel[0] = 128; // U component
			v_pixel[0] = 128; // V component
		}*/
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

