/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 Pranav <<user@hostname.org>>
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
 * SECTION:element-dynamic
 *
 * FIXME:Describe dynamic here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! dynamic ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <libs/bytestream/bytestream.h>
#include "gstdynamic.h"

GST_DEBUG_CATEGORY_STATIC (gst_dynamic_debug);
#define GST_CAT_DEFAULT gst_dynamic_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src_%u",
    GST_PAD_SRC,
    GST_PAD_SOMETIMES,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_dynamic_parent_class parent_class
G_DEFINE_TYPE (GstDynamic, gst_dynamic, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (dynamic, "dynamic", GST_RANK_NONE,
    GST_TYPE_DYNAMIC);

static void gst_dynamic_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_dynamic_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_dynamic_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);
static GstFlowReturn gst_dynamic_chain (GstPad * pad,
    GstObject * parent, GstBuffer * buf);
static GstBuffer * gst_dynamic_getline (GstDynamic *filter);
static void gst_dynamic_loopfunc (GstElement *element);

/* GObject vmethod implementations */

/* initialize the dynamic's class */
static void
gst_dynamic_class_init (GstDynamicClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_dynamic_set_property;
  gobject_class->get_property = gst_dynamic_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "Dynamic",
      "FIXME:Generic",
      "FIXME:Generic Template Element", "Pranav <<user@hostname.org>>");

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
gst_dynamic_init (GstDynamic * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_dynamic_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_dynamic_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
  g_signal_connect(GST_ELEMENT(filter),"pad-added",G_CALLBACK(gst_dynamic_loopfunc),NULL);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_dynamic_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstDynamic *filter = GST_DYNAMIC (object);

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
gst_dynamic_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstDynamic *filter = GST_DYNAMIC (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_dynamic_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstDynamic *filter;
  gboolean ret;

  filter = GST_DYNAMIC (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

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
static GstBuffer * gst_dynamic_getline (GstDynamic *filter)
{
	guint8 *data;
	gint n, num;

	/* max. line length is 512 characters - for safety */
	for (n = 0; n < 512; n++) 
	{
		num = gst_bytestream_peek_bytes (filter->bs, &data, n + 1);
		if (num != n + 1)
			return NULL;

		/* newline? */
		if (data[n] == '\n') 
		{
			GstBuffer *buf = gst_buffer_new_allocate (NULL, n + 1, NULL);

			gst_bytestream_peek_bytes (filter->bs, &data, n);
			gst_buffer_fill (buf, 0, data, n);
			gst_buffer_memset (buf, n, '\0', 1);
			gst_bytestream_flush_fast (filter->bs, n + 1);

			return buf;
		}
	}
}

static void gst_dynamic_loopfunc (GstElement *element)
{
	GstDynamic *filter = GST_DYNAMIC (element);
	GstBuffer *buf;
	GstPad *pad;
	GstMapInfo map;
	gint num, n;

	/* parse header */
	if (filter->firstrun) 
	{
		gchar *padname;
		guint8 id;

		if (!(buf = gst_dynamic_getline (filter))) 
		{
			gst_element_error (element, STREAM, READ, (NULL),
					("Stream contains no header"));
			return;
		}
		gst_buffer_extract (buf, 0, &id, 1);
		num = atoi (id);
		gst_buffer_unref (buf);

		/* for each of the streams, create a pad */
		for (n = 0; n < num; n++) 
		{
			padname = g_strdup_printf ("src_%u", n);
			pad = gst_pad_new_from_static_template (&src_factory, padname);
			g_free (padname);

			/* here, you would set _event () and _query () functions */

			/* need to activate the pad before adding */
			gst_pad_set_active (pad, TRUE);

			gst_element_add_pad (element, pad);
			filter->srcpadlist = g_list_append (filter->srcpadlist, pad);
		}
	}

	/* and now, simply parse each line and push over */
	if (!(buf = gst_dynamic_getline (filter))) 
	{
		GstEvent *event = gst_event_new (GST_EVENT_EOS);
		GList *padlist;

		for (padlist = filter->srcpadlist; padlist != NULL; padlist = g_list_next (padlist)) 
		{
			pad = GST_PAD (padlist->data);
			gst_pad_push_event (pad, gst_event_ref (event));
		}
		gst_event_unref (event);
		/* pause the task here */
		return;
	}

	/* parse stream number and go beyond the ':' in the data */
	gst_buffer_map (buf, &map, GST_MAP_READ);
	num = atoi (map.data[0]);
	if (num >= 0 && num < g_list_length (filter->srcpadlist)) 
	{
		pad = GST_PAD (g_list_nth_data (filter->srcpadlist, num));

		/* magic buffer parsing foo */
		for (n = 0; map.data[n] != ':' &&
				map.data[n] != '\0'; n++) ;
		if (map.data[n] != '\0') 
		{
			GstBuffer *sub;

			/* create region copy that starts right past the space. The reason
			 * that we don't just forward the data pointer is because the
			 * pointer is no longer the start of an allocated block of memory,
			 * but just a pointer to a position somewhere in the middle of it.
			 * That cannot be freed upon disposal, so we'd either crash or have
			 * a memleak. Creating a region copy is a simple way to solve that. */
			sub = gst_buffer_copy_region (buf, GST_BUFFER_COPY_ALL,n + 1, map.size - n - 1);
			gst_pad_push (pad, sub);
		}
	}
	gst_buffer_unmap (buf, &map);
	gst_buffer_unref (buf);
}


/* chain function
 * this function does the actual processing
 */
	static GstFlowReturn
gst_dynamic_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstDynamic *filter;

	filter = GST_DYNAMIC (parent);

	if (filter->silent == FALSE)
		g_print ("I'm plugged, therefore I'm in.\n");

	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
	static gboolean
dynamic_init (GstPlugin * dynamic)
{
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template dynamic' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_dynamic_debug, "dynamic",
			0, "Template dynamic");

	return GST_ELEMENT_REGISTER (dynamic, dynamic);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstdynamic"
#endif

/* gstreamer looks for this structure to register dynamics
 *
 * exchange the string 'Template dynamic' with your dynamic description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		dynamic,
		"dynamic",
		dynamic_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
