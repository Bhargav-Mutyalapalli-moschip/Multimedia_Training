#include <gst/gst.h>

int main(int argc, char* argv[]) {
    GstElement *pipeline, *source, *scale, *sink;
    GstBus *bus;
    GstMessage *msg;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the elements
    pipeline = gst_pipeline_new("video-scaler-pipeline");
    source = gst_element_factory_make("videotestsrc", "source");
    scale = gst_element_factory_make("videoscale", "scaler");
    sink = gst_element_factory_make("autovideosink", "sink");

    // Create a scaler element using gst_video_scaler_new
    GstVideoScaler *scaler = gst_video_scaler_new(GST_VIDEO_RESAMPLER_METHOD_NEAREST, GST_VIDEO_SCALER_FLAG_NONE 2,1920,1080,NULL);

    // Check if scaler creation was successful
    if (!scaler) {
        g_error("Failed to create video scaler");
        return -1;
    }

    // Set the scaler on the videoscale element
    g_object_set(G_OBJECT(scale), "scaler", scaler, NULL);

    if (!pipeline || !source || !scale || !sink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Set up the pipeline
    gst_bin_add(GST_BIN(pipeline), source);
    gst_bin_add(GST_BIN(pipeline), scale);
    gst_bin_add(GST_BIN(pipeline), sink);

    if (!gst_element_link(source, scale)) {
        g_printerr("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (!gst_element_link(scale, sink)) {
        g_printerr("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the pipeline to the "playing" state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait until error or EOS
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    // Release resources
    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

