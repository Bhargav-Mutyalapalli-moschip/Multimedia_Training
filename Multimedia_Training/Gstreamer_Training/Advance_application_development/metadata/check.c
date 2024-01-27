#include<stdio.h>
#include<gst/gst.h>


int main(int argc,char *argv[])
{
	//gstreamer initialization
	gst_init(&argc,&argv);

	GstElement *source;
	gchar *uri;
	gboolean val;

	/*if (gst_uri_is_valid (argv[1])) {
		uri = g_strdup (argv[1]);
	} else {
		uri = gst_filename_to_uri (argv[1], NULL);
	}*/
	/*val=gst_uri_is_valid(argv[1]);
	if(val == TRUE)
		g_print("TRUE\n");
	else
		g_print("FALSE\n");
	if(val == FALSE)
	{
		uri=gst_filename_to_uri(argv[1],NULL);
		g_print("The path:%s\n",uri);
	}
	else
	{
		uri=g_strdup(argv[1]);
		g_print("The path:%s\n",uri);
	}*/
	//gchar *gst_uri_get_location(const gchar *uri);
	/*uri=gst_uri_get_location(argv[1]);
	g_print("The location:%s\n",uri);*/





}
