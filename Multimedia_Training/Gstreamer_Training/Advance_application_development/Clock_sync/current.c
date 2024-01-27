#include <gst/gst.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline (not required for time conversion)
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create a GStreamer clock
    GstClock *pipeline_clock = gst_system_clock_obtain(); // Use the system clock

    // Get the GStreamer time
    GstClockTime gst_time = gst_clock_get_time(pipeline_clock);

    // Convert GStreamer time to nanoseconds (UNIX timestamp)
    gint64 unix_timestamp_ns = GST_TIME_AS_NSECONDS(gst_time);

    // Convert UNIX timestamp in nanoseconds to time_t (seconds since epoch)
    time_t unix_timestamp_seconds = unix_timestamp_ns / GST_SECOND;

    // Convert time_t to a struct tm representing local time
    struct tm *local_time = localtime(&unix_timestamp_seconds);

    // Format and print the date and time
    char time_buffer[80];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", local_time);

    g_print("GStreamer Time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(gst_time));
    g_print("Current Time: %s\n", time_buffer);

    // Cleanup
    gst_object_unref(pipeline_clock); // Release the clock

    return 0;
}

