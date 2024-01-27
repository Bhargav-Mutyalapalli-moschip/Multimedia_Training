#include <gst/gst.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstElement *pipeline, *decodebin, *your_sink_element;
    GMainLoop *loop;

    // Initialize your pipeline, decodebin, and sink element here...

    // Create the pipeline
    pipeline = gst_pipeline_new("dynamic-pipeline");
    decodebin = gst_element_factory_make("decodebin", "decoder");

    // Check if elements were created successfully
    if (!pipeline || !decodebin) {
        g_printerr("Failed to create GStreamer elements.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), decodebin, NULL);

    // Link decodebin to your sink element (modify this based on your pipeline)

    // Set decodebin to PAUSED state to probe its source pads
    GstStateChangeReturn ret = gst_element_set_state(decodebin, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set decodebin to PAUSED state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Create a GMainLoop to run the pipeline
    loop = g_main_loop_new(NULL, FALSE);

    // Variable to track whether the desired pad has been found
    gboolean found_desired_pad = FALSE;

    // Enter a loop to look for the desired pad
    while (!found_desired_pad) {
        GstPad *new_pad = gst_element_get_static_pad(your_sink_element, "sink");
        if (new_pad) {
            g_print("Found the pad we're looking for.\n");
            // Link the source pad to the sink pad
            if (gst_pad_link(gst_element_get_static_pad(decodebin, "src"), new_pad) == GST_PAD_LINK_OK) {
                g_print("Linked decodebin source pad to sink pad.\n");
                found_desired_pad = TRUE;
            } else {
                g_printerr("Failed to link source pad to sink pad.\n");
            }
            gst_object_unref(new_pad);
        }
        g_usleep(100000); // Sleep for 100 milliseconds
    }

    // Set the pipeline to PLAYING state to start processing data
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to PLAYING state.\n");
        gst_object_unref(pipeline);
        g_main_loop_unref(loop);
        return -1;
    }

    // Run the GStreamer main loop
    g_main_loop_run(loop);

    // Cleanup and release resources...
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}

