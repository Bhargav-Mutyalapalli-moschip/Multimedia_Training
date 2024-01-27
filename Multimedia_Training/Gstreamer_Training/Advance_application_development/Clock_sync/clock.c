#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create elements and add them to the pipeline (e.g., source, decoder, sink)
    GstElement *source = gst_element_factory_make("videotestsrc", "source");
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), sink);

    // Link the elements
    if (!gst_element_link(source, sink)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to the PLAYING state
    GstClockTime base_time;
    GstClock *pipeline_clock = gst_pipeline_get_clock(GST_PIPELINE(pipeline));
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Get the base_time when the pipeline started playing
    base_time = gst_clock_get_time(pipeline_clock);
    g_print("Pipeline is in PLAYING state. Base-time: %" GST_TIME_FORMAT "\n",
            GST_TIME_ARGS(base_time));

    g_print("Pipeline is in PLAYING state. Press Enter to pause...\n");
    getchar();  // Wait for user input

    // Set the pipeline to the PAUSED state
    gst_element_set_state(pipeline, GST_STATE_PAUSED);

    // Calculate running-time
    GstClockTime current_time = gst_clock_get_time(pipeline_clock);
    g_print("Pipeline is in PLAYING state. Base-time: %" GST_TIME_FORMAT "\n",
            GST_TIME_ARGS(current_time));
    GstClockTime running_time = current_time - base_time;

    g_print("Pipeline is in PAUSED state. Running-time: %" GST_TIME_FORMAT "\n",
            GST_TIME_ARGS(running_time));

    // Cleanup and release resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

