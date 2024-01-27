#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *audio_source, *tee, *audio_queue, *audio_convert, *audio_resample, *audio_sink;
    GstElement *video_queue, *visual, *video_convert, *video_sink;
    GstBus *bus;
    GstMessage *msg;
    GstPad *tee_audio_pad, *tee_video_pad;
    GstPad *queue_audio_pad, *queue_video_pad;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Initialize the plugin system - this was missing in the previous code */
    gst_plugin_feature_init(NULL, NULL);

    /* Create the elements (as before) ... */

    /* Configure elements (as before) ... */

    /* Link all elements that can be automatically linked because they have "Always" pads (as before) ... */

    /* Manually link the Tee, which has "Request" pads (as before) ... */

    /* Start playing the pipeline (as before) ... */

    /* Wait until error or EOS (as before) ... */

    /* Release the request pads from the Tee, and unref them (as before) ... */

    /* Free resources (as before) ... */

    return 0;
}

