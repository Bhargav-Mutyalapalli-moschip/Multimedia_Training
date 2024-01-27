#include
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NV12 format has the Y component followed by the UV components (interleaved)
typedef struct {
    uint8_t *y_plane;
    uint8_t *uv_plane;
    int width;
    int height;
} NV12Frame;

// Function to impose a logo onto an NV12 video frame
void impose_logo_nv12(NV12Frame *video_frame, uint8_t *logo_data, int logo_width, int logo_height, int x, int y) {
    if (!video_frame || !video_frame->y_plane || !video_frame->uv_plane) {
        return;
    }

    int video_width = video_frame->width;
    int video_height = video_frame->height;

    // Ensure the logo does not go out of bounds
    if (x + logo_width > video_width || y + logo_height > video_height) {
        return;
    }

    // Copy the original video frame to a new buffer
    NV12Frame original_frame;
    original_frame.width = video_width;
    original_frame.height = video_height;
    original_frame.y_plane = (uint8_t *)malloc(video_width * video_height);
    original_frame.uv_plane = (uint8_t *)malloc(video_width * video_height / 2);
    memcpy(original_frame.y_plane, video_frame->y_plane, video_width * video_height);
    memcpy(original_frame.uv_plane, video_frame->uv_plane, video_width * video_height / 2);

    // Loop through the rows and columns of the logo image and impose it on the new buffer
    for (int row = 0; row < logo_height; row++) {
        for (int col = 0; col < logo_width; col++) {
            int logo_x = col;
            int logo_y = row;
            int video_x = x + col;
            int video_y = y + row;

            // Calculate the offsets for Y and UV components
            int logo_y_offset = logo_y * logo_width + logo_x;
            int video_y_offset = video_y * video_width + video_x;
            int uv_offset = (video_y / 2) * video_width + (video_x / 2) * 2;

            // Extract Y and UV components from the logo image
            uint8_t y_value = logo_data[logo_y_offset];
            uint8_t u_value = logo_data[logo_width * logo_height + (logo_y / 2) * (logo_width / 2) + logo_x / 2];
            uint8_t v_value = logo_data[logo_width * logo_height + (logo_y / 2) * (logo_width / 2) + logo_x / 2 + 1];

            // Impose the Y component of the logo onto the new buffer
            original_frame.y_plane[video_y_offset] = y_value;

            // Impose the UV components of the logo onto the new buffer
            original_frame.uv_plane[uv_offset] = u_value;
            original_frame.uv_plane[uv_offset + 1] = v_value;
        }
    }

    // Copy the imposed frame back to the original video frame
    memcpy(video_frame->y_plane, original_frame.y_plane, video_width * video_height);
    memcpy(video_frame->uv_plane, original_frame.uv_plane, video_width * video_height / 2);

    // Cleanup: Free memory
    free(original_frame.y_plane);
    free(original_frame.uv_plane);
}

int main() {
    // Simulate a video frame in NV12 format
    int video_width = 1920; // Width of the video frame
    int video_height = 1080; // Height of the video frame
    NV12Frame video_frame;
    video_frame.width = video_width;
    video_frame.height = video_height;
    video_frame.y_plane = (uint8_t *)malloc(video_width * video_height);
    video_frame.uv_plane = (uint8_t *)malloc(video_width * video_height / 2);

    // Simulate a logo image
    int logo_width = 100; // Width of the logo
    int logo_height = 50; // Height of the logo
    uint8_t *logo_data = (uint8_t *)malloc(logo_width * logo_height * 1.5); // NV12 format

    // Load your logo image data into the logo_data buffer

    // Impose the logo onto the video frame at position (100, 100)
    impose_logo_nv12(&video_frame, logo_data, logo_width, logo_height, 100, 100);

    // Cleanup: Free memory
    free(video_frame.y_plane);
    free(video_frame.uv_plane);
    free(logo_data);

    return 0;
}

