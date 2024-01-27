#include <gst/gst.h>

typedef struct _GstMyFilter {
  GstElement element;
} GstMyFilter;

static gboolean
gst_my_filter_src_query (GstPad    *pad,
                         GstObject *parent,
                         GstQuery  *query)
{
  gboolean ret = FALSE;
  GstMyFilter *filter = GST_MY_FILTER (parent);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:
      {
        gint64 position = -1;
        // Query the current position of the source element (assuming it's a file source)
        if (gst_pad_peer_query_position (pad, GST_FORMAT_TIME, &position)) {
          gst_query_set_position (query, GST_FORMAT_TIME, position);
          ret = TRUE;
        }
      }
      break;
    case GST_QUERY_DURATION:
      {
        gint64 duration = -1;
        // Query the duration of the source element (assuming it's a file source)
        if (gst_pad_peer_query_duration (pad, GST_FORMAT_TIME, &duration)) {
          gst_query_set_duration (query, GST_FORMAT_TIME, duration);
          ret = TRUE;
        }
      }
      break;
    case GST_QUERY_CAPS:
      {
        // Create and set the supported caps for your source element (e.g., video/x-raw)
        GstCaps *caps = gst_caps_new_simple ("video/x-raw",
                                             "format", G_TYPE_STRING, "RGBA",
                                             "width", G_TYPE_INT, 640,
                                             "height", G_TYPE_INT, 480,
                                             NULL);
        if (caps != NULL) {
          gst_query_set_caps (query, caps);
          gst_caps_unref (caps);
          ret = TRUE;
        }
      }
      break;
    default:
      /* Just call the default handler */
      ret = gst_pad_query_default (pad, parent, query);
      break;
  }
  return ret;
}

int main(int argc, char *argv[]) {
  GstMyFilter *filter;
  GstElement *pipeline, *src, *sink;
  GstBus *bus;
  GstMessage *msg;
  gboolean ret;

  // Initialize GStreamer
  gst_init (&argc, &argv);

  // Create the elements
  filter = g_object_new (GST_TYPE_MY_FILTER, NULL);
  pipeline = gst_pipeline_new ("my-pipeline");
  src = gst_element_factory_make ("filesrc", "source");
  sink = gst_element_factory_make ("autovideosink", "sink");

  if (!filter || !pipeline || !src || !sink) {
    g_print ("Not all elements could be created.\n");
    return -1;
  }

  // Set the input video file for the file source element
  g_object_set (src, "location", "input.mp4", NULL);

  // Add elements to the pipeline
  gst_bin_add_many (GST_BIN (pipeline), src, filter, sink, NULL);

  // Link elements
  if (gst_element_link_many (src, filter, sink, NULL) != TRUE) {
    g_print ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  // Set the query function for the source pad of your filter element
  GstPad *srcpad = gst_element_get_static_pad (filter, "src");
  gst_pad_set_query_function (srcpad, gst_my_filter_src_query);
  gst_object_unref (srcpad);

  // Start the pipeline
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);

  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_print ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  // Wait until error or EOS
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  // Release resources
  if (msg != NULL)
    gst_message_unref (msg);

  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 0;
}

