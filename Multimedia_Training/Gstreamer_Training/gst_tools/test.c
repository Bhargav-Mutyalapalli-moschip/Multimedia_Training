#include <gst/gst.h>
#include <string.h>

#define AUDIO_DATA_SIZE 1024  // Size of each audio chunk

typedef struct {
  GstElement *pipeline;
  GstElement *appsrc;
} CustomData;

static void start_feed(GstElement *source, guint size, CustomData *data) {
  // Push audio data to appsrc when needed
  GstBuffer *buffer = gst_buffer_new_allocate(NULL, AUDIO_DATA_SIZE, NULL);
  // Fill buffer with your audio data
  // ...
  GstFlowReturn ret;
  g_signal_emit_by_name(data->appsrc, "push-buffer", buffer, &ret);
  gst_buffer_unref(buffer);
}

static void stop_feed(GstElement *source, CustomData *data) {
  // Stop feeding audio data to appsrc when enough has been played
  // Implement as needed
}

int main(int argc, char *argv[]) {
  CustomData data;
  GstBus *bus;
  GMainLoop *main_loop;

  // Initialize GStreamer
  gst_init(&argc, &argv);

  // Create the pipeline
  data.pipeline = gst_parse_launch("appsrc name=/home/bhargav/Documents/gstreamer_sample/gst_tools/varma.mp3 ! audioconvert ! audioresample ! autoaudiosink", NULL);
  data.appsrc = gst_bin_get_by_name(GST_BIN(data.pipeline), "mysource");

  // Configure appsrc
  g_object_set(data.appsrc, "caps",
               gst_caps_new_simple("audio/x-raw",
                                   "format", G_TYPE_STRING, "S16LE",
                                   "rate", G_TYPE_INT, 44100,
                                   "channels", G_TYPE_INT, 2, NULL),
               "format", GST_FORMAT_TIME,
               "block", FALSE,
               NULL);

  // Connect signals for feeding data
  g_signal_connect(data.appsrc, "need-data", G_CALLBACK(start_feed), &data);
  g_signal_connect(data.appsrc, "enough-data", G_CALLBACK(stop_feed), &data);

  // Get the bus and handle errors
  bus = gst_element_get_bus(data.pipeline);
  gst_bus_add_signal_watch(bus);

  // Create and run the GMainLoop
  main_loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(main_loop);

  // Cleanup
  gst_element_set_state(data.pipeline, GST_STATE_NULL);
  gst_object_unref(data.pipeline);
  g_main_loop_unref(main_loop);

  return 0;
}

