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
 * SECTION:element-cropmeth
 *
 * FIXME:Describe cropmeth here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! cropmeth ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstcropmeth.h"

GST_DEBUG_CATEGORY_STATIC (gst_cropmeth_debug);
#define GST_CAT_DEFAULT gst_cropmeth_debug

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
  PROP_LEARN
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

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_cropmeth_parent_class parent_class 
G_DEFINE_TYPE (GstCropmeth, gst_cropmeth, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (cropmeth, "cropmeth", GST_RANK_NONE,
    GST_TYPE_CROPMETH);

static void gst_cropmeth_set_property (GObject * object,guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_cropmeth_get_property (GObject * object,guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean gst_cropmeth_sink_event (GstPad * pad,GstObject * parent, GstEvent * event);
static GstFlowReturn gst_cropmeth_chain (GstPad * pad,GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the cropmeth's class */
static void gst_cropmeth_class_init (GstCropmethClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_cropmeth_set_property;
  gobject_class->get_property = gst_cropmeth_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",FALSE, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_LEARN,g_param_spec_int ("learn", "learn", "It is just for checking ? ",0,10,0, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "Cropmeth",
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
static void gst_cropmeth_init (GstCropmeth * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_cropmeth_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_cropmeth_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void gst_cropmeth_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
  GstCropmeth *filter = GST_CROPMETH (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    case PROP_LEARN:
      filter->learn = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void gst_cropmeth_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
  GstCropmeth *filter = GST_CROPMETH (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    case PROP_LEARN:
      g_value_set_int (value, filter->learn);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_cropmeth_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
  GstCropmeth *filter;
  gboolean ret;

  filter = GST_CROPMETH (parent);

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

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_cropmeth_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstCropmeth *filter;

  filter = GST_CROPMETH (parent);

  if (filter->silent == FALSE)
    g_print ("I'm plugged, therefore I'm in.\n");

  /* just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean cropmeth_init (GstPlugin * cropmeth)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template cropmeth' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_cropmeth_debug, "cropmeth",0, "Template cropmeth");

  return GST_ELEMENT_REGISTER (cropmeth, cropmeth);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstcropmeth"
#endif

/* gstreamer looks for this structure to register cropmeths
 *
 * exchange the string 'Template cropmeth' with your cropmeth description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    cropmeth,
    "cropmeth",
    cropmeth_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
