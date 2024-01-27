#include <gst/gst.h>
#include <stdio.h>

int main (int   argc,char *argv[])
{
	GstElement *bin, *pipeline, *source, *sink;

	/* init */
	gst_init (&argc, &argv);

	/* create */
	pipeline = gst_pipeline_new ("my_pipeline");
	bin = gst_bin_new ("my_bin");
	source = gst_element_factory_make ("fakesrc", "source");
	sink = gst_element_factory_make ("fakesink", "sink");

	/* First add the elements to the bin */
	gst_bin_add_many (GST_BIN (bin), source, sink, NULL);
	/* add the bin to the pipeline */
	gst_bin_add (GST_BIN (pipeline), bin);
	gst_element_link (source, sink);
	GstElement *check=gst_bin_get_by_name (GST_BIN(pipeline),"sink");
	if(check == NULL)
	{
		printf("Element is not there in this bin\n");
	}
	else
		printf("Element is there in this bin\n");
	

	/* link the elements */


}

