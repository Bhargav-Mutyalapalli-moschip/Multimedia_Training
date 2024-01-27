#include <gst/gst.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    GstElement *pipeline, *source, *demuxer, *audio_queue, *audio_decoder, *audio_sink;
    GstElement *video_queue, *video_decoder, *video_sink, *tee_audio, *tee_video;
    GstPad *tee_audio_pad, *tee_video_pad;

    // Create the pipeline elements
    pipeline = gst_pipeline_new("separate-audio-video");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    audio_decoder = gst_element_factory_make("avdec_aac", "audio-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-output");
    video_queue = gst_element_factory_make("queue", "video-queue");
    video_decoder = gst_element_factory_make("avdec_h264", "video-decoder");
    video_sink = gst_element_factory_make("autovideosink", "video-output");
    tee_audio = gst_element_factory_make("tee", "tee-audio");
    tee_video = gst_element_factory_make("tee", "tee-video");

    if (!pipeline || !source || !demuxer || !audio_queue || !audio_decoder || !audio_sink ||
        !video_queue || !video_decoder || !video_sink || !tee_audio || !tee_video) {
        g_error("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set the input file for the source element
    g_object_set(G_OBJECT(source), "location", "/home/bhargav/Downloads/videor.mp4", NULL);

    // Link the elements together
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, tee_audio, audio_queue, audio_decoder, audio_sink, 
                     tee_video, video_queue, video_decoder, video_sink, NULL);

    if (!gst_element_link(source, demuxer) ||
        !gst_element_link_many(tee_audio, audio_queue, audio_decoder, audio_sink, NULL) ||
        !gst_element_link_many(tee_video, video_queue, video_decoder, video_sink, NULL)) {
        g_error("Elements could not be linked. Exiting.\n");
        return -1;
    }

    // Link the demuxer to the tee elements
    tee_audio_pad = gst_element_get_request_pad(tee_audio, "src_%u");
    tee_video_pad = gst_element_get_request_pad(tee_video, "src_%u");
    gst_pad_link(gst_element_get_static_pad(demuxer, "audio_0"), tee_audio_pad);
    gst_pad_link(gst_element_get_static_pad(demuxer, "video_0"), tee_video_pad);
    gst_object_unref(tee_audio_pad);
    gst_object_unref(tee_video_pad);

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

