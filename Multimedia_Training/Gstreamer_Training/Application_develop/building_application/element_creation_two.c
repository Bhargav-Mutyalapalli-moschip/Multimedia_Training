#include <gst/gst.h>

int main (int argc,char *argv[])
{
	GstElementFactory *factory;
	GstElement * element;

	/* init GStreamer */
	gst_init (&argc, &argv);

	/* create element, method #2 */
	factory = gst_element_factory_find ("fakesrc");
	if (!factory) {
		g_print ("Failed to find factory of type 'fakesrc'\n");
		return -1;
	}
	g_print ("successfully  taken permission from element factory!\n");
	//element = gst_element_factory_create (factory, NULL);  //If you pass NULL the unique name was defined to it
	element = gst_element_factory_create (factory, "source");
	if (!element) {
		g_print ("Failed to create element, even though its factory exists!\n");
		return -1;
	}
	g_print ("successfully  create elements, even though its factory exists!\n");
	 const gchar *element_name = GST_OBJECT_NAME(element);
        g_print("Element created with name: %s\n", element_name);

	gst_object_unref (GST_OBJECT (element));
	gst_object_unref (GST_OBJECT (factory));

	return 0;
}
