#include <gst/gst.h>

static void cb_new_pad(GstElement *element, GstPad *pad, gpointer data) {
    gchar *name;
    g_print("Came to pad addes signal function\n");
    name = gst_pad_get_name(pad);
    g_print("A new pad %s was created\n", name);
    g_free(name);
    /* Here, you would set up a new pad link for the newly created pad */
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demux;
    GMainLoop *loop;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create elements */
    pipeline = gst_pipeline_new("my_pipeline");
    source = gst_element_factory_make("filesrc", "source");
    demux = gst_element_factory_make("oggdemux", "demuxer");

    /* Check if elements were created properly */
    if (!pipeline || !source || !demux) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    /* Set the file location for the source element */
    g_object_set(source, "location", "/home/bhargav/Documents/gstreamer_sample/gst_tools/english.ogg", NULL);

    /* Put together the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, demux, NULL);
    gst_element_link_pads(source, "src", demux, "sink");

    /* Listen for newly created pads */
    g_signal_connect(demux, "pad-added", G_CALLBACK(cb_new_pad), NULL);

    /* Start the pipeline */
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    /* Clean up */
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);

    return 0;
}

