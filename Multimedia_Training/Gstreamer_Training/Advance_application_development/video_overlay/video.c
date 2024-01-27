#include<gst/gst.h>
static void on_window_destroy(GtkWidget *widget, gpointer data);
static void on_sync_message(GstBus *bus, GstMessage *message, gpointer data);

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    // Create a GTK window
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 640, 480);
    g_signal_connect(main_window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Create a GStreamer pipeline
    GstElement *pipeline = gst_pipeline_new("video-pipeline");
    GstElement *filesrc = gst_element_factory_make("filesrc", "file-source");
    GstElement *decodebin = gst_element_factory_make("decodebin", "decoder");
    videosink = gst_element_factory_make("xvimagesink", "video-sink");

    if (!pipeline || !filesrc || !decodebin || !videosink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set the input video file
    g_object_set(G_OBJECT(filesrc), "location", "/home/bhargav/Downloads/KGF Chapter 2 Trailer _ Telugu _Yash_Sanjay Dutt_Raveena_Srinidhi_Prashanth Neel_Vijay Kiragandur.mp4", NULL);

    // Add elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), filesrc);
    gst_bin_add(GST_BIN(pipeline), decodebin);
    gst_bin_add(GST_BIN(pipeline), videosink);

    // Link elements
    if (!gst_element_link(filesrc, decodebin)) {
        g_print("Elements could not be linked. Exiting.\n");
        return -1;
    }

    // Create a GTK drawing area for video display
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(main_window), drawing_area);
    gtk_widget_show_all(main_window);

    // Get the GstVideoOverlay interface from the videosink
    GstVideoOverlay *overlay = GST_VIDEO_OVERLAY(videosink);

    if (!overlay) {
        g_print("Video sink does not support GstVideoOverlay interface. Exiting.\n");
        return -1;
    }

    // Set the window handle for video overlay
    GdkWindow *window = gtk_widget_get_window(drawing_area);
    gst_video_overlay_set_window_handle(overlay, (guintptr)GDK_WINDOW_XID(window));

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Add a message handler to sync messages from the pipeline
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(on_sync_message), overlay);

    // Start the GTK main loop
    gtk_main();

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
// Callback function to handle pad-added signals
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	// Get the pad's name
	gchar *padname = gst_pad_get_name(pad);
	g_print("the pad name :%s\n",padname);

	// Check the pad's direction (source or sink) and link it accordingly
	if (g_str_has_prefix(padname, "src")) {
		GstPad *sinkpad = gst_element_get_static_pad(videobalance, "sink");
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
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    // This function is called when the GTK window is destroyed
    // You can add your cleanup code here if needed
    gtk_main_quit(); // Terminate the GTK main loop
}

static void on_sync_message(GstBus *bus, GstMessage *message, gpointer data) {
    // This function is called to synchronize messages from GStreamer pipeline
    GstVideoOverlay *overlay = GST_VIDEO_OVERLAY(data);

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ELEMENT: {
            const GstStructure *s = gst_message_get_structure(message);
            const gchar *name = gst_structure_get_name(s);

            if (g_str_has_prefix(name, "prepare-xwindow-id")) {
                // This message indicates that the video sink is ready to receive
                // the window handle for video overlay
                guintptr window_handle;
                gst_structure_get(gst_message_get_structure(message), "xid", G_TYPE_UINTPTR, &window_handle, NULL);

                // Set the window handle for video overlay
                gdk_window_foreign_new_for_display(gdk_display_get_default(), (GdkNativeWindow)window_handle);
                gst_video_overlay_set_window_handle(overlay, window_handle);
            }
            break;
        }
        default:
            break;
    }
}

