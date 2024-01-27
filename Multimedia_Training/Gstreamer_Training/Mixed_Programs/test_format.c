#include <stdio.h>
#include <gst/gst.h>

int main(int argc, char* argv[]) {
    gst_init(&argc, &argv);

    // Define a video format
    GstVideoFormat format = GST_VIDEO_FORMAT_RGB;

    // Convert the video format to a string
    const gchar* formatStr = gst_video_format_to_string(format);

    if (formatStr) {
        printf("Video Format: %s\n", formatStr);
    } else {
        printf("Invalid Video Format\n");
    }

    return 0;
}

