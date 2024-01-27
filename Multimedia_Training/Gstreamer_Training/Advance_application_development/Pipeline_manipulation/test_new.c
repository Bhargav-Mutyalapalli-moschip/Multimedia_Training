#include <gst/gst.h>

static GMainLoop *loop;
static GstElement *appsrc;

static gboolean push_data(gpointer user_data) {
    static guint8 counter = 0;

    // Prepare a buffer with some data
    guint size = 1024;
    GstBuffer *buffer = gst_buffer_new_allocate(NULL, size, NULL);
    GST_BUFFER_PTS(buffer) = GST_CLOCK_TIME_NONE;
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 2);
    //gst_buffer_memset(buffer, 0, counter % 256, size);
    gst_buffer_memset(buffer, 0, counter % 256, size);
    counter++;

    GstFlowReturn ret;
    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);

    if (ret != GST_FLOW_OK) {
        g_print("Pushing data failed: %s\n", gst_flow_get_name(ret));
        g_main_loop_quit(loop);
    }

    return TRUE;
}

static void need_data_handler(GstElement *element, guint unused_size, gpointer user_data) {
    g_print("Need data requested. Pushing more data...\n");

    // Schedule the push_data function to be called
    g_idle_add(push_data, NULL);
}

static void enough_data_handler(GstElement *element, gpointer user_data) {
    g_print("Enough data requested. Waiting for more need-data signal...\n");
    sleep(5);
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);

    // Create the pipeline and appsrc element
    GstElement *pipeline, *conv, *videosink;
    pipeline = gst_pipeline_new("pipeline");
    appsrc = gst_element_factory_make("appsrc", "source");
    conv = gst_element_factory_make("videoconvert", "conv");
    videosink = gst_element_factory_make("autovideosink", "videosink");

    if (!pipeline || !appsrc || !conv || !videosink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Configure appsrc
    g_object_set(G_OBJECT(appsrc),
        "caps", gst_caps_new_simple("video/x-raw",
                                    "format", G_TYPE_STRING, "RGB",
                                    "width", G_TYPE_INT, 640,
                                    "height", G_TYPE_INT, 480,
                                    "framerate", GST_TYPE_FRACTION, 30, 1,
                                    NULL),
        "stream-type", 0,
        "format", GST_FORMAT_TIME,
        NULL);

    // Add elements to the pipeline and link them
    gst_bin_add_many(GST_BIN(pipeline), appsrc, conv, videosink, NULL);
    gst_element_link_many(appsrc, conv, videosink, NULL);

    // Connect the "need-data" signal to the need_data_handler function
    g_signal_connect(appsrc, "need-data", G_CALLBACK(need_data_handler), NULL);

    // Connect the "enough-data" signal to the enough_data_handler function
    g_signal_connect(appsrc, "enough-data", G_CALLBACK(enough_data_handler), NULL);

    // Set the pipeline to the PLAYING state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Start the main loop
    g_main_loop_run(loop);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

