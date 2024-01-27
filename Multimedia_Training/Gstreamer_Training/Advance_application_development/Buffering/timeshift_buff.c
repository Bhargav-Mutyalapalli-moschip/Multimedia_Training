#include <gst/gst.h>
#include <glib.h>

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
#define BUFFER_DURATION_SECONDS 30

GstElement *pipeline;
GstElement *source;
GstElement *decoder;
GstElement *tee;
GstElement *queue;
GstElement *timeshift_queue;
GstElement *sink;

GstElement *timeshift_buffer;
GstPad *tee_src_pad;
GstPad *queue_sink_pad;

gboolean seek_requested = FALSE;

// Callback function for seek events
static gboolean SeekEventCallback(GstElement *element, GstEvent *event, gpointer user_data) {
    gint64 seek_position;
    GstFormat format = GST_FORMAT_TIME;

    gst_event_parse_seek(event, &seek_position);

    // Perform the seek in the timeshift buffer
    gst_element_seek(timeshift_buffer, 1.0, format, GST_SEEK_FLAG_FLUSH,
                     GST_SEEK_TYPE_SET, seek_position, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);

    return TRUE;
}

// Callback function for bus messages
static gboolean BusCallback(GstBus *bus, GstMessage *msg, gpointer user_data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            g_print("Received error message: %s\n", GST_ERROR_MESSAGE(msg));
            g_main_loop_quit((GMainLoop *)user_data);
            break;
        }
        case GST_MESSAGE_ELEMENT: {
            const GstStructure *structure = gst_message_get_structure(msg);
            const gchar *name = gst_structure_get_name(structure);
            
            // Handle custom seek events
            if (g_strcmp0(name, "seek-event") == 0) {
                gst_element_post_message(timeshift_buffer, gst_message_ref(msg));
                break;
            }
        }
        default:
            break;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    pipeline = gst_pipeline_new("timeshift-pipeline");
    source = gst_element_factory_make("filesrc", "source");
    decoder = gst_element_factory_make("decodebin", "decoder");
    tee = gst_element_factory_make("tee", "tee");
    queue = gst_element_factory_make("queue", "queue");
    timeshift_queue = gst_element_factory_make("queue", "timeshift-queue");
    timeshift_buffer = gst_element_factory_make("queue", "timeshift-buffer");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !decoder || !tee || !queue || !timeshift_queue || !timeshift_buffer || !sink) {
        g_print("Elements could not be created. Exiting.\n");
        return -1;
    }

    // Set up the pipeline
    g_object_set(source, "location", "your_video_file.mp4", NULL);
    g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), NULL);

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, tee, queue, timeshift_queue, timeshift_buffer, sink, NULL);

    if (!gst_element_link_many(source, decoder, NULL) ||
        !gst_element_link_many(queue, sink, NULL) ||
        !gst_element_link(timeshift_queue, timeshift_buffer) ||
        !gst_element_link(timeshift_buffer, queue)) {
        g_print("Elements could not be linked. Exiting.\n");
        return -1;
    }

    // Connect pad-added signals
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), NULL);

    // Set the queue size for timeshift buffering
    g_object_set(timeshift_buffer, "max-size-time", G_GINT64_CONSTANT(BUFFER_DURATION_SECONDS * GST_SECOND), NULL);

    // Get the tee source pad
    tee_src_pad = gst_element_get_request_pad(tee, "src_%u");

    // Link the tee to the timeshift queue
    queue_sink_pad = gst_element_get_static_pad(timeshift_queue, "sink");
    gst_pad_link(tee_src_pad, queue_sink_pad);
    gst_object_unref(tee_src_pad);
    gst_object_unref(queue_sink_pad);

    // Set up bus watch
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, BusCallback, loop);
    gst_object_unref(bus);

    // Set up a custom seek event handler
    g_signal_connect(timeshift_buffer, "element-added", G_CALLBACK(SeekEventCallback), NULL);

    // Set the pipeline to the "playing" state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Start the GMainLoop
    g_print("Press Ctrl+C to exit...\n");
    g_main_loop_run(loop);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(tee, "sink");
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

