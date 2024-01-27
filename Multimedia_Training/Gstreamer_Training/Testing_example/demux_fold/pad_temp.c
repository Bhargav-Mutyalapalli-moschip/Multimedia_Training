#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create an element (in this example, we'll use a source element)
    GstElement *element = gst_element_factory_make("qtdemux", "demux");

    if (!element) {
        g_printerr("Failed to create element.\n");
        return -1;
    }

    // Get the pad template from the element (e.g., the source pad template)
    GstPadTemplate *pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(element), "audio_%u");

    if (!pad_template) {
        g_printerr("Failed to get pad template.\n");
        gst_object_unref(element);
        return -1;
    }

    // Create a new pad based on the pad template
    GstPad *new_pad = gst_pad_new_from_template(pad_template, "new_pad");

    if (!new_pad) {
        g_printerr("Failed to create new pad.\n");
        gst_object_unref(element);
        return -1;
    }
    gchar *padname = gst_pad_get_name(new_pad);
    g_print("the pad name :%s\n",padname);

    // Add the new pad to the element
     if (!gst_pad_set_active(new_pad, TRUE) ||
        !gst_element_add_pad(element, new_pad)) {
        g_printerr("Failed to add new pad to element.\n");
        gst_object_unref(new_pad);
        gst_object_unref(element);
        return -1;
    }

    g_print("New pad created successfully.\n");

    // Clean up
    //gst_object_unref(new_pad);
    gst_object_unref(element);

    return 0;
}

