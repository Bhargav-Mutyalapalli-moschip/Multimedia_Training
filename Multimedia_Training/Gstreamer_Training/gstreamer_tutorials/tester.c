#include <gst/gst.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    GstElement *pipeline, *source, *demuxer, *audio_queue, *video_queue, *audio_decoder, *video_decoder, *audio_sink, *video_sink, *tee;
    GstPad *audio_pad, *video_pad;

    // Create the pipeline elements
    pipeline = gst_pipeline_new("simultaneous-playback");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    video_queue = gst_element_factory_make("queue", "video-queue");
    audio_decoder = gst_element_factory_make("avdec_aac", "audio-decoder");
    video_decoder = gst_element_factory_make("avdec_h264", "video-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-output");
    video_sink = gst_element_factory_make("autovideosink", "video-output");
    tee = gst_element_factory_make("tee", "tee");

    if (!pipeline || !source || !demuxer || !audio_queue || !video_queue || !audio_decoder || !video_decoder || !audio_sink || !video_sink || !tee) {
        g_error("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set the input file for the source element
    g_object_set(G_OBJECT(source), "location", "/home/bhargav/Downloads/videor.mp4", NULL);

    // Link the elements together
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, tee, audio_queue, audio_decoder, audio_sink, video_queue, video_decoder, video_sink, NULL);
    if (!gst_element_link(source, demuxer)) {
        g_error("Source and Demuxer could not be linked. Exiting.\n");
        return -1;
    }

    if (!gst_element_link_many(audio_queue, audio_decoder, audio_sink, NULL) || !gst_element_link_many(video_queue, video_decoder, video_sink, NULL)) {
        g_error("Audio or video branch elements could not be linked. Exiting.\n");
        return -1;
    }

    // Link the tee to the audio and video queues
    audio_pad = gst_element_get_static_pad(tee, "src0");
    video_pad = gst_element_get_static_pad(tee, "src1");
    gst_pad_link(audio_pad, gst_element_get_static_pad(audio_queue, "sink"));
    gst_pad_link(video_pad, gst_element_get_static_pad(video_queue, "sink"));
    gst_object_unref(audio_pad);
    gst_object_unref(video_pad);

    // Set the pipeline to playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg;
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    g_print("Running...\n");
    g_main_loop_run(loop);

    // Clean up
    g_print("Cleaning up...\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

