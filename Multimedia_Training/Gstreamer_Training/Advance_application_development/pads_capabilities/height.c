#include <gst/gst.h>

static void on_caps_changed(GstElement *element, GstPad *pad, gpointer user_data) {
    GstCaps *caps;
    GstStructure *structure;
    gint width, height;

    // Query the current apabilities of the pad
    caps = gst_pad_get_current_caps(pad);
    if (!caps) {
        g_printerr("Failed to get current caps.\n");
        return;
    }

    // Get the first structure from the capabilities
    structure = gst_caps_get_structure(caps, 0);

    // Retrieve width and height
    if (gst_structure_get_int(structure, "width", &width) &&
        gst_structure_get_int(structure, "height", &height)) {
        g_print("Width: %d, Height: %d\n", width, height);
    } else {
        g_printerr("Failed to get width and height.\n");
    }

    // Release the caps
    gst_caps_unref(caps);
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *videotestsrc,*sink;
    GstPad *pad;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a pipeline
    pipeline = gst_pipeline_new("test-pipeline");

    // Create the videotestsrc element
    videotestsrc = gst_element_factory_make("videotestsrc", "source");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !videotestsrc) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Add videotestsrc to the pipeline
    gst_bin_add(GST_BIN(pipeline), videotestsrc);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Link the elements
    if (!gst_element_link(videotestsrc, sink)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Get the source pad of videotestsrc
    pad = gst_element_get_static_pad(videotestsrc, "src");

    // Set up a callback for caps change events
    g_signal_connect(pad, "notify::caps", G_CALLBACK(on_caps_changed), NULL);

    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    GMainLoop *loop=g_main_loop_new(NULL,FALSE);
    g_main_loop_run(loop);
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_poll(bus, GST_MESSAGE_ERROR | GST_MESSAGE_EOS, -1);

    // Clean up
    gst_object_unref(pad);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

