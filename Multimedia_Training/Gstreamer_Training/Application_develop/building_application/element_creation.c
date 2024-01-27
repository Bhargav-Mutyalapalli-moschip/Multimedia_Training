#include <gst/gst.h>

int main (int argc,char *argv[])
{
	GstElement *element;

	/* init GStreamer */
	gst_init (&argc, &argv);

	/* create element */
	element = gst_element_factory_make ("fakesrc", NULL);
	if (!element) {
		g_print ("Failed to create element of type 'fakesrc'\n");
		return -1;
	}
	const gchar *element_name = GST_OBJECT_NAME(element);
	g_print("Element created with name: %s\n", element_name);

	g_printerr("Element is created successfully\n");
	gst_object_unref (GST_OBJECT (element));
	g_printerr("Element is destroyed successfully\n");

	return 0;
}

