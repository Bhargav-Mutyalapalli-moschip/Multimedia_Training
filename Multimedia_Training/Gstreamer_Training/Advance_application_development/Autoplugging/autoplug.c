#include <gst/gst.h>

static gboolean custom_autoplug(GstPad *pad, GstCaps *caps, GstElement *parent) {
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

    /*// Add and link the decoder to the parent element
    gst_bin_add(GST_BIN(parent), decoder);
    if (!gst_element_link_pads(pad, NULL, decoder, "sink")) {
        g_printerr("Failed to link elements.\n");
        return FALSE;
    }*/
    // Add and link the decoder to the parent element
    gst_bin_add(GST_BIN(parent), decoder);
    if (!gst_pad_link(pad, gst_element_get_static_pad(decoder, "sink"))) {
        g_printerr("Failed to link pads.\n");
        return FALSE;
    }

    return TRUE;
}
GstElement *audioconvert;
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
    // Get the pad's name
    gchar *padname = gst_pad_get_name(pad);
    g_print("the pad name :%s\n",padname);

    // Check the pad's direction (source or sink) and link it accordingly
    if (g_str_has_prefix(padname, "src")) {
	    GstPad *sinkpad = gst_element_get_static_pad(audioconvert, "sink");
	    if(!sinkpad)
		    g_print("unsuccesfull retrival of pad info\n");
	    if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
		    g_printerr("Video pad link failed.\n");
	    } else {
		    g_print("Linked video pad.\n");
	    }
    }

    g_free(padname);
}


int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create a new pipeline
	GstElement *pipeline = gst_pipeline_new("custom-pipeline");
	GstElement *filesrc = gst_element_factory_make("filesrc", "file-source");
	GstElement *decoder = NULL;
	audioconvert = gst_element_factory_make("audioconvert", "audio-converter");
	GstElement *audiosink = gst_element_factory_make("autoaudiosink", "audio-sink");

    if (!pipeline || !filesrc || !decoder || !audioconvert || !audiosink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Set the input file path
    g_object_set(filesrc, "location",argv[1], NULL);

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), filesrc, decoder, audioconvert, audiosink, NULL);

    // Set autoplug callback for the decoder
    g_signal_connect(decoder, "autoplug-continue", G_CALLBACK(custom_autoplug), pipeline);

    // Link elements
    if (!gst_element_link(filesrc, decoder) || !gst_element_link(audioconvert, audiosink)) {
        g_printerr("Failed to link elements.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    // Set callback for the decoder
    g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), pipeline);




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

