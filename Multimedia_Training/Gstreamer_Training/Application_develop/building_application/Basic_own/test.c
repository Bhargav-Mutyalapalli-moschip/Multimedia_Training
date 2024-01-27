#include <gst/gst.h>

// Custom data structure
typedef struct custom {
    GstElement *pipeline;
    GstElement *source;
    GstElement *decoder;
    GstElement *sink;
    GstPad *pad_src;
    GstPad *pad_sink;
} Customdata;

// Custom function to link pads
static gboolean link_pads(Customdata *data) {
    GstPad *pad_src = data->pad_src;
    GstPad *pad_sink = data->pad_sink;

    if (!pad_src || !pad_sink) {
        g_print("Source or sink pad is not available!\n");
        return FALSE;
    }

    if (GST_PAD_LINK_SUCCESSFUL(gst_pad_link(pad_src, pad_sink))) {
        g_print("Pads linked successfully!\n");
        return FALSE;  // Return FALSE to stop the timer
    } else {
        g_print("Failed to link pads, retrying...\n");
        return TRUE;   // Return TRUE to continue the timer
    }
}

int main(int argc, char *argv[]) {
    Customdata data;

    // Initialization function
    gst_init(&argc, &argv);

    // Create a pipeline
    data.pipeline = gst_pipeline_new("test-pipeline");

    if (!data.pipeline) {
        g_print("Failed to create pipeline\n");
        return -1;
    }

    // Create the elements
    data.source = gst_element_factory_make("filesrc", "source");
    data.decoder = gst_element_factory_make("decodebin", "decoder");
    data.sink = gst_element_factory_make("autovideosink", "sink");

    if (!data.source || !data.decoder || !data.sink) {
        g_print("Failed to create the elements\n");
        return -1;
    }

    // Add the elements to the pipeline
    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.decoder, data.sink, NULL);

    // Link the source and decodebin
    gst_element_link(data.source, data.decoder);

    // Set the GObject properties
    g_object_set(data.source, "location", "/home/pranav/Documents/gstreamer_basic/kgf2_telugu.mp4", NULL);

    // Set the pipeline state to playing
    gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

    gst_debug_bin_to_dot_file_with_ts(GST_BIN(data.pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    // Retrieve a request pad from the decoder element
    data.pad_src = gst_element_request_pad_simple(data.decoder, "src_%u");
    if (!data.pad_src) {
        g_print("Failed to retrieve the request pad from the decoder\n");
        return -1;
    }

    // Poll the pads until they can be successfully linked
    while (link_pads(&data)) {
        g_usleep(100000);  // Sleep for 100ms
    }

    // Wait for the pipeline to finish
    gst_element_get_state(data.pipeline, NULL, NULL, -1);

    return 0;
}

