#include <gst/gst.h>

int main(int argc, char* argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    GstElement* pipeline = gst_parse_launch("videotestsrc ! autovideosink", NULL);

    // Retrieve elements implementing a custom interface (replace "custom-interface" with your interface type)
    GType custom_iface_type = g_type_from_name("custom-interface");
    GstElement* element = gst_bin_get_by_interface(GST_BIN(pipeline), custom_iface_type);

    if (element != NULL) {
        // Element found, you can now manipulate it
        g_print("Found an element implementing the custom interface.\n");
        g_object_unref(element);
    } else {
        g_print("No element implementing the custom interface found in the pipeline.\n");
    }

    // Cleanup and exit
    gst_object_unref(pipeline);
    return 0;
}

