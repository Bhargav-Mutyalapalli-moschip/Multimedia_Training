#include <gst/gst.h>
#include <gst/video/video.h>
#include <math.h>
static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data) {
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

//Square logic
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
        guint u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // U plane stride
        guint v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 2); // V plane stride
        guint pstride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,0);
        guint pstride_uv = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,1);


        gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
        gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);
	gint planes = GST_VIDEO_FRAME_N_PLANES(&vframe);

	g_print("/*--------------------------------------------------*/\n");
	g_print("The width : %d   The height : %d\n",width,height);
	g_print("The no of planes : %d\n",planes);
	g_print("The y stride : %d   The y pixel stride : %d\n",y_stride,pstride_y);
	g_print("The u stride : %d   The uv pixel stride : %d\n",u_stride,pstride_uv);
	g_print("The v stride : %d   The uv pixel stride : %d\n",v_stride,pstride_uv);
	g_print("/*--------------------------------------------------*/\n");



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
                guint8 *y_pixel = y_pixels + h * y_stride + w * pstride_y;
                guint8 *u_pixel = u_pixels + (h/2)  * u_stride + (w / 2) * pstride_uv;
                guint8 *v_pixel = v_pixels + (h/2)  * v_stride + (w / 2) * pstride_uv;

                // Check if the pixel is inside the square
                if (w >= square_top_left_x && w <= square_bottom_right_x &&
                    h >= square_top_left_y && h <= square_bottom_right_y) {
                    // Set Y, U, and V values to white
                    y_pixel[0] = 0; // Y component
                    u_pixel[0] = 128; // U component 
                    v_pixel[0] = 128; // V component 
                }
            }
        }

        gst_video_frame_unmap(&vframe);
    }

    return GST_PAD_PROBE_OK;
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    // Create the pipeline
    GstElement *pipeline = gst_pipeline_new("video-pipeline");
    GstElement *src = gst_element_factory_make("videotestsrc", "source");
    GstElement *filter = gst_element_factory_make("capsfilter", "filter-caps");
    GstElement *convert = gst_element_factory_make("videoconvert", "video-convert");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !sink || !filter || !convert) {
        g_printerr("Elements could not be created. Exiting.\n");
        return -1;
    }

    GstCaps *caps;
    // Set up the pipeline
    gst_bin_add(GST_BIN(pipeline), src);
    gst_bin_add(GST_BIN(pipeline), filter);
    gst_bin_add(GST_BIN(pipeline), convert);
    gst_bin_add(GST_BIN(pipeline), sink);
    gst_element_link_many(src, filter, convert,sink, NULL);

    // Set the caps on the capsfilter element
    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "NV24",
                               "width", G_TYPE_INT, 640,
                               "height", G_TYPE_INT, 480,
                               NULL);
    g_object_set(G_OBJECT(filter), "caps", caps, NULL);

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

