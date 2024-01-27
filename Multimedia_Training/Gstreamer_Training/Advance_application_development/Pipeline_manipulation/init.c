#include<gst/gst.h>
int main(int argc,char *argv[])
{
	gst_init(&argc,&argv);
	GstElement *pipeline=gst_pipeline_new("test");
	GST_DEBUG_OBJECT(pipeline, "done softnautics");
	g_print("init invoked\n");
	return 0;
}
