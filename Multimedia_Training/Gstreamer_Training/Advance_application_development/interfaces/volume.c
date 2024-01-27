#include <gst/gst.h>

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);

GstElement *audio_sink,*volume;
int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create a GStreamer pipeline
	GstElement *pipeline = gst_pipeline_new("audio-player");
	GstElement *source = gst_element_factory_make("filesrc", "file-source");
	GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
	volume = gst_element_factory_make("volume", "volume");
	audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");

	if (!pipeline || !source || !decoder || !audio_sink) {
		g_print("Failed to create elements\n");
		return -1;
	}

	// Set the input file location
	g_object_set(source, "location", "/home/pranav/Documents/gstreamer_sample/gst_tools/varma.mp3", NULL);

	// Add elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), source, decoder, volume,audio_sink, NULL);

	// Link elements together
	if (!gst_element_link(source, decoder)) {
		g_print("Failed to link elements\n");
		gst_object_unref(pipeline);
		return -1;
	}
	// Link elements together
	if (!gst_element_link(volume, audio_sink)) {
		g_print("Failed to link elements\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Connect pad-added signals
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), NULL);

	// Set the volume to a value between 0.0 and 1.0
	g_object_set(volume, "volume", 0.5, NULL);

	// Set the pipeline to the playing state
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Listen for messages on the bus
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

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
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(volume, "sink");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Audio pad link failed.\n");
		} else {
			g_print("Linked audio pad.\n");
		}
	}

	g_free(padname);
}
