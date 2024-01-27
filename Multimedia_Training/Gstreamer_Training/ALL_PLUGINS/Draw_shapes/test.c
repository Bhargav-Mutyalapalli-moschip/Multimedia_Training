#include <cairo.h>
#include <gst/video/video.h>
#include <gst/gst.h>

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

// Function to draw a colored border on a Cairo surface
static void draw_circle(cairo_t *cr, double x, double y, double radius, double line_width) {
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);  // Set the border color to red
    cairo_set_line_width(cr, line_width);
    cairo_arc(cr, x, y, radius, 0, 2 * G_PI);  // Draw the circle's border
    cairo_stroke(cr);
}

// Modify the frame_probe function
static GstPadProbeReturn frame_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);
    GstVideoInfo video_info;
    GstVideoFrame vframe;

    if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&
        gst_video_frame_map(&vframe, &video_info, buffer, GST_MAP_WRITE)) {
        guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0);
        guint8 *uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV (chrominance) plane
        guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0);
        guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1);
        guint pixel_stride = 1; // For NV12 format, 1 byte per pixel
        gint width = GST_VIDEO_FRAME_WIDTH(&vframe) / 2; // Adjusted for UV plane
        gint height = GST_VIDEO_FRAME_HEIGHT(&vframe) / 2; // Adjusted for UV plane

        // Create a Cairo surface for drawing on the UV (chrominance) plane
        cairo_surface_t *surface = cairo_image_surface_create_for_data(uv_pixels,
                                                                      CAIRO_FORMAT_A8, // Adjust format as needed
                                                                      width,
                                                                      height,
                                                                      uv_stride);
        cairo_t *cr = cairo_create(surface);

        // Draw a circle on the Cairo surface
        draw_circle(cr, width / 2, height / 2, 50.0, 2.0); // Adjust the circle's position and border thickness as needed

        // Clean up Cairo resources
        cairo_destroy(cr);
        cairo_surface_destroy(surface);

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
    gst_element_link_many(src, filter, convert, sink, NULL);

    // Set the caps on the capsfilter element
    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "NV12",
                               "width", G_TYPE_INT, 1920,
                               "height", G_TYPE_INT, 1080,
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

