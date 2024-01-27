#include <gst/gst.h>

static void print_capabilities(GstElement *element) {
    GstPad *src_pad = gst_element_get_static_pad(element, "src");
    if (!src_pad) {
        g_printerr("Failed to get source pad from the element.\n");
        return;
    }

    GstCaps *caps = gst_pad_get_current_caps(src_pad);
    if (!caps) {
        g_printerr("Failed to get current caps from the pad.\n");
        gst_object_unref(src_pad);
        return;
    }

    g_print("Capabilities of the element:\n");
    gst_caps_foreach(caps, print_structure, NULL);

    gst_caps_unref(caps);
    gst_object_unref(src_pad);
}

static void print_structure(const GstStructure *structure, gpointer user_data) {
    gchar *structure_string = gst_structure_to_string(structure);
    g_print("%s\n", structure_string);
    g_free(structure_string);
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *videotestsrc, *autovideosink;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a pipeline
    pipeline = gst_pipeline_new("test-pipeline");

    // Create the videotestsrc element
    videotestsrc = gst_element_factory_make("videotestsrc", "source");
    autovideosink = gst_element_factory_make("autovideosink", "sink"); // Add autovideosink as the sink element

    if (!pipeline || !videotestsrc || !autovideosink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Add videotestsrc and autovideosink to the pipeline
    gst_bin_add(GST_BIN(pipeline), videotestsrc);
    gst_bin_add(GST_BIN(pipeline), autovideosink);

    // Link the elements
    if (!gst_element_link(videotestsrc, autovideosink)) { // Link the source to the sink
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Print capabilities of the element
    print_capabilities(videotestsrc);

    // Run the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_poll(bus, GST_MESSAGE_ERROR | GST_MESSAGE_EOS, -1);
    //gst_bus_unref(bus);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

