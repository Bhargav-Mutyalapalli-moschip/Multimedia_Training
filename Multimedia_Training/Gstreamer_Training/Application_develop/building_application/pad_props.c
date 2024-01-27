#include <gst/gst.h>

static void read_video_props(GstCaps *caps);

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Sample caps to demonstrate the function
    GstCaps *sample_caps = gst_caps_from_string("video/x-raw, width=1920, height=1080");

    // Call the function to read video properties
    read_video_props(sample_caps);

    // Free the sample caps
    gst_caps_unref(sample_caps);

    // Clean up GStreamer
    gst_deinit();

    return 0;
}

static void read_video_props(GstCaps *caps) {
    gint width, height;
    const GstStructure *str;

    g_return_if_fail(gst_caps_is_fixed(caps));

    str = gst_caps_get_structure(caps, 0);
    if (!gst_structure_get_int(str, "width", &width) ||
        !gst_structure_get_int(str, "height", &height)) {
        g_print("No width/height available\n");
        return;
    }

    g_print("The video size of this set of capabilities is %dx%d\n",
            width, height);
}

