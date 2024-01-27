#include <gst/gst.h>

static gboolean on_autoplug_continue(GstElement *element, GstPad *pad, GstCaps *caps, gpointer user_data) {
    GstElement *pipeline = GST_ELEMENT(user_data);

    // Extract the media format from caps
    const gchar *media_format = gst_structure_get_name(gst_caps_get_structure(caps, 0));

    // Create and link the appropriate decoder based on the media format
    GstElement *decoder = NULL;

    if (g_strcmp0(media_format, "audio/mpeg") == 0) {
        decoder = gst_element_factory_make("mad", NULL); // Example: Use the mad plugin for MP3
    } else if (g_strcmp0(media_format, "audio/x-wav") == 0) {
        decoder = gst_element_factory_make("wavparse", NULL); // Example: Use wavparse for WAV
    }

    if (!decoder) {
        g_printerr("Unsupported media format: %s\n", media_format);
        return FALSE;
    }

    // Add and link the decoder to the pipeline
    gst_bin_add(GST_BIN(pipeline), decoder);
    if (!gst_pad_link(pad, gst_element_get_static_pad(decoder,"sink"))) {
        g_printerr("Failed to link pads.\n");
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a new pipeline
    GstElement *pipeline = gst_pipeline_new("custom-pipeline");
    GstElement *filesrc = gst_element_factory_make("filesrc", "file-source");
    GstElement *decodebin = gst_element_factory_make("decodebin", "decodebin");
    GstElement *audioconvert = gst_element_factory_make("audioconvert", "audio-converter");
    GstElement *audiosink = gst_element_factory_make("autoaudiosink", "audio-sink");

    if (!pipeline || !filesrc || !decodebin || !audioconvert || !audiosink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the input file path
    g_object_set(filesrc, "location", "/home/bhargav/Downloads/kgf.mp4", NULL);

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), filesrc, decodebin, audioconvert, audiosink, NULL);

    // Link elements
    if (!gst_element_link(filesrc, decodebin) ||
        !gst_element_link(decodebin, audioconvert) ||
        !gst_element_link(audioconvert, audiosink)) {
        g_printerr("Failed to link elements.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Connect the autoplug-continue signal to the custom callback
    g_signal_connect(decodebin, "autoplug-continue", G_CALLBACK(on_autoplug_continue), pipeline);

    // Set the pipeline state to playing
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to start playback.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Create a main loop to listen for events
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Clean up resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}

