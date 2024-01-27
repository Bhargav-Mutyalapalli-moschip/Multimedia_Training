#include<gst/gst.h>
int main(int argc,char *argv[])
{
	gst_init(&argc,&argv);
	GstElement *source;
	g_print("The source:%d\n",sizeof(source));
}
