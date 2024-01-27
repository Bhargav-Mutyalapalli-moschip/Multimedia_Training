#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

// Callback function for when the GTK window is destroyed
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
GstElement *videosink;
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
    videosink = gst_element_factory_make("autovideosink", "video-sink");

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

    // Connect pad-added signals
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), NULL);


    // Get the GstVideoOverlay interface from the videosink
    GstVideoOverlay *overlay = GST_VIDEO_OVERLAY(videosink);

    if (!overlay) {
        g_print("Video sink does not support GstVideoOverlay interface. Exiting.\n");
        return -1;
    }

    // Get the GDK window handle of the GTK window
    GdkWindow *window = gtk_widget_get_window(main_window);

    // Set the overlay window handle
    gst_video_overlay_set_window_handle(overlay, (guintptr)GDK_WINDOW_XID(window));

    // Set the pipeline state to playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Show the GTK window
    gtk_widget_show_all(main_window);

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

