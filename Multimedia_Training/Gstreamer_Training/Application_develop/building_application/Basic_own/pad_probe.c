#include <gst/gst.h>

static gboolean pad_probe_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    // Check if this is the pad you're interested in
    const gchar *pad_name = gst_pad_get_name(pad);
    if (g_str_has_prefix(pad_name, "src_%u")) {
        g_print("Found the pad we're looking for: %s\n", pad_name);
        // Link the source pad to your desired sink pad
        // gst_pad_link(pad, your_sink_pad);
    }
    return GST_PAD_PROBE_REMOVE; // Remove the probe after handling it once
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create the GStreamer pipeline and elements
    GstElement *pipeline, *source, *demuxer;

    pipeline = gst_pipeline_new("my-pipeline");
    source = gst_element_factory_make("filesrc", "source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");

    if (!pipeline || !source || !demuxer) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, NULL);

    g_object_set(source, "location", "/home/pranav/Documents/gstreamer_basic/kgf2_telugu.mp4", NULL);

    // Link source to demuxer
    if (!gst_element_link(source, demuxer)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to "playing" state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Add a pad probe to the demuxer's source pad
    GstPad *demuxer_src_pad = gst_element_get_static_pad(demuxer, "src_%u");
    gst_pad_add_probe(demuxer_src_pad, GST_PAD_PROBE_TYPE_IDLE, (GstPadProbeCallback)pad_probe_cb, NULL, NULL);

    // ... (rest of the code)

    return 0;
}

