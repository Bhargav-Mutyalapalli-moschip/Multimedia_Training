#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err = NULL;
            gchar *debug_info = NULL;
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        }
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            break;
    }
    return TRUE;
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer user_data) {
    GstPad *sink_pad = gst_element_get_static_pad(user_data, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    if (!gst_pad_is_linked(sink_pad)) {
        new_pad_caps = gst_pad_query_caps(new_pad, NULL);
        new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
        new_pad_type = gst_structure_get_name(new_pad_struct);

        if (!g_str_has_prefix(new_pad_type, "audio/x-raw")) {
            g_print("  It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
        } else {
            ret = gst_pad_link(new_pad, sink_pad);
            if (GST_PAD_LINK_FAILED(ret)) {
                g_print("  Type is '%s' but link failed.\n", new_pad_type);
            } else {
                g_print("  Link succeeded (type '%s').\n", new_pad_type);
            }
        }
    } else {
        g_print("  We are already linked. Ignoring.\n");
    }

    if (new_pad_caps != NULL)
        gst_caps_unref(new_pad_caps);

    gst_object_unref(sink_pad);
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *decoder, *sink;
    GstBus *bus;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    pipeline = gst_pipeline_new("audio-pipeline");
    source = gst_element_factory_make("filesrc", "source");
    decoder = gst_element_factory_make("decodebin", "decoder");
    sink = gst_element_factory_make("autoaudiosink", "sink");

    if (!pipeline || !source || !decoder || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);

    /* Connect pad-added signal for decoder */
    g_signal_connect(decoder, "pad-added", G_CALLBACK(pad_added_handler), sink);

    /* Modify the source's properties */
    g_object_set(source, "location", "/home/bhargav/Downloads/song.mp3", NULL);

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Watch for messages on the bus */
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_callback, NULL);

    /* Run the main loop */
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}

