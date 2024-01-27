#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demuxer, *audio_queue, *video_queue, *audio_decoder, *video_decoder, *audio_sink, *video_sink;
    GstBus *bus;
    GstMessage *msg;
    GstPad *audio_pad, *video_pad;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create GStreamer elements
    pipeline = gst_pipeline_new("video-player");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    video_queue = gst_element_factory_make("queue", "video-queue");
    audio_decoder = gst_element_factory_make("faad", "audio-decoder"); // Use 'faad' for AAC audio
    video_decoder = gst_element_factory_make("avdec_h264", "video-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-output");
    video_sink = gst_element_factory_make("autovideosink", "video-output");

    if (!pipeline || !source || !demuxer || !audio_queue || !video_queue || !audio_decoder || !video_decoder || !audio_sink || !video_sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the input file path
    g_object_set(source, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4", NULL);

    // Add all elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, audio_queue, video_queue, audio_decoder, video_decoder, audio_sink, video_sink, NULL);

    // Link source to demuxer
    if (!gst_element_link(source, demuxer)) {
        g_printerr("Source and Demuxer could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Link audio elements
    if (!gst_element_link(audio_decoder, audio_sink)) {
        g_printerr("Audio elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Link video elements
    if (!gst_element_link(video_decoder, video_sink)) {
        g_printerr("Video elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get the pads from the demuxer and link them to the corresponding queues
    audio_pad = gst_element_get_static_pad(demuxer, "audio_0");
    video_pad = gst_element_get_static_pad(demuxer, "video_0");

    if (!audio_pad || !video_pad) {
        g_printerr("Failed to get pads from demuxer.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (gst_pad_link(audio_pad, gst_element_get_static_pad(audio_queue, "sink")) != GST_PAD_LINK_OK ||
        gst_pad_link(video_pad, gst_element_get_static_pad(video_queue, "sink")) != GST_PAD_LINK_OK) {
        g_printerr("Pads could not be linked to queues.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Listen for messages on the bus
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Parse bus messages
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

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

