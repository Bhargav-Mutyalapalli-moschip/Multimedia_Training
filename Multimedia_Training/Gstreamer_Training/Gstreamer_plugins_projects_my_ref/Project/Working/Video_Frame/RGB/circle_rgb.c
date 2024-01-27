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
    GstMapInfo map;
    GstVideoFormat format = GST_VIDEO_FORMAT_UNKNOWN;

    if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad))) {
        format = video_info.finfo->format;
    }

    if (format != GST_VIDEO_FORMAT_RGB) {
        g_print("Unsupported video format. This code works with RGB format.\n");
        return GST_PAD_PROBE_OK;
    }

    if (gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
        guint8 *pixels = map.data;
        guint stride = video_info.width * 3;  // Assuming 3 bytes per pixel for RGB (24-bit)

        gint width = video_info.width;
        gint height = video_info.height;

        // Define the center and radius of the circle
        gint circle_center_x = 100;
        gint circle_center_y = 100;
        gint circle_radius = 50;
        gint border_thickness = 2; // Adjust the thickness of the circle border

        // Loop through the entire frame
        guint h, w;
        for (h = 0; h < height; ++h) {
            for (w = 0; w < width; ++w) {
                guint8 *pixel = pixels + h * stride + w * 3; // RGB has 3 bytes per pixel

                // Calculate the distance between the current pixel and the circle's center
                gint dx = w - circle_center_x;
                gint dy = h - circle_center_y;
                double distance = sqrt(dx * dx + dy * dy);

                // Check if the pixel is on the border of the circle
                if (fabs(distance - circle_radius) <= border_thickness / 2) {
                    // Set RGB values to white (255, 255, 255)
                    pixel[0] = 255; // Red component
                    pixel[1] = 255; // Green component
                    pixel[2] = 255; // Blue component
                }
            }
        }

        gst_buffer_unmap(buffer, &map);
    }

    return GST_PAD_PROBE_OK;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    GstElement *pipeline = gst_pipeline_new("video-pipeline");
    GstElement *src = gst_element_factory_make("videotestsrc", "source");
    GstElement *filter = gst_element_factory_make("capsfilter", "filter-caps");
    GstElement *convert = gst_element_factory_make("videoconvert", "video-convert");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !sink || !filter) {
        g_printerr("Elements could not be created. Exiting.\n");
        return -1;
    }

    GstCaps *caps;
    gst_bin_add(GST_BIN(pipeline), src);
    gst_bin_add(GST_BIN(pipeline), filter);
    gst_bin_add(GST_BIN(pipeline), convert);
    gst_bin_add(GST_BIN(pipeline), sink);
    gst_element_link_many(src, filter, convert,sink, NULL);

    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "RGB",
                               "width", G_TYPE_INT, 640,
                               "height", G_TYPE_INT, 480,
                               NULL);
    g_object_set(G_OBJECT(filter), "caps", caps, NULL);

    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_callback, loop);
    gst_object_unref(bus);

    GstPad *sinkpad = gst_element_get_static_pad(sink, "sink");
    gst_pad_add_probe(sinkpad, GST_PAD_PROBE_TYPE_BUFFER, frame_probe, NULL, NULL);
    gst_object_unref(sinkpad);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);
    gst_deinit();

    return 0;
}

