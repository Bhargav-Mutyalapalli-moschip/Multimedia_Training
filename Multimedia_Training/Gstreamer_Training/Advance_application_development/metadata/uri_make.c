#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    if (argc < 2) {
        g_print("Usage: %s <URI>\n", argv[0]);
        return -1;
    }

    const gchar *uri = argv[1];
    GstElement *filesrc;
    filesrc=gst_element_factory_make("filesrc","file-src");
    GstElement *element = gst_element_make_from_uri(GST_URI_SRC,uri,NULL);

    if (element != NULL) {
        g_print("Element created successfully from URI: %s\n", uri);

        // Clean up
        gst_object_unref(element);
    } else {
        g_print("Failed to create element from URI.\n");
    }

    return 0;
}

