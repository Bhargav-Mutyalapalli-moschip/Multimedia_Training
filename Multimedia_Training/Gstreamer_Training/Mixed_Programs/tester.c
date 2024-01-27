#include <gst/gst.h>

static void on_pad_added(GstElement *src, GstPad *new_pad, gpointer data) {
    GstElement *sink = GST_ELEMENT(data);
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);

    if (g_str_has_prefix(new_pad_type, "video/x-h264")) {
        g_print(" Linking video pad to video elements\n");
        ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret)) {
            g_print(" Type is '%s' but link failed.\n", new_pad_type);
        }
    } else if (g_str_has_prefix(new_pad_type, "audio/mpeg")) {
        g_print(" Linking audio pad to audio elements\n");
        ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret)) {
            g_print(" Type is '%s' but link failed.\n", new_pad_type);
        }
    } else {
        g_print(" It has unknown type '%s', ignoring.\n", new_pad_type);
    }

    if (new_pad_caps != NULL) {
        gst_caps_unref(new_pad_caps);
    }
    gst_object_unref(sink_pad);
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *qtdemux, *video_queue, *audio_queue, *h264parse, *avdec_h264, *aacparse, *faad, *videosink, *audiosink;
    GMainLoop *loop;

    gst_init(&argc, &argv);

    loop = g_main_loop_new(NULL, FALSE);

    pipeline = gst_pipeline_new("mypipeline");

    source = gst_element_factory_make("filesrc", "source");
    qtdemux = gst_element_factory_make("qtdemux", "demuxer");
    video_queue = gst_element_factory_make("queue", "video-queue");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    h264parse = gst_element_factory_make("h264parse", "h264parse");
    avdec_h264 = gst_element_factory_make("avdec_h264", "avdec_h264");
    aacparse = gst_element_factory_make("aacparse", "aacparse");
    faad = gst_element_factory_make("faad", "faad");
    videosink = gst_element_factory_make("autovideosink", "videosink");
    audiosink = gst_element_factory_make("autoaudiosink", "audiosink");

    if (!(pipeline && source && qtdemux && video_queue && audio_queue && h264parse && avdec_h264 && aacparse && faad && videosink && audiosink)) {
        g_printerr("Elements could not be created.\n");
        return -1;
    }

    g_object_set(source, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4", NULL);

    gst_bin_add_many(GST_BIN(pipeline), source, qtdemux, video_queue, audio_queue, h264parse, avdec_h264, aacparse, faad, videosink, audiosink, NULL);

    if (!gst_element_link(source, qtdemux)) {
        g_printerr("Failed to link source and qtdemux.\n");
        return -1;
    }

    if (!gst_element_link_many(video_queue, h264parse, avdec_h264, videosink, NULL)) {
        g_printerr("Failed to link video elements.\n");
        return -1;
    }

    if (!gst_element_link_many(audio_queue, aacparse, faad, audiosink, NULL)) {
        g_printerr("Failed to link audio elements.\n");
        return -1;
    }

    g_signal_connect(qtdemux, "pad-added", G_CALLBACK(on_pad_added), audio_queue);
    g_signal_connect(qtdemux, "pad-added", G_CALLBACK(on_pad_added), video_queue);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);

    g_print("Returned, stopping playback\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);

    g_print("Deleting pipeline\n");
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

