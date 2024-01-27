#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create elements
    GstElement *pipeline = gst_pipeline_new("pipeline");
    GstElement *src = gst_element_factory_make("videotestsrc", "src");
    GstElement *tee = gst_element_factory_make("tee", "tee");
    GstElement *sink1 = gst_element_factory_make("autovideosink", "sink1");
    GstElement *sink2 = gst_element_factory_make("autovideosink", "sink2");

    // Check elements
    if (!pipeline || !src || !tee || !sink1 || !sink2) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), src, tee, sink1, sink2, NULL);

    // Link the elements
    if (!gst_element_link(src, tee)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get tee's source pads
    GstPad *tee_src_pad1 = gst_element_get_request_pad(tee, "src_%u");
    GstPad *tee_src_pad2 = gst_element_get_request_pad(tee, "src_%u");

    // Link tee's source pads to sinks
    if (gst_pad_link(tee_src_pad1, sink1) != GST_PAD_LINK_OK ||
        gst_pad_link(tee_src_pad2, sink2) != GST_PAD_LINK_OK) {
        g_printerr("Tee could not be linked to sinks.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                                 GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    // ...

    // Cleanup and return
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

