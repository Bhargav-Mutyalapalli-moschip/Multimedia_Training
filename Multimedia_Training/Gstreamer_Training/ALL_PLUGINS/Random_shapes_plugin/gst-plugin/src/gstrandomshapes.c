/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 Rakesh Konathala <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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
 * SECTION:element-randomshapes
 *
 * FIXME:Describe randomshapes here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! randomshapes ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video.h>

#include "gstrandomshapes.h"

GST_DEBUG_CATEGORY_STATIC (gst_randomshapes_debug);
#define GST_CAT_DEFAULT gst_randomshapes_debug

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
  PROP_XCO,
  PROP_YCO,
  PROP_SHAPE,
  PROP_COLOUR
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (/*"video/x-raw,format=NV12,width=720,height=480,framerate=24/1"*/"ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_randomshapes_parent_class parent_class
G_DEFINE_TYPE (GstRandomshapes, gst_randomshapes, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (randomshapes, "randomshapes", GST_RANK_NONE,
    GST_TYPE_RANDOMSHAPES);

static void gst_randomshapes_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_randomshapes_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_randomshapes_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);
static GstFlowReturn gst_randomshapes_chain (GstPad * pad,
    GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the randomshapes's class */
static void
gst_randomshapes_class_init (GstRandomshapesClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_randomshapes_set_property;
  gobject_class->get_property = gst_randomshapes_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_XCO,
      g_param_spec_int ("xco", "X-co", "required point on x-axis",
          0,200,150, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_YCO,
      g_param_spec_int ("yco", "Y-co", "required point on y-axis",
          0,200,150, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_SHAPE,
      g_param_spec_string ("shape", "Shape", "required shape",
          "square", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_COLOUR,
      g_param_spec_string ("colour", "Colour", "required colour",
          "red", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gst_element_class_set_details_simple (gstelement_class,
      "Randomshapes",
      "FIXME:Generic",
      "FIXME:Generic Template Element", "Rakesh Konathala <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void
gst_randomshapes_init (GstRandomshapes * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_randomshapes_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_randomshapes_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
  filter->xco= 150;
  filter->yco= 150;
  strcpy(filter->shape,"square");
  strcpy(filter->colour,"red");
}

static void
gst_randomshapes_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstRandomshapes *filter = GST_RANDOMSHAPES (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    case PROP_XCO:
      filter->xco = g_value_get_int (value);
      break;
    case PROP_YCO:
      filter->yco = g_value_get_int (value);
      break;
    case PROP_SHAPE:
      strcpy(filter->shape,g_value_get_string(value));
      break;
    case PROP_COLOUR:
      strcpy(filter->colour,g_value_get_string(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_randomshapes_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstRandomshapes *filter = GST_RANDOMSHAPES (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    case PROP_XCO:
      g_value_set_int (value ,filter->xco);
      break;
    case PROP_YCO:
      g_value_set_int (value ,filter->yco);
      break;
    case PROP_SHAPE:
      g_value_set_string(value,filter->shape);
      break;
    case PROP_COLOUR:
      g_value_set_string(value,filter->colour);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_randomshapes_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstRandomshapes *filter;
  gboolean ret;

  filter = GST_RANDOMSHAPES (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */
      gchar *caps_str = gst_caps_to_string(caps);
      g_print("Received caps: %s\n", caps_str);
      g_free(caps_str); //deallocate the memory for caps_str

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
    ret = gst_pad_event_default (pad, parent, event);
    break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
	static GstFlowReturn
gst_randomshapes_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstRandomshapes *filter;
	GstMapInfo map;
	GstVideoInfo video_info;
	GstVideoFrame vframe;

	guint *ptr;
	gint i,j,l=50,b=50,c=0;
	guint buf_size = gst_buffer_get_size(buf);
	g_print("size of buffr is %u",buf_size);
	buf=gst_buffer_make_writable(buf);

	filter = GST_RANDOMSHAPES (parent);
	//g_print("\nxcordinate given is %d\n",filter->xco);
	//g_print("ycoordinate given is %d\n",filter->yco);
	g_print("the given shape is %s\n",filter->shape);
	g_print("the given color is %s\n",filter->colour);

	if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) &&gst_video_frame_map(&vframe, &video_info, buf, GST_MAP_WRITE)) 
	{
		guint8 *pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0);
		guint stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0);
		guint pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe, 0);

		gint width = video_info.width;
		g_print("The width:%d\n",width);
		gint height = video_info.height;
		g_print("The height:%d\n",height);

		guint h, w;
		for (h = 0; h < height; ++h) 
		{
			for (w = 0; w < width; ++w)
			{
				guint8 *pixel = pixels + h * stride + w * pixel_stride;

				// Example: Set pixel values to red (assuming RGB format)
				/*pixel[0] = 0; // Red
				  pixel[1] = 0;   // Green
				  pixel[2] = 0;   // Blue*/
				pixel[w]=0

			}
		}

		gst_video_frame_unmap(&vframe);
	}


	/*if(gst_buffer_map(buf,&map,GST_MAP_WRITE))
	  {
	  g_print("\nxcordinate given is %d\n",filter->xco);
	  g_print("ycoordinate given is %d\n",filter->yco);

	  ptr=(guint *)map.data;
	  for(j=0;j<240;j++)
	  {
	  for(i=0;i<320;i++)
	  {
	  if(((filter->xco==i) && (filter->yco==j)) && (c<l))
	  {

	  ptr[j]==0xf800;
	  c++;
	  }
	//			ptr[j]=0xf800;
	}
	ptr += 240;
	} 
	gst_buffer_unmap(buf,&map);*/

	if (filter->silent == FALSE)
		g_print ("___________plugin coneected ________\n");

	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
	static gboolean
randomshapes_init (GstPlugin * randomshapes)
{
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template randomshapes' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_randomshapes_debug, "randomshapes",
			0, "Template randomshapes");

	return GST_ELEMENT_REGISTER (randomshapes, randomshapes);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstrandomshapes"
#endif

/* gstreamer looks for this structure to register randomshapess
 *
 * exchange the string 'Template randomshapes' with your randomshapes description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		randomshapes,
		"randomshapes",
		randomshapes_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
