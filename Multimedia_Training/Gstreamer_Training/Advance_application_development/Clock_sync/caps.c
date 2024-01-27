#include <gst/gst.h>

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *pipeline, *filesrc, *decodebin, *videoconvert, *videorate, *autovideosink;
int main(int argc, char *argv[]) {
    GstCaps *caps;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the elements
    pipeline = gst_pipeline_new("my-pipeline");
    filesrc = gst_element_factory_make("filesrc", "file-source");
    decodebin = gst_element_factory_make("decodebin", "decoder");
    videoconvert = gst_element_factory_make("videoconvert", "converter");
    videorate = gst_element_factory_make("videorate", "rate-converter");
    autovideosink = gst_element_factory_make("autovideosink", "video-sink");

    if (!pipeline || !filesrc || !decodebin || !videoconvert || !videorate || !autovideosink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set the input file location
    g_object_set(G_OBJECT(filesrc), "location", "/home/bhargav/Downloads/kgf.mp4", NULL);

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), filesrc, decodebin, videoconvert, videorate, autovideosink, NULL);
    if (!gst_element_link_many(filesrc, decodebin, NULL)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    if (!gst_element_link_many(videoconvert, videorate, autovideosink, NULL)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the caps on the videoconvert element
    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "NV12",
                               "width", G_TYPE_INT, 720,
                               "height", G_TYPE_INT, 480,
                               "framerate", GST_TYPE_FRACTION, 24, 1,
                               NULL);
    g_object_set(G_OBJECT(videoconvert), "caps", caps, NULL);
    gst_caps_unref(caps);
    // Connect pad-added signals
    g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), NULL);

    // Set the pipeline to the playing state
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
	    g_print("Failed to set pipeline to playing state. Exiting.\n");
	    gst_object_unref(pipeline);
	    return -1;
    }

    // Watch for messages on the bus
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_poll(bus, GST_MESSAGE_ERROR | GST_MESSAGE_EOS, -1);

    if (msg != NULL) {
	    GError *err = NULL;
	    gchar *debug_info = NULL;

	    switch (GST_MESSAGE_TYPE(msg)) {
		    case GST_MESSAGE_ERROR:
			    gst_message_parse_error(msg, &err, &debug_info);
			    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
			    g_printerr("Debugging information: %s\n", (debug_info) ? debug_info : "none");
			    g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End of stream reached.\n");
                break;
            default:
                g_print("Unexpected message received.\n");
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
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(videoconvert, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Video pad link failed.\n");
		} else {
			g_print("Linked video pad.\n");
		}
	}
	gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	g_free(padname);
}

