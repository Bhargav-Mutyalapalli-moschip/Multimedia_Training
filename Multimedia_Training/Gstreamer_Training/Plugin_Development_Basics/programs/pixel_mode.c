#include <gst/gst.h>
#include <glib.h>

static GstPadProbeReturn modify_pixels_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    GstVideoFrame frame;
    
    if (gst_video_frame_map(&frame, GST_PAD_PROBE_INFO_BUFFER(info), GST_VIDEO_FRAME_MAP_FLAG_NO_REF) == GST_MAP_OK) {
        guint8 *data = GST_VIDEO_FRAME_PLANE_DATA(&frame, 0);
        gint stride = GST_VIDEO_FRAME_PLANE_STRIDE(&frame, 0);
        gint width = GST_VIDEO_FRAME_WIDTH(&frame);
        gint height = GST_VIDEO_FRAME_HEIGHT(&frame);
        
        // Modify the pixel values in the first plane (Y plane)
        for (gint y = 0; y < height; y++) {
            for (gint x = 0; x < width; x++) {
                guint8 *pixel = data + y * stride + x;
                // Manipulate the pixel value here (e.g., invert the pixel)
                *pixel = 255 - *pixel;
            }
        }

        gst_video_frame_unmap(&frame);
    }

    return GST_PAD_PROBE_OK;
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *src, *filter, *sink;
    GstPad *pad;
    GMainLoop *loop;

    // Initialize GStreamer
    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);

    // Create the pipeline
    pipeline = gst_pipeline_new("video-processing-pipeline");

    // Create elements
    src = gst_element_factory_make("videotestsrc", "src");
    filter = gst_element_factory_make("capsfilter", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !filter || !sink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set properties for the video source
    g_object_set(G_OBJECT(src), "pattern", 0, NULL); // Set the test pattern (0 is a solid color)
    g_object_set(G_OBJECT(src), "is-live", TRUE, NULL);

    // Set the caps (video format) for the filter
    GstCaps *caps = gst_caps_from_string("video/x-raw");
    g_object_set(G_OBJECT(filter), "caps", caps, NULL);
    gst_caps_unref(caps);

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, filter, sink, NULL);

    // Link elements
    if (!gst_element_link_many(src, filter, sink, NULL)) {
        g_printerr("Elements could not be linked. Exiting.\n");
        return -1;
    }

    // Get the source pad from the filter element
    pad = gst_element_get_static_pad(filter, "src");
    if (!pad) {
        g_printerr("Pad not found. Exiting.\n");
        return -1;
    }

    // Add a probe to modify the pixels in the Y plane
    gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER, modify_pixels_cb, NULL, NULL);

    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    g_print("Running...\n");
    g_main_loop_run(loop);

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

