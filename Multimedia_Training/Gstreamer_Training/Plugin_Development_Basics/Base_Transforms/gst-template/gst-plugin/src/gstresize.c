/*
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2023 Pranav <<user@hostname.org>>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-resize
 *
 * FIXME:Describe resize here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! resize ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/base.h>
#include <gst/controller/controller.h>

#include "gstresize.h"

GST_DEBUG_CATEGORY_STATIC (gst_resize_debug);
#define GST_CAT_DEFAULT gst_resize_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT,
};

/* the capabilities of the inputs and outputs.
 *
 * FIXME:describe the real formats here.
 */
static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_resize_parent_class parent_class
G_DEFINE_TYPE (GstResize, gst_resize, GST_TYPE_BASE_TRANSFORM);
GST_ELEMENT_REGISTER_DEFINE (resize, "resize", GST_RANK_NONE,
    GST_TYPE_RESIZE);

static void gst_resize_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_resize_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean gst_resize_sink_event (GstBaseTransform * trans,GstEvent * event);
static GstFlowReturn gst_resize_transform_ip (GstBaseTransform *
    base, GstBuffer * outbuf);
    
static gboolean gst_resize_set_caps (GstBaseTransform * trans,
          GstCaps * incaps,GstCaps * outcaps);

/* GObject vmethod implementations */

/* initialize the resize's class */
static void
gst_resize_class_init (GstResizeClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_resize_set_property;
  gobject_class->get_property = gst_resize_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  gst_element_class_set_details_simple (gstelement_class,
      "Resize",
      "Generic/Filter",
      "FIXME:Generic Template Filter", "Pranav <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_template));

  GST_BASE_TRANSFORM_CLASS (klass)->transform_ip =
      GST_DEBUG_FUNCPTR (gst_resize_transform_ip);
 
GST_BASE_TRANSFORM_CLASS (klass)->sink_event =
      GST_DEBUG_FUNCPTR (gst_resize_sink_event);
      
      GST_BASE_TRANSFORM_CLASS (klass)->set_caps =
      GST_DEBUG_FUNCPTR (gst_resize_set_caps);
      
  /* debug category for fltering log messages
   *
   * FIXME:exchange the string 'Template resize' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_resize_debug, "resize", 0,
      "Template resize");
}

/* initialize the new element
 * initialize instance structure
 */
static void
gst_resize_init (GstResize * filter)
{
  filter->silent = FALSE;
}

static void
gst_resize_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstResize *filter = GST_RESIZE (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_resize_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstResize *filter = GST_RESIZE (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}
/* GstBaeTransform set_caps function implementation */

static gboolean gst_resize_set_caps (GstBaseTransform * trans,
          GstCaps * incaps,GstCaps * outcaps)
{
	g_print("set_caps function was invoked\n");
	gchar *capsstr;
	
	capsstr = gst_caps_to_string(incaps);
	
	g_print("Capabilities: %s\n", capsstr);
	
	GstCaps *simp_caps  = gst_new_simple_caps("video/x-raw",
        "width", G_TYPE_INT, 1920,
        "height", G_TYPE_INT, 1080,
        NULL);
        g_print("hi\n");
        /*gst_caps_replace(&outcaps , simp_caps);
        
        gchar *outstr = gst_caps_to_string(outcaps);
        g_print("Capabilities of outcaps: %s\n", outstr);*/
	
	return TRUE;
}

/* GstBaseTransform vmethod implementations */
static gboolean gst_resize_sink_event (GstBaseTransform * trans,GstEvent * event)
{
GstResize *filter;
  gboolean ret;
GstPad *pad=GST_BASE_TRANSFORM_SINK_PAD (trans);
  filter = GST_RESIZE (trans);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;
      g_print("Caps event is called\n");
      gst_event_parse_caps (event, &caps);
      /* do something with the caps */
      GstCaps *customCaps = gst_caps_new_simple("video/x-raw",
      "width", G_TYPE_INT, 1280,
      "height", G_TYPE_INT, 720,
      NULL);
      
      /* and forward */
      ret = gst_pad_event_default (pad, (GstObject *)trans, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, (GstObject *)trans, event);
      break;
  }
  return ret;
 }
/* this function does the actual processing
 */
static GstFlowReturn
gst_resize_transform_ip (GstBaseTransform * base, GstBuffer * outbuf)
{
  GstResize *filter = GST_RESIZE (base);

  if (GST_CLOCK_TIME_IS_VALID (GST_BUFFER_TIMESTAMP (outbuf)))
    gst_object_sync_values (GST_OBJECT (filter), GST_BUFFER_TIMESTAMP (outbuf));

  if (filter->silent == FALSE)
    g_print ("I'm plugged, therefore I'm in.\n");
   GstPad *inputPad = GST_BASE_TRANSFORM_SINK_PAD (base);
   GstCaps *inputCaps = gst_pad_get_current_caps(inputPad);
   gchar *capsString = gst_caps_to_string(inputCaps);

    // Print the string representation
    g_print("Caps as string: %s\n", capsString);
    GstCaps *customCaps = gst_caps_new_simple("video/x-raw",
      "format", G_TYPE_STRING, "YV12",
      "width", G_TYPE_INT, 1280,
      "height", G_TYPE_INT, 720,
      NULL);

  /* Get the output pad of the element */
  GstPad *outputPad = GST_BASE_TRANSFORM_SRC_PAD(base);

  /* Set the custom caps on the output pad */
  gst_pad_set_caps(outputPad, customCaps);

  /* Don't forget to unref the custom caps when you're done with them */
  gst_caps_unref(customCaps);


  /* FIXME: do something interesting here.  This simply copies the source
   * to the destination. */

  return GST_FLOW_OK;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
resize_init (GstPlugin * resize)
{
  return GST_ELEMENT_REGISTER (resize, resize);
}

/* gstreamer looks for this structure to register resizes
 *
 * FIXME:exchange the string 'Template resize' with you resize description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    resize,
    "resize",
    resize_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
