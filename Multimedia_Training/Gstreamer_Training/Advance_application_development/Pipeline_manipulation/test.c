#include <gst/gst.h>

static gboolean handoff_callback(GstElement *element, GstBuffer *buffer, gpointer user_data) {
    // This callback function will be called when the identity element hands off a buffer.
    // You can manipulate the buffer data here.

    guint buffer_size = gst_buffer_get_size(buffer);

    // Map the buffer for read and write access.
    GstMapInfo map;
    if (!gst_buffer_map(buffer, &map, GST_MAP_READWRITE)) {
        g_printerr("Failed to map buffer.\n");
        return TRUE; // Continue processing other buffers, if any.
    }

    guint8 *data = map.data;
    gint width = 384;
    gint height = 288;
    gint rowstride = GST_ROUND_UP_4(width * 3); // Assuming 3 bytes per pixel (RGB24)

    for (gint y = 0; y < height; y++) {
        for (gint x = 0; x < width; x++) {
            // Invert the RGB values
            data[y * rowstride + x * 3] = 255 - data[y * rowstride + x * 3]; // Red
            data[y * rowstride + x * 3 + 1] = 255 - data[y * rowstride + x * 3 + 1]; // Green
            data[y * rowstride + x * 3 + 2] = 255 - data[y * rowstride + x * 3 + 2]; // Blue
        }
    }

    // Unmap the buffer to finalize changes.
    gst_buffer_unmap(buffer, &map);

    return TRUE; // Returning TRUE means the buffer will continue down the pipeline.
}



int main(int argc, char *argv[]) {
    // Initialize GStreamer.
    gst_init(&argc, &argv);

    // Create the pipeline and elements.
    GstElement *pipeline, *src, *identity, *sink;
    GstBus *bus;
    GMainLoop *loop;

    pipeline = gst_pipeline_new("pipeline");
    src = gst_element_factory_make("videotestsrc", "source");
    identity = gst_element_factory_make("identity", "identity");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !src || !identity || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the silent property of the identity element to FALSE to enable debugging output.
    g_object_set(G_OBJECT(identity), "silent", FALSE, NULL);

    // Add elements to the pipeline.
    gst_bin_add(GST_BIN(pipeline), src);
    gst_bin_add(GST_BIN(pipeline), identity);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Link elements.
    if (!gst_element_link(src, identity) || !gst_element_link(identity, sink)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Connect to the "handoff" signal of the identity element to inspect and manipulate buffers.
    g_signal_connect(identity, "handoff", G_CALLBACK(handoff_callback), NULL);

    // Set the pipeline to the playing state.
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Create and run the main loop.
    bus = gst_element_get_bus(pipeline);
    loop = g_main_loop_new(NULL, FALSE);

    g_main_loop_run(loop);

    // Clean up.
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}

