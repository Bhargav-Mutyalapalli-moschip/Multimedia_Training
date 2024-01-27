#include<stdio.h>
#include<gst/gst.h>

//Dynamic pads callback
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);

//Bus callback
static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data);

//probe callback signature
static GstPadProbeReturn cb_have_data (GstPad *pad,GstPadProbeInfo *info,gpointer user_data);

GstElement *pipeline;
GstElement *filesrc;
GstElement *decodebin;
GstElement *videoscale;
GstElement *videoconvert;
GstElement *filter;
GstElement *autovideosink;
GstPad *src_pad;
GstPad *sink_pad;
GMainLoop *loop;
GstBus *bus;
GstMessage *msg;
GstStateChangeReturn ret;
GstCaps *filtercaps;

int main(int argc,char *argv[])
{
	//initialize the gstreamer
	gst_init(&argc,&argv);

	//create the pipeline
	pipeline = gst_pipeline_new("Test-Pipeline");
	g_assert(pipeline); //check the pipeline is created or not

	//create the elements 
	filesrc=gst_element_factory_make("filesrc",NULL);
	decodebin=gst_element_factory_make("decodebin",NULL);
	filter=gst_element_factory_make("capsfilter",NULL);
	videoscale=gst_element_factory_make("videoscale",NULL);
	videoconvert=gst_element_factory_make("videoconvert",NULL);
	autovideosink=gst_element_factory_make("autovideosink",NULL);

	//create a main and don't run
	loop=g_main_loop_new(NULL,FALSE);
	g_assert(loop);

	//check all elements is created or not
	g_assert(filesrc);
	g_assert(decodebin);
	g_assert(filter);
	g_assert(videoscale);
	g_assert(videoconvert);
	g_assert(autovideosink);

	//add the elements to bin
	gst_bin_add_many(GST_BIN(pipeline),filesrc,decodebin,filter,videoconvert,autovideosink,videoscale,NULL);

	//link the elements
	if(!gst_element_link(filesrc,decodebin))
	{
		g_message("failed to link the filesrc and decodebin\n");
		return -1;
	}

	//second link
	if(!gst_element_link_many(videoconvert,videoscale,filter,autovideosink,NULL))
	{
		g_message("failed to link the capsfilter,videoconvert,autovideosink\n");
		return -1;
	}

	//set the properties of filesrc
	g_object_set(filesrc,"location","/home/bhargav/Downloads/kgf.mp4",NULL);

	//change the filter caps
	filtercaps = gst_caps_new_simple ("video/x-raw",
			"format", G_TYPE_STRING, "NV12",
			"width", G_TYPE_INT, 360,
			"height", G_TYPE_INT, 240,
			"framerate", GST_TYPE_FRACTION, 24, 1,
			NULL);

	g_object_set (G_OBJECT (filter), "caps", filtercaps, NULL);
	gst_caps_unref (filtercaps);

	src_pad = gst_element_get_static_pad (filesrc, "src");
	gst_pad_add_probe (src_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) cb_have_data, NULL, NULL);
	gst_object_unref (src_pad);

	//create a dynamic pad and link the decodebin and capsfilter element
	g_signal_connect(decodebin,"pad-added",G_CALLBACK(on_pad_added),NULL);


	// Set the pipeline to the playing state
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to set pipeline to playing state. Exiting.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Set up a bus to watch for messages
	bus = gst_element_get_bus(pipeline);
	gst_bus_add_watch(bus, bus_callback, NULL);
	gst_object_unref(bus);

	//Run the main loop
	g_main_loop_run(loop);

	GstClockTime last_buffer_running_time = GST_CLOCK_TIME_NONE;

	// Free resources
	g_main_loop_unref(loop);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);




}
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
    if (g_str_has_prefix(padname, "src")) {
        GstPad *sinkpad = gst_element_get_static_pad(videoconvert, "sink");
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

//bus callback function
static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data)
{

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

//probe call back function
static GstPadProbeReturn cb_have_data (GstPad *pad,GstPadProbeInfo *info,gpointer user_data)
{
  gint x, y;
  GstMapInfo map;
  guint16 *ptr, t;
  GstBuffer *buffer;

  buffer = GST_PAD_PROBE_INFO_BUFFER (info);

  buffer = gst_buffer_make_writable (buffer);

  /* Making a buffer writable can fail (for example if it
   * cannot be copied and is used more than once)
   */
  if (buffer == NULL)
    return GST_PAD_PROBE_OK;

  /* Mapping a buffer can fail (non-writable) */
  if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
    ptr = (guint16 *) map.data;
    /* invert data */
    for (y = 0; y < 720; y++) {
      for (x = 0; x < 1280 / 2; x++) {
        t = ptr[1280 - 1 - x];
        ptr[1280 - 1 - x] = ptr[x];
        ptr[x] = t;
	g_print("the t value:%d\n",t);
      }
      ptr += 1280;
    }
    gst_buffer_unmap (buffer, &map);
  }

  GST_PAD_PROBE_INFO_DATA (info) = buffer;

  return GST_PAD_PROBE_OK;
}


	




