#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create an element
    GstElement *element = gst_element_factory_make("qtdemux", NULL);

    if (!element) {
        g_printerr("Element could not be created.\n");
        return -1;
    }

    // Get the element's pad templates
    GstStaticPadTemplate *pad_template;
    GList *pad_template_list = NULL;
    GstElementFactory *factory = gst_element_get_factory(element);

    if (!factory) {
        g_printerr("Element factory not found.\n");
        gst_object_unref(element);
        return -1;
    }

    pad_template_list = gst_element_factory_get_static_pad_templates(factory);

    if (!pad_template_list) {
        g_printerr("No pad templates found for the element.\n");
        gst_object_unref(element);
        return -1;
    }

    // Iterate through the pad templates
    for (GList *iter = pad_template_list; iter != NULL; iter = g_list_next(iter)) {
        pad_template = GST_STATIC_PAD_TEMPLATE(g_list_nth_data(iter, 0));

        if (pad_template) {
            // Check if the "Sometimes" flag is set
            if (pad_template->flags & GST_PAD_SOMETIMES) {
                g_print("Pad template name: %s\n", pad_template->name_template);
            }
        }
    }

    // Clean up
    g_list_free(pad_template_list);
    gst_object_unref(element);

    return 0;
}

