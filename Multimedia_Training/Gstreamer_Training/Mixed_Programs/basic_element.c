#include <gst/gst.h>
#include <stdio.h>

int main (int   argc,char *argv[])
{
	GstElement *element;
	/* init GStreamer */
	gst_init (&argc, &argv);
	/* create element */
	element = gst_element_factory_make ("fakesrc", "source");
	if (!element) 
	{
		g_print ("Failed to create element of type 'fakesrc'\n");
		return -1;
	}
	printf("Element is created successfully\n");
	gst_object_unref (GST_OBJECT (element));
	return 0;
}
