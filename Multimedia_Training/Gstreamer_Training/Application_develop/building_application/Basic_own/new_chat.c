#include <gst/gst.h>

// Function to recursively search for an audio pad
static GstPad* find_audio_pad(GstElement *element) {
    if (GST_IS_ELEMENT(element)) {
        GList *element_pads = gst_element_get_static_pad(GST_ELEMENT(element));
        for (GList *iterator = element_pads; iterator != NULL; iterator = iterator->next) {
            GstPad *pad = GST_PAD(iterator->data);
            const gchar *pad_name = gst_pad_get_name(pad);

            // Check if this is the pad you are interested in (based on pad_name or capabilities)
            if (g_str_has_prefix(pad_name, "audio_0")) {
                g_list_free(element_pads); // Free the list of pads
                return pad;
            }
        }
        g_list_free(element_pads); // Free the list of pads

        // If the desired pad was not found, recursively search the internal elements
        GstIterator *iter = gst_bin_iterate_elements(GST_BIN(element));
        GstElement *internal_element;
        while (gst_iterator_next(iter, (gpointer)&internal_element) == GST_ITERATOR_OK) {
            GstPad *audio_pad = find_audio_pad(internal_element);
            if (audio_pad != NULL) {
                gst_iterator_free(iter);
                return audio_pad;
            }
        }
        gst_iterator_free(iter);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Create GStreamer elements: pipeline, decodebin, audio_sink, etc.
    GstElement *pipeline, *decodebin, *audio_sink;

    // Initialize and configure your elements...

    // Create the pipeline
    pipeline = gst_pipeline_new("audio-pipeline");
    decodebin = gst_element_factory_make("decodebin", "decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");

    // Check if elements were created successfully
    if (!pipeline || !decodebin || !audio_sink) {
        g_printerr("Failed to create GStreamer elements.\n");
        return -1;
    }

    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), decodebin, audio_sink, NULL);

    // Link the elements (your specific pipeline configuration may vary)
    if (!gst_element_link(decodebin, audio_sink)) {
        g_printerr("Failed to link elements in the pipeline.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the decodebin to PAUSED state to probe its source pads
    GstStateChangeReturn ret = gst_element_set_state(decodebin, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set decodebin to PAUSED state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Find the audio pad
    GstPad *audio_pad = find_audio_pad(decodebin);

    if (audio_pad != NULL) {
        g_print("Successfully retrieved the audio pad.\n");

        // You can use the audio_pad as needed.
        // For example, you can link it to your audio sink:
        if (gst_pad_link(audio_pad, gst_element_get_static_pad(audio_sink, "sink")) == GST_PAD_LINK_OK) {
            g_print("Linked audio pad to audio sink.\n");
        } else {
            g_printerr("Failed to link audio pad to audio sink.\n");
        }

        // Release the audio pad when done
        gst_object_unref(audio_pad);
    } else {
        g_printerr("Failed to retrieve the audio pad.\n");
    }

    // Set the pipeline to PLAYING state to start processing data
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to PLAYING state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Run the GStreamer main loop...

    // Cleanup and release resources...

    return 0;
}

