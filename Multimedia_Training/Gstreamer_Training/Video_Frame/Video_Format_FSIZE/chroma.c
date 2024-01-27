
#include <stdio.h>
#include <gst/gst.h>
static void on_pad_added(GstElement *src, GstPad *new_pad, GstElement *sink) {
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_structure = NULL;
    const gchar *new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    // Check the new pad's type
    new_pad_caps = gst_pad_query_caps(new_pad, NULL);
    new_pad_structure = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_structure);

    if (g_str_has_prefix(new_pad_type, "video/x-raw")) {
        // Attempt the link
        ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret)) {
            g_print("Type is '%s' but link failed.\n", new_pad_type);
        } else {
            g_print("Link succeeded (type '%s').\n", new_pad_type);
        }
    } else {
        g_print("It has type '%s' which is not raw video. Ignoring.\n", new_pad_type);
    }

    if (new_pad_caps != NULL)
        gst_caps_unref(new_pad_caps);

    gst_object_unref(sink_pad);
}

static void print_chroma_subsampling_rate(GstCaps *caps) {
    const gchar *chroma_format = NULL;
    const gchar *mime_type = NULL;
    const gchar *media_type = NULL;

    // Iterate through the caps to find video information
    for (guint i = 0; i < gst_caps_get_size(caps); i++) {
        GstStructure *structure = gst_caps_get_structure(caps, i);

        // Check if it's a video structure
        if (gst_structure_has_name(structure, "video/x-raw")) {
            // Extract chroma format
            chroma_format = gst_structure_get_string(structure, "chroma-format");
            // Extract mime type and media type
            mime_type = gst_structure_get_name(structure);
            media_type = gst_structure_get_string(structure, "format");
            break;
        }
    }

    if (chroma_format != NULL) {
        g_print("Chroma Subsampling Rate: %s\n", chroma_format);
        g_print("MIME Type: %s\n", mime_type);
        g_print("Media Type: %s\n", media_type);
    } else {
        g_print("Chroma Subsampling Rate not found.\n");
    }
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *decoder, *sink;
    GstCaps *caps;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create GStreamer elements
    pipeline = gst_pipeline_new("video-player");
    source = gst_element_factory_make("filesrc", "file-source");
    decoder = gst_element_factory_make("decodebin", "decoder");
    sink = gst_element_factory_make("fakesink", "fake-sink");

    if (!pipeline || !source || !decoder || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the input video file
    g_object_set(G_OBJECT(source), "location", "/home/bhargav/Documents/gstreamer_sample/video.mp4", NULL);

    // Build the pipeline
    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), decoder);
    gst_bin_add(GST_BIN(pipeline), sink);

    if (!gst_element_link(source, decoder) ) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Connect to the "pad-added" signal of the decoder
    g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), sink);

    // Start playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait until error or EOS
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                                GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Parse caps from the fakesink
    caps = gst_pad_query_caps(gst_element_get_static_pad(sink, "sink"), NULL);

    if (caps) {
        print_chroma_subsampling_rate(caps);
        gst_caps_unref(caps);
    }

    // Clean up
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

