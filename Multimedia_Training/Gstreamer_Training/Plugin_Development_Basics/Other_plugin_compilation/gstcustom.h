//Normal gstreamer headerfile
#include<gst/gst.h>

#ifndef __GST_CUSTOM_H__
#define __GST_CUSTOM_H__

#include <string.h>

G_BEGIN_DECLS

#define GST_TYPE_CUSTOM (gst_custom_get_type())
G_DECLARE_FINAL_TYPE (GstCustom, gst_custom,GST, CUSTOM, GstElement)

struct _GstCustom
{
	GstElement element;

	//src pad
	GstPad *srcpad;
	//sink pad
	GstPad *sinkpad;

	gboolean silent;


};

G_END_DECLS

#endif /* __GST_CUSTOM_H__ */
