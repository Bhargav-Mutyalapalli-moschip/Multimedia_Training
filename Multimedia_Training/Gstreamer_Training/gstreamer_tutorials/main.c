#include<stdio.h>
#include<stdlib.h>
#include<gst/gst.h>
int main(int argc , char *argv[])
{
	char command;
	int state=GST_STATE_PLAYING,flip_mode=0;
	GstElement *pipeline,*videoflip;
	gst_init(&argc,&argv);
	pipeline=gst_parse_launch("v4l2src ! videoconvert ! videoflip name=videoflip_element ! ximagesink",NULL);
	videoflip=gst_bin_get_by_name(GST_BIN(pipeline),"videoflip_element");
	gst_element_set_state(pipeline,GST_STATE_PLAYING);
	while(1)
	{
		command=getchar();
		switch(command)
		{
			case 'p' :
				state=(state == GST_STATE_PLAYING) ? GST_STATE_NULL : GST_STATE_PLAYING;
				gst_element_set_state(pipeline,state);
				break;
			case 'f' :
				g_object_set(videoflip,"video-direction",++flip_mode % 9,NULL);
				break;
			case 'q' :
				gst_element_set_state(pipeline,GST_STATE_NULL);
				gst_object_unref(videoflip);
				gst_object_unref(pipeline);
				exit(0);
				break;
			}
	}

}

				


