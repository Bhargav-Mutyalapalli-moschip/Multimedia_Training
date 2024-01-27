#include <gst/gst.h>
#include <gst/video/video.h>

int main(int argc, char *argv[]) {
    GstVideoDirection direction;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Set the desired video direction
    direction = GST_VIDEO_DIRECTION_CLOCKWISE_90;

    // Create a GStreamer pipeline (you can replace this with your actual pipeline)
    GstElement *pipeline = gst_pipeline_new("video-pipeline");
    
    // Create video elements and configure the pipeline (replace with your elements)
    GstElement *source=gst_element_factory_make("videotestsrc","video-test");
    GstElement *sink=gst_element_factory_make("autovideosink","auto-sink");
    gst_bin_add(source,sink);
    gst_element_link(source,sink);

    // Playing state
    gst_element_set_state(pipeline, GST_STATE_PLAYING);



    // Check the value of direction and take appropriate action
    switch (direction) {
        case GST_VIDEO_DIRECTION_CLOCKWISE_90:
            g_print("Video direction is clockwise 90 degrees.\n");
            // Implement your logic for handling this direction
            break;
        case GST_VIDEO_DIRECTION_CLOCKWISE_180:
            g_print("Video direction is clockwise 180 degrees.\n");
            // Implement your logic for handling this direction
            break;
        case GST_VIDEO_DIRECTION_CLOCKWISE_270:
            g_print("Video direction is clockwise 270 degrees.\n");
            // Implement your logic for handling this direction
            break;
        case GST_VIDEO_DIRECTION_IDENTITY:
            g_print("Video direction is identity (no rotation).\n");
            // Implement your logic for handling this direction
            break;
        default:
            g_print("Unknown video direction.\n");
            // Handle an unknown direction if needed
            break;
    }

    // Clean up GStreamer resources
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_deinit();

    return 0;
}

