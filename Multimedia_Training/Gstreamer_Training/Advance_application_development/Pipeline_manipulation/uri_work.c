#include <gst/gst.h>

// Callback function for handling the "pad-added" signal
static void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer user_data) {
    GstElement *sink = (GstElement *)user_data;
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");

    // Check if the new pad is already linked
    if (gst_pad_is_linked(sink_pad)) {
        g_print("We are already linked. Ignoring.\n");
        goto exit;
    }

    // Attempt to link the new pad to the sink pad
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = gst_pad_get_current_caps(new_pad);
    GstStructure *new_pad_structure = gst_caps_get_structure(new_pad_caps, 0);
    const gchar *new_pad_type = gst_structure_get_name(new_pad_structure);

    g_print("Received new pad '%s' with type '%s'\n", GST_PAD_NAME(new_pad), new_pad_type);

    ret = gst_pad_link(new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED(ret)) {
        g_print("Type is '%s' but link failed.\n", new_pad_type);
    } else {
        g_print("Link succeeded (type '%s').\n", new_pad_type);
    }

    gst_caps_unref(new_pad_caps);

exit:
    gst_object_unref(sink_pad);
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a pipeline
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create elements
    GstElement *uri_decode_bin = gst_element_factory_make("uridecodebin", "source");
    GstElement *videoconvert = gst_element_factory_make("videoconvert", "convert");
    GstElement *videoscale = gst_element_factory_make("videoscale", "scale");
    GstElement *autovideosink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !uri_decode_bin || !videoconvert || !videoscale || !autovideosink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), uri_decode_bin, videoconvert, videoscale, autovideosink, NULL);

    // Link elements
    if (!gst_element_link_many(videoconvert, videoscale, autovideosink, NULL)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the URI property for uridecodebin
    g_object_set(uri_decode_bin, "uri", "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm", NULL);

    // Connect the "pad-added" signal of uridecodebin to link it to the rest of the pipeline
    g_signal_connect(uri_decode_bin, "pad-added", G_CALLBACK(pad_added_handler), videoconvert);

    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    gst_message_unref(msg);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}

