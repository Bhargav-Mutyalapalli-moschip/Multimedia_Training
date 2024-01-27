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

static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);
    GstVideoInfo video_info;
    GstVideoFrame vframe;

    if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
            gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {
        guint8 *pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // RGB16 plane
        guint stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0);

        guint pixel_stride=GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,0);
        g_print("The pixel stride:%d\n",pixel_stride);

        gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
        gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);

        // Define the center and radius of the circle
        gint circle_center_x = 100;
        gint circle_center_y = 100;
        gint circle_radius = 50;
        gint border_thickness = 2; // Adjust the thickness of the circle border

        // Loop through the entire frame
        guint h, w;
        for (h = 0; h < height; ++h) {
            for (w = 0; w < width; ++w) {
		    guint8 *pixel = pixels + h * stride + w * pixel_stride;
		    /*gint8 *red=pixel+(0*(5/3));
		    gint8 *green=pixel+(1*(5/3));
		    gint8 *blue=pixel+(2*(5/3));*/
		    // Calculate the distance between the current pixel and the circle's center
		    gint dx = w - circle_center_x;
		    gint dy = h - circle_center_y;
		    double distance = sqrt(dx * dx + dy * dy);

		    // Check if the pixel is on the border of the circle
		    if (fabs(distance - circle_radius) <= border_thickness / 2) {

			    // Set pixel value to white (RGB16 format)
			    pixel[0]=8;
			    pixel[1]=8;
			    pixel[2]=252;


			    
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
    GstElement *sink = gst_element_factory_make("ximagesink", "sink");

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

    // Set the caps on the capsfilter element for RGB16 format
    caps = gst_caps_new_simple("video/x-raw",
		    "format", G_TYPE_STRING, "RGB",
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

