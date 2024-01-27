static GstFlowReturn transform_ip(GstBaseTransform* trans, GstBuffer* inbuf) {
    MyLogoFilter* filter = GST_MYLOGOFILTER(trans);

    // Extract video frame data (NV12 format) from the input buffer.

    // Impose the logo onto the video frame.

    // Apply animation effects (rotate and scroll) as per user settings.

    // Push the modified video frame to the output buffer.

    // Push the buffer downstream.
    return gst_pad_push(filter->srcpad, outbuf);
}

