#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demuxer, *audio_queue, *video_queue, *audio_decoder, *video_decoder, *audio_sink, *video_sink, *tee_audio, *tee_video;
    GstBus *bus;
    GstMessage *msg;
    GstPad *demux_audio_pad, *demux_video_pad;
    GstPad *tee_audio_pad, *tee_video_pad;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create GStreamer elements
    pipeline = gst_pipeline_new("video-player");
    source = gst_element_factory_make("filesrc", "file-source");
    demuxer = gst_element_factory_make("matroskademux", "demuxer");
    tee_audio = gst_element_factory_make("tee", "tee_audio");
    tee_video = gst_element_factory_make("tee", "tee_video");
    audio_queue = gst_element_factory_make("queue", "audio-queue");
    video_queue = gst_element_factory_make("queue", "video-queue");
    audio_decoder = gst_element_factory_make("vorbisdec", "audio-decoder");
    video_decoder = gst_element_factory_make("vp8dec", "video-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-output");
    video_sink = gst_element_factory_make("autovideosink", "video-output");

    if (!pipeline || !source || !demuxer || !tee_audio || !tee_video || !audio_queue || !video_queue || !audio_decoder || !video_decoder || !audio_sink || !video_sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the input file path
    g_object_set(source, "location", "/home/bhargav/Downloads/videor.mp4", NULL);

    // Add all elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, tee_audio, tee_video, audio_queue, video_queue, audio_decoder, video_decoder, audio_sink, video_sink, NULL);

    // Link source to demuxer
    if (!gst_element_link(source, demuxer)) {
        g_printerr("Source and Demuxer could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Link audio elements
    if (!gst_element_link_many(tee_audio, audio_queue, audio_decoder, audio_sink, NULL)) {
        g_printerr("Audio elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Link video elements
    if (!gst_element_link_many(tee_video, video_queue, video_decoder, video_sink, NULL)) {
        g_printerr("Video elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get pads and link tee to audio and video queues
     demux_audio_pad = gst_element_get_request_pad(demuxer, "audio_%u");
    //demux_audio_pad = gst_element_get_request_pad(demuxer, "audio_0");
    tee_audio_pad = gst_element_get_static_pad(tee_audio, "sink_%u");
    demux_video_pad = gst_element_get_request_pad(demuxer, "video_%u");
    tee_video_pad = gst_element_get_static_pad(tee_video, "sink_%u");
    if(!demux_audio_pad)
    {
	    g_printerr("failed to retrive from demux audio\n");
    }
    if (gst_pad_link(demux_audio_pad, tee_audio_pad) != GST_PAD_LINK_OK) {
	    g_printerr("Tee could not be linked for audio.\n");
	    gst_object_unref(pipeline);
	    return -1;
    }

    if (gst_pad_link(demux_video_pad, tee_video_pad) != GST_PAD_LINK_OK) {
	    g_printerr("Tee could not be linked for video.\n");
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

