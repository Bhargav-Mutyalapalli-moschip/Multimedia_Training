#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *sink;
    
    /* Initialize GStreamer */
    gst_init(&argc, &argv);
    
    /* Create a pipeline */
    pipeline = gst_pipeline_new("mypipeline");
    
    /* Create elements */
    source = gst_element_factory_make("fakesrc", "source");
    sink = gst_element_factory_make("fakesink", "sink");
    
    /* Add elements to the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    
    /* Set the state of the pipeline to playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    /* Synchronize the state of source with its parent (the pipeline) */
    gst_element_sync_state_with_parent(source);
    
    /* ... do some processing ... */
    
    /* Set the state of the pipeline to NULL */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    
    /* Clean up */
    gst_object_unref(pipeline);
    
    return 0;
}

