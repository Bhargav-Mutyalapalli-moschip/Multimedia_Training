#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstRegistry *registry;
    GList *elementFactories, *elemFactoryIter;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Get the GStreamer registry */
    registry = gst_registry_get();

    /* Get a list of element factories from the registry */
    elementFactories = gst_registry_get_feature_list(registry, GST_TYPE_ELEMENT_FACTORY);

    /* Iterate through the list of element factories */
    for (elemFactoryIter = elementFactories; elemFactoryIter != NULL; elemFactoryIter = elemFactoryIter->next) {
        GstElementFactory *elementFactory = GST_ELEMENT_FACTORY(elemFactoryIter->data);
        const gchar *elementName = gst_element_factory_get_longname(elementFactory);
        g_print("Element Factory: %s\n", elementName);
    }

    /* Free the list and unreference the registry */
    gst_plugin_feature_list_free(elementFactories);
    gst_object_unref(registry);

    return 0;
}

