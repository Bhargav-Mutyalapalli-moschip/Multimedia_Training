#include<stdio.h>
#include<gst/gst.h>
GMainLoop *loop;
static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data)
{
	GMainLoop *loop = (GMainLoop *)data;

	switch (GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_ERROR:
			g_print("Received error message:\n");
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("Error: %s\n", err->message);
			g_main_loop_quit(loop);
			break;
		case GST_MESSAGE_EOS:
			g_print("End of stream reached.\n");
			g_main_loop_quit(loop);
			break;
		default:
			break;
	}
}
void main( int args , char*argv[])
{
	GstElement *pipeline ,*source,*sink;
	GstBus *bus;
	gst_init(&args,&argv);
	
	loop = g_main_loop_new(NULL,FALSE);

	source = gst_element_factory_make("videotestsrc","source");
	
	pipeline = gst_pipeline_new("mypipeline");

	bus = gst_element_get_bus(pipeline);
	gst_bus_add_watch(bus, bus_callback, NULL);
	gst_object_unref(bus);
	
	sink = gst_element_factory_make("autovideosink","sink");
	
	if(!pipeline ||!source || !sink)
		g_print("failed to create elements\n");

	gst_bin_add_many(GST_BIN(pipeline),source,sink,NULL);

	gst_element_link(source,sink);

	gst_element_set_state(pipeline , GST_STATE_PLAYING);


	g_main_loop_run(loop);
	
	g_object_unref(pipeline);
	g_main_loop_unref(loop);
}
