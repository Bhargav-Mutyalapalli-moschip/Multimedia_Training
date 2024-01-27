#include <gst/gst.h>

static void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer user_data);

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demuxer, *audio_sink, *video_sink;
    GstBus *bus;
    GstMessage *msg;
    GMainLoop *loop;

    // Initialize GStreamer
    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);

    // Create the pipeline
    pipeline = gst_pipeline_new("multimedia-player");

    // Create the source element
    source = gst_element_factory_make("filesrc", "file-source");
    g_assert(source);

    // Create the demuxer element (use qtdemux for MP4 files)
    demuxer = gst_element_factory_make("qtdemux", "qtdemuxer");
    g_assert(demuxer);

    // Set the input file path for the source element
    g_object_set(G_OBJECT(source), "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4", NULL);

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, NULL);

    // Set up a callback for handling pad-added events
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(pad_added_handler), pipeline);

    // Set the pipeline to PLAYING state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Create audio and video sinks
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");
    video_sink = gst_element_factory_make("autovideosink", "video-sink");
    g_assert(audio_sink && video_sink);

    // Add audio and video sinks to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), audio_sink, video_sink, NULL);

    // Link audio and video sinks
    if (!gst_element_link(audio_sink, video_sink)) {
        g_error("Failed to link audio and video sinks.");
        return -1;
    }

    // Start the main loop
    g_print("Running...\n");
    g_main_loop_run(loop);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, gpointer user_data) {
    GstElement *pipeline = GST_ELEMENT(user_data);
    gchar *new_pad_name = gst_pad_get_name(new_pad);

    g_print("Received new pad '%s' from '%s':\n", new_pad_name, GST_ELEMENT_NAME(src));

    if (g_str_has_prefix(new_pad_name, "audio_")) {
        GstElement *audio_queue = gst_element_factory_make("queue", NULL);

        if (!audio_queue) {
            g_error("Failed to create audio queue.");
            return;
        }

        // Add audio queue to the pipeline
        gst_bin_add(GST_BIN(pipeline), audio_queue);

        // Link the new audio pad to the audio queue
        if (gst_pad_link(new_pad, gst_element_get_static_pad(audio_queue, "sink")) != GST_PAD_LINK_OK) {
            g_error("Failed to link audio pad to audio queue.");
        }
    } else if (g_str_has_prefix(new_pad_name, "video_")) {
        GstElement *video_queue = gst_element_factory_make("queue", NULL);

        if (!video_queue) {
            g_error("Failed to create video queue.");
            return;
        }

        // Add video queue to the pipeline
        gst_bin_add(GST_BIN(pipeline), video_queue);

        // Link the new video pad to the video queue
        if (gst_pad_link(new_pad, gst_element_get_static_pad(video_queue, "sink")) != GST_PAD_LINK_OK) {
            g_error("Failed to link video pad to video queue.");
        }
    }

    g_free(new_pad_name);
}

