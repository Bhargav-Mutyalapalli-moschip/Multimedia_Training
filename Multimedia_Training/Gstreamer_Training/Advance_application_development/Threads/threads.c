#include <gst/gst.h>
#include <glib.h>

static GMainLoop *loop;

// Structure to hold data for each pipeline thread
typedef struct {
    GstElement *pipeline;
    GThread *thread;
} PipelineThreadData;

// Function to set up the audio pipeline
static gboolean setup_audio_pipeline(GstElement *pipeline) {
    GstElement *audio_src, *audio_decoder, *audio_sink;
    GstPad *audio_sink_pad;
    
    // Create audio elements
    audio_src = gst_element_factory_make("uridecodebin", "audio-source");
    audio_decoder = gst_element_factory_make("autoaudiosink", "audio-decoder");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio-sink");

    if (!audio_src || !audio_decoder || !audio_sink) {
        g_print("Failed to create audio elements\n");
        return FALSE;
    }

    // Add audio elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), audio_src, audio_decoder, audio_sink, NULL);

    // Link audio elements
    if (!gst_element_link_many(audio_src, audio_decoder, audio_sink, NULL)) {
        g_print("Failed to link audio elements\n");
        return FALSE;
    }

    // Get the sink pad of the audio sink and add a probe for handling EOS (End of Stream)
    audio_sink_pad = gst_element_get_static_pad(audio_sink, "sink");
    gst_pad_add_probe(audio_sink_pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, 
        (GstPadProbeCallback)eos_probe_callback, NULL, NULL);

    return TRUE;
}

// Function to set up the video pipeline
static gboolean setup_video_pipeline(GstElement *pipeline) {
    GstElement *video_src, *video_decoder, *video_sink;
    
    // Create video elements
    video_src = gst_element_factory_make("uridecodebin", "video-source");
    video_decoder = gst_element_factory_make("autovideosink", "video-decoder");
    
    if (!video_src || !video_decoder) {
        g_print("Failed to create video elements\n");
        return FALSE;
    }

    // Add video elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline), video_src, video_decoder, NULL);

    // Link video elements
    if (!gst_element_link_many(video_src, video_decoder, NULL)) {
        g_print("Failed to link video elements\n");
        return FALSE;
    }

    return TRUE;
}

// Function to handle EOS (End of Stream) event for audio
static GstPadProbeReturn eos_probe_callback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    if (GST_EVENT_TYPE(GST_PAD_PROBE_INFO_DATA(info)) == GST_EVENT_EOS) {
        g_print("Audio EOS reached\n");
        g_main_loop_quit(loop);
    }
    return GST_PAD_PROBE_OK;
}

// Function to set up and run the GStreamer pipeline
static gpointer pipeline_thread_func(gpointer user_data) {
    PipelineThreadData *thread_data = (PipelineThreadData *)user_data;
    GstElement *pipeline = thread_data->pipeline;
    
    // Create and set up audio and video pipelines
    if (!setup_audio_pipeline(pipeline) || !setup_video_pipeline(pipeline)) {
        g_print("Failed to set up pipelines\n");
        return NULL;
    }

    // Start the pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);

    // Stop the pipeline
    gst_element_set_state(pipeline, GST_STATE_NULL);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the main pipeline
    GstElement *pipeline = gst_pipeline_new("audio-video-pipeline");
    if (!pipeline) {
        g_print("Failed to create the main pipeline\n");
        return -1;
    }

    loop = g_main_loop_new(NULL, FALSE);

    // Create and set up a thread for the main pipeline
    PipelineThreadData thread_data = {pipeline, NULL};
    thread_data.thread = g_thread_new("PipelineThread", pipeline_thread_func, &thread_data);

    // Wait for the thread to finish
    g_thread_join(thread_data.thread);

    // Release resources
    g_main_loop_unref(loop);
    gst_object_unref(pipeline);

    return 0;
}

