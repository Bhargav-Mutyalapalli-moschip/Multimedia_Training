#include <gst/gst.h>
#include <gst/navigation/gstnavigation.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *navigationHandler;
    GstNavigation *navigation;
    GstEvent *navigationEvent;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the pipeline
    pipeline = gst_pipeline_new("my-pipeline");

    // Create a custom navigation handler element that implements GstNavigation
    navigationHandler = gst_element_factory_make("customnavigationhandler", "navigationhandler");

    // Add the navigation handler element to the pipeline
    gst_bin_add(GST_BIN(pipeline), navigationHandler);

    // Get the GstNavigation interface from the navigation handler
    navigation = GST_NAVIGATION(navigationHandler);

    // Create a navigation event (example: "select" event)
    navigationEvent = gst_navigation_event_new_select();

    // Send the navigation event to the pipeline
    gst_navigation_send_event(navigation, navigationEvent);

    // Set the pipeline to the "playing" state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the GMain Loop
    GMainLoop *mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

    // Clean up resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(mainLoop);
    gst_deinit();

    return 0;
}

