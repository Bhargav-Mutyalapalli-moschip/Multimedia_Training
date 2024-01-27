#include <gst/gst.h>
#include <stdio.h>

int main (int   argc,char *argv[])
{
	GstElementFactory *factory;
	GstElement * element;

	/* init GStreamer */
	gst_init (&argc, &argv);

	/* create element, method #2 */
	factory = gst_element_factory_find ("croptech");
	if (!factory) {
		g_print ("Failed to find factory of type 'croptech'\n");
		return -1;
	}
	element = gst_element_factory_create (factory, "source");
	if (!element) {
		g_print ("Failed to create element, even though its factory exists!\n");
		return -1;
	}
	printf("Created the croptech element success\n");
	gst_object_unref (GST_OBJECT (element));
	gst_object_unref (GST_OBJECT (factory));

	return 0;
}
