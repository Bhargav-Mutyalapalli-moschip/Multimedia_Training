//Required header files
#include<gst/gst.h>

//own structure
struct __gstpranav
{
	//required pads
	Gstpad *srcpad;
	Gstpad *sinkpad;

	//required properties
	gboolean silent;
	gint layer;
};

