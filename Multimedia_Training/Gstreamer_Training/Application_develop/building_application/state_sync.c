#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *sink;
    GstBus *bus;
    GstMessage *msg;
    
    /* Initialize GStreamer */
    gst_init(&argc, &argv);
    
    /* Create a pipeline */
    pipeline = gst_pipeline_new("mypipeline");
    
    /* Create elements */
    source = gst_element_factory_make("fakesrc", "source");
    sink = gst_element_factory_make("fakesink", "sink");
    
    /* Check if elements were created successfully */
    if (!pipeline || !source || !sink) {
        g_print("One or more elements could not be created. Exiting.\n");
        return -1;
    }
    
    /* Add elements to the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    
    /* Link the elements together (source -> sink) */
    if (!gst_element_link(source, sink)) {
        g_print("Elements could not be linked. Exiting.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    
    /* Set the state of the pipeline to playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    /* Synchronize the state of the source element with its parent (the pipeline) */
    gst_element_sync_state_with_parent(source);
    
    /* Watch for messages from the pipeline's bus */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    
    /* Parse and handle messages */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;
        
        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                /* Handle other message types if needed */
                break;
        }
        
        gst_message_unref(msg);
    }
    
    /* Set the state of the pipeline to NULL */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    
    /* Clean up */
    gst_object_unref(bus);
    gst_object_unref(pipeline);
    
    return 0;
}

