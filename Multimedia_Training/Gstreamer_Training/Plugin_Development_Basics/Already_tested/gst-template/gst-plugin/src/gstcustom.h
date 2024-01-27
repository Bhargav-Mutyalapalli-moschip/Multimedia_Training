//Normal gstreamer headerfile
#include<gst/gst.h>

#ifndef __GST_CUSTOM_H__
#define __GST_CUSTOM_H__

#include <string.h>

G_BEGIN_DECLS

#define GST_TYPE_CUSTOM (gst_custom_get_type())
G_DECLARE_FINAL_TYPE (GstCustom, gst_custom,GST, CUSTOM, GstElement)

/*--------Graphical properties----------*/
// Define the enumeration for video test patterns
typedef enum {
  GST_CUSTOM_SMPTE,
  GST_CUSTOM_SNOW,
  GST_CUSTOM_BLACK
} GstCutomLayer;


struct _GstCustom
{
	GstElement element;

	//src pad
	GstPad *srcpad;
	//sink pad
	GstPad *sinkpad;

	gboolean silent;

        GstCustomLayer layer;
	char word[50];


};

G_END_DECLS

#endif /* __GST_CUSTOM_H__ */
