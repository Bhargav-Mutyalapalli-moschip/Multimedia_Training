#include<stdio.h>
#include <gst/gst.h>

GstElement *pipeline, *source, *sink,*decodebin;
static guint64 start_time = 0;

static void pad_on(GstElement* element ,GstPad *pad , gpointer data)
{
	GstPad *sinkpad = gst_element_get_static_pad(sink,"sink");
	if(!(gst_pad_link(pad, sinkpad)))
	{
		g_print("failed to link the pads\n");
		return;
	}
}
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
// This callback function will be called when the probe is triggered.
static GstPadProbeReturn pad_probe_callback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	g_print("probe call back function was called\n");

	GstMapInfo map;
	guint16 *ptr, t;
	GstBuffer *buffer;

	//tClockTime start_time = 0;	
	GstClockTime current_time = gst_util_get_timestamp();	
	buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	if (buffer == NULL)
	{
		g_print("data present in the buffer is null\n");
		return GST_PAD_PROBE_OK;
	}
	buffer = gst_buffer_make_writable (buffer);
	g_print("hello\n");

	GstClockTime timestamp = GST_BUFFER_TIMESTAMP(buffer);
	g_print("Buffer Timestamp: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(timestamp));

	if (start_time == 0) {
		start_time = current_time;
	}

	// Calculate and print the latency
	g_print("current time %" GST_TIME_FORMAT"\n",GST_TIME_ARGS(current_time));
	guint64 latency = current_time - start_time;
	g_print("Latency: %" GST_TIME_FORMAT " microseconds\n", GST_TIME_ARGS(latency));

	guint buffer_size_bytes = gst_buffer_get_size(buffer);
	g_print("Buffer size: %u bytes\n", buffer_size_bytes);

	return GST_PAD_PROBE_OK;
}

int main(int argc, char *argv[]) {
	GMainLoop *loop;
	GstPad *pad;
	guint duration_seconds = 5;
	// Initialize GStreamer
	gst_init(&argc, &argv);
	loop = g_main_loop_new(NULL, FALSE);

	// Create a simple pipeline: videotestsrc -> fakesink
	pipeline = gst_pipeline_new("latency-analysis-pipeline");
	source = gst_element_factory_make("filesrc", "source");
	decodebin = gst_element_factory_make("decodebin","decodebin");
	sink = gst_element_factory_make("autovideosink", "sink");

	if (!pipeline || !source || !sink || !decodebin) {
		g_printerr("Pipeline elements could not be created.\n");
		return -1;
	}

	g_object_set(source ,"location","/home/softnautics/kishore/latency/kgf2_telugu.mp4",NULL);

	// Add elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), source,sink,decodebin ,NULL);

	// Link the elements
	if (!gst_element_link(source, decodebin)) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	g_signal_connect(decodebin , "pad-added",G_CALLBACK(pad_on),NULL);

	GstBus *bus = gst_element_get_bus(pipeline);
	gst_bus_add_watch(bus, bus_callback, loop);
	gst_object_unref(bus);
	pad = gst_element_get_static_pad(source, "src");

	// Add a probe to the source pad
  gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
      (GstPadProbeCallback)pad_probe_callback , NULL, NULL);
  gst_object_unref (pad);
	// Start the pipeline
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	//g_timeout_add_seconds(duration_seconds, (GSourceFunc)g_main_loop_quit, loop);
	// Start the GMainLoop to handle events
	g_main_loop_run(loop);

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	g_main_loop_unref(loop);

	return 0;
}

