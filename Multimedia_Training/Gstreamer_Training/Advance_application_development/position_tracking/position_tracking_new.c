
static gboolean
cb_print_position(GstElement *pipeline) {
    gint64 pos, len;

    /*if (gst_element_query_position(pipeline, GST_FORMAT_TIME, &pos) &&
        gst_element_query_duration(pipeline, GST_FORMAT_TIME, &len)) {
        g_print("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
                GST_TIME_ARGS(pos), GST_TIME_ARGS(len));
    }*/
    /*if (gst_element_query_position(pipeline,GST_FORMAT_BYTES, &pos) &&
        gst_element_query_duration(pipeline, GST_FORMAT_TIME, &len)) {
        g_print("Time: %ld / %" GST_TIME_FORMAT "\r",
                pos, GST_TIME_ARGS(len));
    }*/
    /*if (gst_element_query_position(pipeline,GST_FORMAT_DEFAULT, &pos) &&
        gst_element_query_duration(pipeline, GST_FORMAT_TIME, &len)) {
        g_print("Time: % " G_GINT64_FORMAT" / %" GST_TIME_FORMAT "\r",
                pos, GST_TIME_ARGS(len));
    }*/
    if (gst_element_query_position(pipeline,GST_FORMAT_BYTES, &pos) &&
        gst_element_query_duration(pipeline, GST_FORMAT_PERCENT, &len)) {
        g_print("Time: %ld,%ld\r",pos, len);
    }


    /* call me again */
    return TRUE;
}
static void
seek_to_time (GstElement *pipeline,
          gint64      time_nanoseconds)
{
  if (!gst_element_seek (pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, time_nanoseconds,
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
    g_print ("Seek failed!\n");
  }
}

int main(int argc, char *argv[]) {
    GstElement *pipeline;
    GMainLoop *loop;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);

    /* Create the pipeline */
    pipeline = gst_parse_launch("playbin uri=file:///home/bhargav/Documents/song.mp3", NULL);

    if (!pipeline) {
        g_printerr("Pipeline could not be created. Exiting.\n");
        return -1;
    }

    /* Set the pipeline to the playing state */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Run the loop to periodically print position and duration */
    g_timeout_add(1000, (GSourceFunc)cb_print_position, pipeline);
    // Seek to a specific time (e.g., 30 seconds)
    seek_to_time(pipeline, GST_SECOND * 30);
    g_main_loop_run(loop);

    /* Clean up and exit */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}

