#include <gst/gst.h>
#include <glib.h>
#include <gst/controller/gstinterpolationcontrolsource.h>
#include <gst/controller/gstdirectcontrolbinding.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline
    GstElement *pipeline, *audioSrc, *audioSink;
    GstControlSource *csource;
    GstControlBinding *binding;
    GMainLoop *loop;

    pipeline = gst_pipeline_new("volume-interpolation-pipeline");
    audioSrc = gst_element_factory_make("audiotestsrc", "audio-source");
    audioSink = gst_element_factory_make("autoaudiosink", "audio-sink");

    if (!pipeline || !audioSrc || !audioSink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add(GST_BIN(pipeline), audioSrc);
    gst_bin_add(GST_BIN(pipeline), audioSink);

    // Link the elements
    if (!gst_element_link(audioSrc, audioSink)) {
        g_printerr("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Create a linear interpolation control source
    csource = gst_interpolation_control_source_new();
    g_object_set(csource, "mode", GST_INTERPOLATION_MODE_LINEAR, NULL);

    // Create a direct control binding to control the volume property
    binding = gst_direct_control_binding_new(GST_OBJECT(audioSrc),"volume",csource);

    GstTimedValueControlSource *tv_csource = (GstTimedValueControlSource *)csource;
    gst_timed_value_control_source_set (tv_csource, 0 * GST_SECOND, 0.0);
    gst_timed_value_control_source_set (tv_csource, 1 * GST_SECOND, 1.0);

    /*gst_control_binding_set_property_name(binding, "volume");
    gst_control_binding_set_control_source(binding, csource);
    gst_element_add_control_binding(GST_ELEMENT(audioSrc), binding);*/

    // Start the GStreamer pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the GMainLoop to allow the interpolation to happen
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Clean up
    g_main_loop_unref(loop);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_deinit();

    return 0;
}

