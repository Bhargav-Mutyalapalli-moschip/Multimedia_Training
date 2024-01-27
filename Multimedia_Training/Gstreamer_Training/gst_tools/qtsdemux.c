#include<stdio.h>
#include<gst/gst.h>
#include<stdlib.h>

static void pad_added_handler(GstElement *element, GstPad *new_pad, gpointer user_data) {
	// Check the pad's caps to determine if it's audio or video
	GstCaps *caps = gst_pad_get_current_caps(new_pad);
	const gchar *caps_string = gst_caps_to_string(caps);

	if (g_strrstr(caps_string, "audio")) {
		// Handle audio pad
		// Your audio processing code here
		GstPad *sinkpad;
		GstElement *decoder = (GstElement *) data;

		g_print ("Dynamic pad created, linking demuxer/decoder\n");



		sinkpad = gst_element_get_static_pad (decoder, "sink");



		gst_pad_link (pad, sinkpad);



		gst_object_unref (sinkpad);

	} else if (g_strrstr(caps_string, "video")) {
		// Handle video pad
		// Your video processing code here
		GstPad *sinkpad;
		GstElement *decoder = (GstElement *) data;

		g_print ("Dynamic pad created, linking demuxer/decoder\n");



		sinkpad = gst_element_get_static_pad (decoder, "sink");



		gst_pad_link (pad, sinkpad);



		gst_object_unref (sinkpad);

	}

	gst_caps_unref(caps);
} 



/*void v_call(GstElement *Element , GstPad *pad , gpointer data)
  {
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  g_print ("Dynamic pad created, linking demuxer/decoder\n");



  sinkpad = gst_element_get_static_pad (decoder, "sink");



  gst_pad_link (pad, sinkpad);



  gst_object_unref (sinkpad);
  }*/



/*void a_call(GstElement *Element , GstPad *pad , gpointer data)
  {
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  g_print ("Dynamic pad created, linking demuxer/decoder\n");



  sinkpad = gst_element_get_static_pad (decoder, "sink");



  gst_pad_link (pad, sinkpad);



  gst_object_unref (sinkpad);
  }*/



int main(int args , char *argv[])
{
	GstElement *source ,*qtdemux ,*vdecoder ,*vconvert,*aconvert ,*asink,*vsink,*adecoder,*pipeline,*bin1,*bin2;

	GMainLoop *loop;

	gst_init(&args , &argv);

	loop = g_main_loop_new(NULL ,FALSE);

	source = gst_element_factory_make("filesrc","source");
	qtdemux = gst_element_factory_make("qtdemux","demuxer");
	vdecoder = gst_element_factory_make("uridecodebin","videodecoder");
	adecoder = gst_element_factory_make("vorbisdec","audiodecoder");
	vconvert = gst_element_factory_make("videoconvert","videoconverter");
	aconvert = gst_element_factory_make("audioconvert","audioconverter");
	vsink = gst_element_factory_make("autovideosink","autovideosink");
	asink = gst_element_factory_make("autoaudiosink","autoaudiosink");

	pipeline = gst_pipeline_new("mypipeline");
	bin1 = gst_bin_new("first_bin");
	bin2 = gst_bin_new("second_bin");


	if(!(source && qtdemux && vdecoder && adecoder && vconvert && aconvert && vsink && asink))
	{
		g_printerr("elements are not created sucessfully\n");
		exit(1);
	}

	g_object_set(source , "location" ,"/home/bhargav/Documents/gstreamer_sample/gst_tools/kgf2_telugu.mp4",NULL);

	gst_bin_add_many(GST_BIN(pipeline),source, qtdemux,NULL);
	gst_element_link(source , qtdemux);

	gst_bin_add_many(GST_BIN(bin1) , vdecoder ,vconvert,vsink,NULL);
	gst_element_link_many(vdecoder,vconvert,vsink,NULL);

	gst_bin_add_many(GST_BIN(bin2),adecoder,aconvert,asink,NULL);
	gst_element_link_many(adecoder,aconvert,asink,NULL);

	gst_bin_add_many(GST_BIN(pipeline),bin1,bin2,NULL);


	g_signal_connect(qtdemux , "pad-added",G_CALLBACK(pad_added_handler),vdecoder);


	gst_element_set_state(pipeline,GST_STATE_PLAYING);

	g_main_loop_run (loop);

	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);



	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_unref (loop);
	return 0;



}
