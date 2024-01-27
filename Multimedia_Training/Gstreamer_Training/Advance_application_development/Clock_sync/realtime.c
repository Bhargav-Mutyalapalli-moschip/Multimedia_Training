#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create a GStreamer pipeline (not required for time conversion)
    GstElement *pipeline = gst_pipeline_new("my-pipeline");

    // Create a GStreamer clock
    GstClock *pipeline_clock = gst_system_clock_obtain(); // Use the system clock

    // Get the current time
    GstClockTime gst_time = gst_clock_get_time(pipeline_clock);

    // Convert GStreamer time to microseconds
    guint64 microseconds = GST_TIME_AS_USECONDS(gst_time);

    // Convert microseconds to real-time
    guint64 milliseconds = microseconds / 1000;
    guint64 seconds = milliseconds / 1000;
    guint64 minutes = seconds / 60;
    guint64 hours = minutes / 60;

    // Calculate remaining minutes, seconds, and milliseconds
    guint64 remaining_minutes = minutes % 60;
    guint64 remaining_seconds = seconds % 60;
    guint64 remaining_milliseconds = milliseconds % 1000;

    // Print real-time values
    g_print("GStreamer Time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(gst_time));
    g_print("Real-Time: %llu hours, %llu minutes, %llu seconds, %llu milliseconds\n",
            hours, remaining_minutes, remaining_seconds, remaining_milliseconds);

    // Cleanup
    gst_object_unref(pipeline_clock); // Release the clock

    return 0;
}

