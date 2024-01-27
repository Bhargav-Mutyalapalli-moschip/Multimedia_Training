#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstSegment segment;
    GstFormat format = GST_FORMAT_TIME;
    guint64 position = 1000000000; // Replace with your desired position

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Initialize the segment with your desired values
    gst_segment_init(&segment, format);
    gst_segment_set_newsegment_full(&segment, TRUE, 1.0, format, 0, GST_CLOCK_TIME_NONE, 0);

    // Calculate the running time using gst_segment_to_running_time()
    guint64 running_time = gst_segment_to_running_time(&segment, format, position);

    // Print the running time
    g_print("Running Time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(running_time));

    return 0;
}

