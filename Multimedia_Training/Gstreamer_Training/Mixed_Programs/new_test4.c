#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstElementFactory *source_factory;
    GstElement *pipeline, *filesrc, *demuxer, *video_queue, *video_parse, *video_decode, *video_sink, *audio_queue, *audio_parse, *audio_decode, *audio_sink;
    GstBus *bus;
    GstMessage *msg;
    GstPad *video_src_pad, *audio_src_pad;
    GstCaps *video_caps, *audio_caps;

    pipeline = gst_pipeline_new("video-player");
    filesrc = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    video_queue = gst_element_factory_make("queue", "video-queue");
    video_parse = gst_element_factory_make("h264parse", "h264-parser");
    video_decode = gst_element_factory_make("avdec_h264", "h264-decoder");
    video_sink = gst_element_factory_make("autovideosink", "video-sink");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    audio_parse = gst_element_factory_make("aacparse", "aac-parser");
    audio_decode = gst_element_factory_make("faad", "aac-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");

    if (!pipeline || !filesrc || !demuxer || !video_queue || !video_parse || !video_decode || !video_sink || !audio_queue || !audio_parse || !audio_decode || !audio_sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    g_object_set(filesrc, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4", NULL);

    gst_bin_add_many(GST_BIN(pipeline), filesrc, demuxer, video_queue, video_parse, video_decode, video_sink, audio_queue, audio_parse, audio_decode, audio_sink, NULL);

    if (!gst_element_link(filesrc, demuxer) ||
        !gst_element_link_many(video_queue, video_parse, video_decode, video_sink, NULL) ||
        !gst_element_link_many(audio_queue, audio_parse, audio_decode, audio_sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get the caps from the video and audio pad templates
    GstPadTemplate *video_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(demuxer), "video_%u");
    GstPadTemplate *audio_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(demuxer), "audio_%u");
    video_caps = gst_pad_template_get_caps(video_pad_template);
    audio_caps = gst_pad_template_get_caps(audio_pad_template);

    g_print("Video capabilities: %s\n", gst_caps_to_string(video_caps));
    g_print("Audio capabilities: %s\n", gst_caps_to_string(audio_caps));

    // Release the caps
    gst_caps_unref(video_caps);
    gst_caps_unref(audio_caps);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

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
        }

        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

