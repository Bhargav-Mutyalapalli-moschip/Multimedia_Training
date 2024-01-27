 GstMapInfo in_map, out_map;

  GError *err = NULL;

 

  if (!gst_buffer_map (buf, &in_map, GST_MAP_READ)) {

    GST_ERROR ("Failed to map input buffer");

    return GST_FLOW_ERROR;

  }

 

  // Define the desired output dimensions

  gint out_width = 1080;

  gint out_height = 720;

 

  // Calculate the scaling factors for width and height

  gdouble scale_x = (gdouble) out_width / in_map.size / 2;

  gdouble scale_y = (gdouble) out_height / in_map.size / 2;

 

  if (scale_x <= 0 || scale_y <= 0) {

    GST_ERROR ("Invalid scaling factors");

    gst_buffer_unmap (buf, &in_map);

    return GST_FLOW_ERROR;

  }

 

  // Create an output buffer with the desired dimensions

  GstBuffer *out_buf = gst_buffer_new_allocate (NULL, in_map.size * scale_x * scale_y, &err);

 

  if (err) {

    GST_ERROR ("Failed to allocate output buffer: %s", err->message);

    g_error_free (err);

    gst_buffer_unmap (buf, &in_map);

    return GST_FLOW_ERROR;

  }

 

  if (!gst_buffer_map (out_buf, &out_map, GST_MAP_WRITE)) {

    GST_ERROR ("Failed to map output buffer");

    gst_buffer_unmap (buf, &in_map);

    gst_buffer_unref (out_buf);

    return GST_FLOW_ERROR;

  }

 

  // Perform nearest neighbor interpolation to resize the frame

  for (guint y = 0; y < out_height; y++) {

    for (guint x = 0; x < out_width; x++) {

      gint in_x = (gint)(x / scale_x);

      gint in_y = (gint)(y / scale_y);

 

      guint8 *in_pixel = in_map.data + (in_y * in_map.size / 2 + in_x) * 2;

      guint8 *out_pixel = out_map.data + (y * out_map.size / 2 + x) * 2;

 

      // Copy the nearest neighbor pixel

      out_pixel[0] = in_pixel[0];

      out_pixel[1] = in_pixel[1];

    }

  }

 

  gst_buffer_unmap (buf, &in_map);

  gst_buffer_unmap (out_buf, &out_map);

 

  // Push the resized buffer to the srcpad

  GstFlowReturn ret = gst_pad_push (GST_PAD (GST_ELEMENT (parent)->srcpad), out_buf);

 

  return ret;
