#include <gst/gst.h>

static gboolean handoff_callback(GstElement *element, GstBuffer *buffer, gpointer user_data) 
{
	// This callback function will be called when the identity element hands off a buffer.
	// You can inspect or process the buffer here.

	gboolean var;
	gint x, y;
	GstMapInfo map;
	guint16 *ptr, t;
	g_print("invoked\n");

	// Get the buffer size:
	guint buffer_size = gst_buffer_get_size(buffer);

	var=gst_buffer_is_writable (buffer);
	if(var == TRUE)
	{
		g_message("YES it is writable\n");
	}
	else
		g_message("NO it is not writable\n");
	buffer=gst_buffer_make_writable (buffer);



	/* Mapping a buffer can fail (non-writable) */
	if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
		ptr = (guint16 *) map.data;
		/* invert data */
		for (y = 0; y < 288; y++) {
			for (x = 0; x < 384 / 2; x++) {
				t = ptr[384 - 1 - x];
				ptr[384 - 1 - x] = ptr[x];
				ptr[x] = t;
			}
			ptr += 384;
		}
		gst_buffer_unmap (buffer, &map);
	}

	// Print the buffer size to the console.
	g_print("Buffer size: %u\n", buffer_size);

	return TRUE; // Returning TRUE means the buffer will continue down the pipeline.
}

int main(int argc, char *argv[]) {
	// Initialize GStreamer.
	gst_init(&argc, &argv);

	// Create the pipeline and elements.
	GstElement *pipeline, *src, *identity, *sink,*vconvert,*vscale,*filter;
	GstCaps *filtercaps;
	pipeline = gst_pipeline_new("pipeline");
	src = gst_element_factory_make("videotestsrc", "source");
	identity = gst_element_factory_make("identity", "identity");
	vconvert = gst_element_factory_make("videoconvert", "vconvert");
	vscale = gst_element_factory_make("videoscale", "vscale");
	filter = gst_element_factory_make("capsfilter", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");
    
    if (!pipeline || !src || !identity || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }
    if (!vscale || !vconvert || !filter) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }
    
    
    // Set the silent property of the identity element to FALSE to enable debugging output.
    g_object_set(G_OBJECT(identity), "silent", FALSE, NULL);
    g_object_set(G_OBJECT(identity), "dump", TRUE, NULL);
    
    // Add elements to the pipeline.
    gst_bin_add(GST_BIN(pipeline), src);
    gst_bin_add(GST_BIN(pipeline), identity);
    gst_bin_add(GST_BIN(pipeline), vconvert);
    gst_bin_add(GST_BIN(pipeline), vscale);
    gst_bin_add(GST_BIN(pipeline), filter);
    gst_bin_add(GST_BIN(pipeline), sink);
    
    // Link elements.
    if (!gst_element_link(src, identity) || !gst_element_link_many(identity, vconvert,vscale,filter,sink,NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    filtercaps = gst_caps_new_simple ("video/x-raw",
		  "format", G_TYPE_STRING, "RGB16",
		    "width", G_TYPE_INT, 384,
		    "height", G_TYPE_INT, 288,
		    "framerate", GST_TYPE_FRACTION, 25, 1,
		    NULL);
    g_object_set (G_OBJECT (filter), "caps", filtercaps, NULL);
    gst_caps_unref (filtercaps);
    gst_debug_bin_to_dot_file_with_ts(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");
    g_print("succesfully caps updated\n");


    // Connect to the "handoff" signal of the identity element to inspect buffers.
    g_signal_connect(identity, "handoff", G_CALLBACK(handoff_callback), NULL);

    // Set the pipeline to playing state.
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop.
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg;
    do {
	    msg = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
	    if (msg != NULL) {
		    gst_message_unref(msg);
	    }
    } while (msg != NULL);
    
    // Clean up.
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);
    
    return 0;
}

