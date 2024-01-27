#include <gst/gst.h>

// This callback function will be called when the identity element emits the "handoff" signal.
static void handoff_callback(GstElement *identity, GstBuffer *buffer, gpointer user_data) {
    // Get the size of the buffer
    guint buffer_size = gst_buffer_get_size(buffer);
    g_print("buffer stord value:%u\n",buffer_size);
    
    // Extract a pointer to the data in the buffer
    /*if (gst_buffer_extract(buffer, 0, &data, buffer_size) != GST_FLOW_OK) {
        g_print("Failed to extract buffer data\n");
        return;
	}*/
    gint x, y;
    GstMapInfo map;
    guint16 *ptr=NULL, t;
    buffer=gst_buffer_make_writable (buffer);
   if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
    ptr = (guint16 *) map.data;
    for(x=0;x<240;x++)
    {
	    for(y=0;y<360/2;y++)
	    {
		    t=ptr[x];
		    ptr[x]=ptr[360-1-x];
		    ptr[360-1-x]=t;
	    }
	    ptr += 360;
    }
    gst_buffer_unmap (buffer, &map);

    // Manipulate the data in the buffer (for example, convert all pixel values to grayscale)
    g_print("Received a buffer with size %u\n", buffer_size);

   }
   //function to print caps
   /*static void read_video_props (GstCaps *caps)
     {
     gint width, height;
     const GstStructure *str;

     g_return_if_fail (gst_caps_is_fixed (caps));

     str = gst_caps_get_structure (caps, 0);
     if (!gst_structure_get_int (str, "width", &width) ||
     !gst_structure_get_int (str, "height", &height)) {
     g_print ("No width/height available\n");
     return;
     }

     g_print ("The video size of this set of capabilities is %dx%d\n",
     width, height);
     }*/
}
int main(int argc, char *argv[]) {
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create a pipeline
	GstElement *pipeline = gst_pipeline_new("my-pipeline");

	// Create elements
	GstElement *src = gst_element_factory_make("videotestsrc", "source");
	GstElement *identity = gst_element_factory_make("identity", "my-identity");
	GstElement *sink = gst_element_factory_make("autovideosink", "sink");

	if (!pipeline || !src || !identity || !sink) {
		g_print("One or more elements could not be created. Exiting.\n");
		return -1;
	}

	// Add elements to the pipeline
	gst_bin_add_many(GST_BIN(pipeline), src, identity, sink, NULL);

	// Link elements
	if (!gst_element_link_many(src, identity, sink, NULL)) {
		g_print("Elements could not be linked. Exiting.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Set the "silent" property of the identity element to FALSE
	g_object_set(identity, "silent", FALSE, NULL);

	// Connect the "handoff" signal to the callback function
	g_signal_connect(identity, "handoff", G_CALLBACK(handoff_callback), NULL);

	// Start the pipeline
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Run the main loop
	GstBus *bus = gst_element_get_bus(pipeline);
	GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
	gst_message_unref(msg);

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(bus);
	gst_object_unref(pipeline);

	return 0;
}

