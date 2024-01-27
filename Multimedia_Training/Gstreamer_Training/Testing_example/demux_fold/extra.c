//retrieve the name 
    /*video_sink_pad = gst_element_get_static_pad(video_queue, "sink");
    if(!video_sink_pad)
	    g_print("failed to retrieve for video sink pad\n");
    audio_sink_pad = gst_element_get_static_pad(audio_queue, "sink");
    if(!audio_sink_pad)
	    g_print("failed to retrieve for audio sink pad\n");
    //video_src_pad=gst_element_get_compatible_pad(demuxer, video_sink_pad, NULL);
    //video_src_pad=gst_element_get_static_pad(demuxer,"video_%u");
    //video_src_pad=gst_element_request_pad_simple(demuxer,"video_%u");
    if(!video_src_pad)
	    g_print("failed to compact pad for video src pad\n");
    if (gst_pad_link(gst_element_get_compatible_pad(demuxer, video_sink_pad, NULL), video_sink_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link video pads.\n");
    }

    // Link the demuxer to the audio queue
    if (gst_pad_link(gst_element_get_compatible_pad(demuxer, audio_sink_pad, NULL), audio_sink_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link audio pads.\n");
    }*/
     const GList *pads;
     GstStaticPadTemplate *padtemplate;
     source_factory = gst_element_factory_find ("qtdemux");
     pads = gst_element_factory_get_static_pad_templates (source_factory);
     while (pads) 
     {
    padtemplate = pads->data;
    pads = g_list_next (pads);

    if (padtemplate->direction == GST_PAD_SRC)
      g_print ("  SRC template: '%s'\n", padtemplate->name_template);
     }
