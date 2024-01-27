#include <gst/gst.h>
#include<gst/video/video.h>

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Define your video information (you need to provide these values)
    gint width = 640; // Replace with your actual width
    gint height = 480; // Replace with your actual height
    GstVideoInfo video_info;
    gst_video_info_set_format(&video_info, GST_VIDEO_FORMAT_RGB, width, height);
    GstBuffer *video_buffer = gst_buffer_new();

    // Create a GstVideoFrame
    GstVideoFrame vframe;
    if (gst_video_frame_map(&vframe, &video_info, video_buffer, GST_MAP_WRITE)) {
        guint8 *pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0);
        guint stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0);
        guint pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe, 0);

        guint h, w;
        for (h = 0; h < height; ++h) {
            for (w = 0; w < width; ++w) {
                guint8 *pixel = pixels + h * stride + w * pixel_stride;

                memset(pixel, 0, pixel_stride);
            }
        }

        gst_video_frame_unmap(&vframe);
    }

    // Clean up
    gst_buffer_unref(video_buffer);
    gst_deinit();

    return 0;
}

