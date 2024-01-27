#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demuxer, *audio_queue, *audio_sink, *video_queue, *video_sink, *tee;
    GstBus *bus;
    GstMessage *msg;
    GstPad *tee_audio_pad, *tee_video_pad;
    GstPad *queue_audio_pad, *queue_video_pad;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    pipeline = gst_pipeline_new("test-pipeline");
    source = gst_element_factory_make("filesrc", "source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    tee = gst_element_factory_make("tee", "tee");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");
    video_queue = gst_element_factory_make("queue", "video-queue");
    video_sink = gst_element_factory_make("autovideosink", "video-sink");

    if (!pipeline) {
        g_printerr("Not all elements could be created.pipe\n");
        return -1;
    }
    if (!source) {
        g_printerr("Not all elements could be created source.\n");
        return -1;
    }
    if (!demuxer) {
        g_printerr("Not all elements could be created.demux\n");
        return -1;
    }
    if (!tee) {
        g_printerr("Not all elements could be created tee.\n");
        return -1;
    }
    if (!audio_queue) {
        g_printerr("Not all elements could be created audio queue.\n");
        return -1;
    }
    if (!audio_sink) {
        g_printerr("Not all elements could be created audio sink.\n");
        return -1;
    }
    if (!video_queue) {
        g_printerr("Not all elements could be created. video queue\n");
        return -1;
    }
    if (!video_sink) {
        g_printerr("Not all elements could be created video sink.\n");
        return -1;
    }
    /* Set the input media file */
    g_object_set(source, "location", "/home/bhargav/Downloads/videor.mp4", NULL);

    /* Link the elements */
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, tee, audio_queue, audio_sink, video_queue, video_sink, NULL);

    if (!gst_element_link(source, demuxer)) {
        g_printerr("Source and Demuxer could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (!gst_element_link_many(tee, audio_queue, audio_sink, NULL) ||
        !gst_element_link_many(tee, video_queue, video_sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Manually link the Tee, which has "Request" pads */
    tee_audio_pad = gst_element_get_request_pad(tee, "src_%u");
    queue_audio_pad = gst_element_get_static_pad(audio_queue, "sink");
    tee_video_pad = gst_element_get_request_pad(tee, "src_%u");
    queue_video_pad = gst_element_get_static_pad(video_queue, "sink");

    if (gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
        gst_pad_link(tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK) {
        g_printerr("Tee could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Start playing the pipeline */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Release the request pads from the Tee, and unref them */
    gst_element_release_request_pad(tee, tee_audio_pad);
    gst_element_release_request_pad(tee, tee_video_pad);
    gst_object_unref(tee_audio_pad);
    gst_object_unref(tee_video_pad);

    /* Free resources */
    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

