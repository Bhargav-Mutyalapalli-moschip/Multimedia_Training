#include <gst/gst.h>
// Function to handle pad-added signal
static void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer user_data) {
    GstElement *tee = GST_ELEMENT(user_data);
    GstPad *sink_pad = gst_element_get_static_pad(tee, "sink");

    // Check if the pad is already linked
    if (gst_pad_is_linked(sink_pad)) {
        g_object_unref(sink_pad);
        return;
    }

    // Check the pad's capabilities
    GstCaps *new_pad_caps = gst_pad_get_current_caps(new_pad);
    const gchar *new_pad_type = gst_structure_get_name(gst_caps_get_structure(new_pad_caps, 0));

    // Check if the pad is audio or video
    if (g_str_has_prefix(new_pad_type, "audio/x-raw")) {
        // Attempt to link audio pad
        if (gst_pad_link(new_pad, sink_pad) == GST_PAD_LINK_OK) {
            g_print("Audio pad linked.\n");
        } else {
            g_printerr("Failed to link audio pad.\n");
        }
    } else if (g_str_has_prefix(new_pad_type, "video/x-raw")) {
        // Attempt to link video pad
        if (gst_pad_link(new_pad, sink_pad) == GST_PAD_LINK_OK) {
            g_print("Video pad linked.\n");
        } else {
            g_printerr("Failed to link video pad.\n");
        }
    }

    gst_caps_unref(new_pad_caps);
    g_object_unref(sink_pad);
}

// Function to handle pad-added signal
/*static void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer user_data) {
    GstElement *tee = GST_ELEMENT(user_data);
    GstPad *sink_pad = gst_element_get_static_pad(tee, "sink");

    // Check if the pad is already linked
    if (gst_pad_is_linked(sink_pad)) {
        g_object_unref(sink_pad);
        return;
    }

    // Attempt to link the new pad with the tee
    if (gst_pad_link(new_pad, sink_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link pads.\n");
    } else {
        g_print("Pad linked.\n");
    }

    g_object_unref(sink_pad);
}*/

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demuxer, *audio_queue, *video_queue, *audio_decoder, *video_decoder, *audio_sink, *video_sink, *tee_audio, *tee_video;
    GstBus *bus;
    GstMessage *msg;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create GStreamer elements
    pipeline = gst_pipeline_new("video-player");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    tee_audio = gst_element_factory_make("tee", "tee_audio");
    tee_video = gst_element_factory_make("tee", "tee_video");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    video_queue = gst_element_factory_make("queue", "video-queue");
    audio_decoder = gst_element_factory_make("avdec_aac", "audio-decoder");
    video_decoder = gst_element_factory_make("avdec_h264", "video-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-output");
    video_sink = gst_element_factory_make("autovideosink", "video-output");

    if (!pipeline || !source || !demuxer || !tee_audio || !tee_video || !audio_queue || !video_queue || !audio_decoder || !video_decoder || !audio_sink || !video_sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the input file path
    g_object_set(source, "location", "/home/bhargav/Downloads/video.mp4", NULL);

    // Add all elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, tee_audio, tee_video, audio_queue, video_queue, audio_decoder, video_decoder, audio_sink, video_sink, NULL);

    // Link source to demuxer
    if (!gst_element_link(source, demuxer)) {
        g_printerr("Source and Demuxer could not be linked.\n");
        return -1;
    }

    // Connect pad-added signals
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(pad_added_handler), tee_audio);
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(pad_added_handler), tee_video);

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Listen for messages on the bus
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Parse bus messages
    if (msg != NULL) {
        GError *err = NULL;
        gchar *debug_info = NULL;

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End of stream reached.\n");
                break;
            default:
                g_printerr("Unexpected message received.\n");
                break;
        }

        gst_message_unref(msg);
    }

    // Free resources
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

