#include <gst/gst.h>

int main(int argc, char *argv[])
{
    GstElement *pipeline, *source, *sink, *filter;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    pipeline = gst_pipeline_new("video-pipeline");
    source = gst_element_factory_make("filesrc", "source");
    filter = gst_element_factory_make("decodebin", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !filter || !sink)
    {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Set the input file path for the source element */
    g_object_set(G_OBJECT(source), "location", "/path/to/your/video/file.mp4", NULL);

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);

    /* Link the source to the decodebin */
    if (!gst_element_link(source, filter))
    {
        g_printerr("Source and decodebin could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Get the pad from decodebin and link it to the sink pad */
    GstPad *pad = gst_element_get_static_pad(filter, "src");
    GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");
    if (gst_pad_link(pad, sink_pad) != GST_PAD_LINK_OK)
    {
        g_printerr("Decodebin and sink pads could not be linked.\n");
        gst_object_unref(pipeline);
        gst_object_unref(pad);
        gst_object_unref(sink_pad);
        return -1;
    }
    gst_object_unref(pad);
    gst_object_unref(sink_pad);

    /* Start playing */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL)
    {
        // Handle messages here
        gst_message_unref(msg);
    }

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

