#include <gst/gst.h>
#include <gst/gsturi.h>

int main (int argc, char *argv[]) {
  GstElement *pipeline, *source, *sink;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create a pipeline */
  pipeline = gst_pipeline_new ("my-pipeline");

  /* Create a source element from the given URI */
  source = gst_element_make_from_uri (GST_URI_TYPE_FILE, "file:///home/bhargav/Documents/gstreamer_sample/video.mp4", NULL);

  /* Create a sink element */
  sink = gst_element_factory_make ("autovideosink", NULL);

  /* Add the source and sink elements to the pipeline */
  gst_bin_add (GST_BIN (pipeline), source);
  gst_bin_add (GST_BIN (pipeline), sink);

  /* Link the source element's pad to the sink element's pad */
  gst_element_link (source, sink);

  /* Set the pipeline to the PLAYING state */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Wait for the pipeline to finish */
  gst_object_unref (pipeline);

  return 0;
}
