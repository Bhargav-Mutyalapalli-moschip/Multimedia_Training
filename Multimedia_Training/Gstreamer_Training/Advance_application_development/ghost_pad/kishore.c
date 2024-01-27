#include <gst/gst.h>

 

/*static void pad_add(GstElement *element ,GstPad *pad , gpointer data)
{
	g_print("call back function is called\n");
	GstPad *sinkpad;
	sinkpad = gst_element_get_static_pad((GstElement*)data,"sink");

 

	GstPadLinkReturn ret;
        ret = gst_pad_link(pad, sinkpad);

 

        if (GST_PAD_LINK_FAILED(ret)) {
            g_print("Failed to link pads!\n");
        } else {
            g_print("Pad linked successfully!\n");
        }

	gst_object_unref(sinkpad);
}*/
GstElement *vconvert,*videosink;
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(videosink, "sink");
		if(!sinkpad)
			g_print("unsuccesfull retrival of pad info\n");
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			g_printerr("Video pad link failed.\n");
		} else {
			g_print("Linked video pad.\n");
		}
	}

	g_free(padname);
}
int main(int argc, char *argv[]) {
	gst_init(&argc, &argv);



	GstElement *pipeline = gst_pipeline_new("mp4-player");



	GstElement *filesrc = gst_element_factory_make("filesrc", "source");
	GstElement *decodebin = gst_element_factory_make("decodebin", "decoder");



	videosink = gst_element_factory_make("autovideosink", "videosink");



	GMainLoop *loop = g_main_loop_new(NULL, FALSE);



	if (!pipeline || !filesrc || !decodebin || !videosink) {
		g_print("One or more elements could not be created. Exiting.\n");
		return -1;
	}
	g_object_set(filesrc, "location", "/home/bhargav/Downloads/all/cool.mp4", NULL);



	gst_bin_add(GST_BIN(pipeline), filesrc);
	gst_bin_add(GST_BIN(pipeline), decodebin);
	gst_bin_add(GST_BIN(pipeline), videosink);

	gst_element_link(filesrc, decodebin);

	g_signal_connect(decodebin ,"pad-added",G_CALLBACK(on_pad_added),NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_main_loop_run(loop);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	g_main_loop_unref(loop);



	return 0;
}
