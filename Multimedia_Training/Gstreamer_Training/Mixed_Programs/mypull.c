#include<stdio.h>
#include<gst/gst.h>

GstElement *source ,*vconvert,*vscale ,*sink,*pipeline;

static GstFlowReturn sample_callback(GstElement *appsink , gpointer data)
{
	g_print("sample call back of appsink was called\n");
	GstSample *sample;
    g_signal_emit_by_name(appsink, "pull-sample", &sample, NULL);

    if (sample)
    {
	    g_print("sample was true\n");
        GstBuffer *buffer = gst_sample_get_buffer(sample);
    buffer = gst_sample_get_buffer (sample);
    
	guint buffer_size_bytes = gst_buffer_get_size(buffer);
	g_print("Buffer size: %u bytes\n", buffer_size_bytes);    
//      gdk_pixbuf_save (pixbuf, "snapshot.png", "png", &error, NULL);

        gst_sample_unref(sample);
    }

    return GST_FLOW_OK;
}
static void pad_add(GstElement *element ,GstPad *pad ,gpointer data)
{
	GstPad *sinkpad;
	sinkpad = gst_element_get_static_pad(vconvert,"sink");

	if(gst_pad_link(pad ,sinkpad))
	{
		g_print("failed to link the pads\n");
		return;
	}
}
void main(int args , char *argv[])
{
	GMainLoop *loop;
	gst_init(&args , &argv);
	
	loop = g_main_loop_new(NULL,FALSE);

	pipeline = gst_pipeline_new("mypipeline");
	source = gst_element_factory_make("uridecodebin","source");
	vconvert = gst_element_factory_make("videoconvert","videoconvert");
	vscale = gst_element_factory_make("videoscale","vscale");
	sink = gst_element_factory_make("appsink","sink");

//	sink = gst_element_factory_make("autovideosink","sink");

	if(!pipeline || !source || !vconvert || !vscale || !sink)
	{
		g_print("failed to create the elements\n");
		return;
	}
	g_object_set(source ,"uri","file:///home/softnautics/Downloads/kgf2_telugu.mp4",NULL);

	gst_bin_add_many(GST_BIN(pipeline),source ,vconvert,vscale,sink,NULL);

	gboolean ret = gst_element_link_many(vconvert ,vscale,sink,NULL);

	if(ret!=TRUE)
	{
		g_print("failed to link the elements\n");
		return;
	}

	g_signal_connect(source , "pad-added",G_CALLBACK(pad_add),NULL);

	g_signal_connect(sink,"new-sample",G_CALLBACK(sample_callback),NULL);

		/* g_object_set (G_OBJECT (sink), "caps",
		 gst_caps_new_simple ("video/x-raw",
		 "format", G_TYPE_STRING, "RGB16",
		 "width", G_TYPE_INT, 106,
		 "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
		 NULL), NULL);*/

	gst_element_set_state(pipeline,GST_STATE_PLAYING);

	GstPad *pad;
	GstCaps *caps;

	pad = gst_element_get_static_pad(vscale,"src");

	
	g_main_loop_run(loop);

	g_object_unref(pipeline);
	
	g_main_loop_quit(loop);
}
