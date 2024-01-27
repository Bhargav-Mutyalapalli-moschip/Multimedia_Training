#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create a sample video caps description (replace this with your actual caps).
    const gchar *caps_description = "video/x-raw,format=I420,width=1920,height=1080";

    // Parse the caps description.
    GstCaps *caps = gst_caps_from_string(caps_description);

    if (caps) {
        // Iterate through the structures in the caps.
        for (guint i = 0; i < gst_caps_get_size(caps); i++) {
            GstStructure *structure = gst_caps_get_structure(caps, i);

            // Check if the format is "I420."
            const gchar *format = gst_structure_get_string(structure, "format");
            if (g_strcmp0(format, "I420") == 0) {
                gint chroma_subsampling_horiz = 0;
                gint chroma_subsampling_vert = 0;

                // Manually extract the chroma subsampling values.
                const gchar *chroma_site = gst_structure_get_string(structure, "chroma-site");
                if (sscanf(chroma_site, "%d:%d", &chroma_subsampling_horiz, &chroma_subsampling_vert) == 2) {
                    g_print("Chroma Subsampling: %d:%d\n", chroma_subsampling_horiz, chroma_subsampling_vert);
                } else {
                    g_print("Chroma Subsampling: Unknown\n");
                }
            }
        }
    }

    gst_object_unref(caps);

    return 0;
}

