/*
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2023 Bhargav Mutyalapalli <<user@hostname.org>>
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
 * SECTION:element-linker
 *
 * FIXME:Describe linker here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! linker ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/base.h>
#include <gst/controller/controller.h>

#include "gstlinker.h"

GST_DEBUG_CATEGORY_STATIC (gst_linker_debug);
#define GST_CAT_DEFAULT gst_linker_debug

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

#define gst_linker_parent_class parent_class
G_DEFINE_TYPE (GstLinker, gst_linker, GST_TYPE_BASE_TRANSFORM);
GST_ELEMENT_REGISTER_DEFINE (linker, "linker", GST_RANK_NONE,
    GST_TYPE_LINKER);

#define GST_TYPE_LINKER_CAPS_CHANGE_MODE (gst_linker_caps_change_mode_get_type())
static GType gst_linker_caps_change_mode_get_type (void)
{
  static GType type = 0;
  static const GEnumValue data[] = {
    {GST_LINKER_CAPS_CHANGE_MODE_IMMEDIATE,
        "Only accept the current filter caps", "immediate"},
    {GST_LINKER_CAPS_CHANGE_MODE_DELAYED,
        "Temporarily accept previous filter caps", "delayed"},
    {0, NULL, NULL},
  };

  if (!type) {
    type = g_enum_register_static ("GstLinkerCapsChangeMode", data);
  }
  return type;
}

static void gst_linker_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_linker_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_linker_transform_ip (GstBaseTransform *
    base, GstBuffer * outbuf);

static gboolean gst_linker_sink_event (GstBaseTransform * trans, GstEvent * event);
static void gst_capsfilter_push_pending_events (GstLinker * filter, GList * events);
static GstFlowReturn gst_linker_prepare_buf (GstBaseTransform * trans, GstBuffer * input,GstBuffer ** buf);
static gboolean linker_accept_caps (GstBaseTransform * base,GstPadDirection direction, GstCaps * caps);
static GstCaps * gst_linker_transform_caps (GstBaseTransform * base,GstPadDirection direction, GstCaps * caps, GstCaps * filter);


/* GObject vmethod implementations */

/* initialize the linker's class */
static void
gst_linker_class_init (GstLinkerClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseTransformClass *trans_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_linker_set_property;
  gobject_class->get_property = gst_linker_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  gst_element_class_set_details_simple (gstelement_class,
      "Linker",
      "Generic/Filter",
      "FIXME:Generic Template Filter", "Bhargav Mutyalapalli <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_template));

  trans_class = GST_BASE_TRANSFORM_CLASS (klass);
  trans_class->transform_caps =GST_DEBUG_FUNCPTR (gst_linker_transform_caps);
  trans_class->transform_ip = GST_DEBUG_FUNCPTR (gst_linker_transform_ip);
  trans_class->accept_caps = GST_DEBUG_FUNCPTR (gst_linker_accept_caps);
  trans_class->prepare_output_buffer =GST_DEBUG_FUNCPTR (gst_linker_prepare_buf);
  trans_class->sink_event = GST_DEBUG_FUNCPTR (gst_linker_sink_event);
  gst_type_mark_as_plugin_api (GST_TYPE_LINKER_CAPS_CHANGE_MODE, 0);

  /* debug category for fltering log messages
   *
   * FIXME:exchange the string 'Template linker' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_linker_debug, "linker", 0,
      "Template linker");
}

/* initialize the new element
 * initialize instance structure
 */
static void
gst_linker_init (GstLinker * filter)
{
	GstBaseTransform *trans = GST_BASE_TRANSFORM (filter);
	gst_base_transform_set_gap_aware (element, TRUE);
	gst_base_transform_set_prefer_passthrough (element, FALSE);
	filter->silent = FALSE;
}

static void gst_linker_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	GstLinker *filter = GST_LINKER (object);

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void gst_linker_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
  GstLinker *filter = GST_LINKER (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstBaseTransform vmethod implementations */

/* this function does the actual processing
 */
static GstFlowReturn
gst_linker_transform_ip (GstBaseTransform * base, GstBuffer * outbuf)
{
  GstLinker *filter = GST_LINKER (base);

  if (GST_CLOCK_TIME_IS_VALID (GST_BUFFER_TIMESTAMP (outbuf)))
    gst_object_sync_values (GST_OBJECT (filter), GST_BUFFER_TIMESTAMP (outbuf));

  if (filter->silent == FALSE)
    g_print ("I'm plugged, therefore I'm in.\n");

  /* FIXME: do something interesting here.  This simply copies the source
   * to the destination. */

  return GST_FLOW_OK;
}
static GstCaps * gst_linker_transform_caps (GstBaseTransform * base,GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
  GstLinker *capsfilter = GST_LINKER (base);
  GstCaps *ret, *filter_caps, *tmp;
  gboolean retried = FALSE;
  GstCapsFilterCapsChangeMode caps_change_mode;
  
  GST_OBJECT_LOCK (capsfilter);

  GstCaps *new_caps=gst_caps_new_simple("video/x-raw","format",G_TYPE_STRING,"NV12","width",G_TYPE_INT,640,"height",G_TYPE_INT,480,NULL);
  filter_caps = gst_caps_ref (new_caps);
  capsfilter->filter_caps_used = TRUE;
  caps_change_mode = capsfilter->caps_change_mode;
  GST_OBJECT_UNLOCK (capsfilter);

retry:
  if (filter) {
    tmp =
        gst_caps_intersect_full (filter, filter_caps, GST_CAPS_INTERSECT_FIRST);
    gst_caps_unref (filter_caps);
    filter_caps = tmp;
  }

  ret = gst_caps_intersect_full (filter_caps, caps, GST_CAPS_INTERSECT_FIRST);

  GST_DEBUG_OBJECT (capsfilter, "input:     %" GST_PTR_FORMAT, caps);
  GST_DEBUG_OBJECT (capsfilter, "filter:    %" GST_PTR_FORMAT, filter);
  GST_DEBUG_OBJECT (capsfilter, "caps filter:    %" GST_PTR_FORMAT,
      filter_caps);
  GST_DEBUG_OBJECT (capsfilter, "intersect: %" GST_PTR_FORMAT, ret);

  if (gst_caps_is_empty (ret)
      && caps_change_mode ==
      GST_CAPS_FILTER_CAPS_CHANGE_MODE_DELAYED && capsfilter->previous_caps
      && !retried) {
    GList *l;

    GST_DEBUG_OBJECT (capsfilter,
        "Current filter caps are not compatible, retry with previous");
    GST_OBJECT_LOCK (capsfilter);
    gst_caps_unref (filter_caps);
    gst_caps_unref (ret);
    filter_caps = gst_caps_new_empty ();
    for (l = capsfilter->previous_caps; l; l = l->next) {
      filter_caps = gst_caps_merge (filter_caps, gst_caps_ref (l->data));
    }
    GST_OBJECT_UNLOCK (capsfilter);
    retried = TRUE;
    goto retry;
  }

  gst_caps_unref (filter_caps);

  return ret;
}
static gboolean linker_accept_caps (GstBaseTransform * base,GstPadDirection direction, GstCaps * caps)
{
  GstLinker *capsfilter = GST_LINKER (base);
  GstCaps *filter_caps;
  gboolean ret;

  GST_OBJECT_LOCK (capsfilter);
  GstCaps *new_caps=gst_caps_new_simple("video/x-raw","format",G_TYPE_STRING,"NV12","width",G_TYPE_INT,640,"height",G_TYPE_INT,480,NULL);
  filter_caps = gst_caps_ref (new_caps);
  capsfilter->filter_caps_used = TRUE;
  GST_OBJECT_UNLOCK (capsfilter);

  ret = gst_caps_can_intersect (caps, filter_caps);
  GST_DEBUG_OBJECT (capsfilter, "can intersect: %d", ret);
  if (!ret
      && capsfilter->caps_change_mode ==
      GST_CAPS_FILTER_CAPS_CHANGE_MODE_DELAYED) {
    GList *l;

    GST_OBJECT_LOCK (capsfilter);
    for (l = capsfilter->previous_caps; l; l = l->next) {
      ret = gst_caps_can_intersect (caps, l->data);
      if (ret)
        break;
    }
    GST_OBJECT_UNLOCK (capsfilter);

    /* Tell upstream to reconfigure, it's still
     * looking at old caps */
    if (ret)
      gst_base_transform_reconfigure_sink (base);
  }

  gst_caps_unref (filter_caps);

  return ret;
}
static GstFlowReturn gst_linker_prepare_buf (GstBaseTransform * trans, GstBuffer * input,GstBuffer ** buf)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstLinker *filter = GST_LINKER (trans);

  /* always return the input as output buffer */
  *buf = input;

  if (GST_PAD_MODE (trans->srcpad) == GST_PAD_MODE_PUSH
      && !filter->got_sink_caps) {

    /* No caps. See if the output pad only supports fixed caps */
    GstCaps *out_caps;
    GList *pending_events = filter->pending_events;

    GST_LOG_OBJECT (trans, "Input pad does not have caps");

    filter->pending_events = NULL;

    out_caps = gst_pad_get_current_caps (trans->srcpad);
    if (out_caps == NULL) {
      out_caps = gst_pad_get_allowed_caps (trans->srcpad);
      g_return_val_if_fail (out_caps != NULL, GST_FLOW_ERROR);
    }

    out_caps = gst_caps_simplify (out_caps);

    if (gst_caps_is_fixed (out_caps) && !gst_caps_is_empty (out_caps)) {
      GST_DEBUG_OBJECT (trans, "Have fixed output caps %"
          GST_PTR_FORMAT " to apply to srcpad", out_caps);

      if (!gst_pad_has_current_caps (trans->srcpad)) {
        if (gst_pad_set_caps (trans->srcpad, out_caps)) {
          if (pending_events) {
            gst_capsfilter_push_pending_events (filter, pending_events);
            pending_events = NULL;
          }
        } else {
          ret = GST_FLOW_NOT_NEGOTIATED;
        }
      } else {
        gst_capsfilter_push_pending_events (filter, pending_events);
        pending_events = NULL;
      }

      g_list_free_full (pending_events, (GDestroyNotify) gst_event_unref);
      gst_caps_unref (out_caps);
    } else {
      gchar *caps_str = gst_caps_to_string (out_caps);

      GST_DEBUG_OBJECT (trans, "Cannot choose caps. Have unfixed output caps %"
          GST_PTR_FORMAT, out_caps);
      gst_caps_unref (out_caps);

      GST_ELEMENT_ERROR (trans, STREAM, FORMAT,
          ("Filter caps do not completely specify the output format"),
          ("Output caps are unfixed: %s", caps_str));

      g_free (caps_str);
      g_list_free_full (pending_events, (GDestroyNotify) gst_event_unref);

      ret = GST_FLOW_ERROR;
    }
  } else if (G_UNLIKELY (filter->pending_events)) {
    GList *events = filter->pending_events;

    filter->pending_events = NULL;

    /* push pending events before a buffer */
    gst_capsfilter_push_pending_events (filter, events);
  }

  return ret;
}
static void gst_capsfilter_push_pending_events (GstLinker * filter, GList * events)
{
  GList *l;

  for (l = g_list_last (events); l; l = l->prev) {
    GST_LOG_OBJECT (filter, "Forwarding %s event",
        GST_EVENT_TYPE_NAME (l->data));
    GST_BASE_TRANSFORM_CLASS (parent_class)->sink_event (GST_BASE_TRANSFORM_CAST
        (filter), l->data);
  }
  g_list_free (events);
}


static gboolean gst_linker_sink_event (GstBaseTransform * trans, GstEvent * event)
{
  GstLinker *filter = GST_LINKER (trans);
  gboolean ret;

  if (GST_EVENT_TYPE (event) == GST_EVENT_FLUSH_STOP) {
    GList *l;

    for (l = filter->pending_events; l; l = l->next) {
      if (GST_EVENT_TYPE (l->data) == GST_EVENT_SEGMENT ||
          GST_EVENT_TYPE (l->data) == GST_EVENT_EOS) {
        gst_event_unref (l->data);
        filter->pending_events = g_list_delete_link (filter->pending_events, l);
        break;
      }
    }
  }

  if (!GST_EVENT_IS_STICKY (event)
      || GST_EVENT_TYPE (event) <= GST_EVENT_CAPS)
    goto done;

  /* If we get EOS before any buffers, just push all pending events */
  if (GST_EVENT_TYPE (event) == GST_EVENT_EOS) {
    GList *l;

    for (l = g_list_last (filter->pending_events); l; l = l->prev) {
      GST_LOG_OBJECT (trans, "Forwarding %s event",
          GST_EVENT_TYPE_NAME (l->data));
      GST_BASE_TRANSFORM_CLASS (parent_class)->sink_event (trans, l->data);
    }
    g_list_free (filter->pending_events);
    filter->pending_events = NULL;
  } else if (!filter->got_sink_caps) {
    GST_LOG_OBJECT (trans, "Got %s event before caps, queueing",
        GST_EVENT_TYPE_NAME (event));

    filter->pending_events = g_list_prepend (filter->pending_events, event);

    return TRUE;
  }

done:

  GST_LOG_OBJECT (trans, "Forwarding %s event", GST_EVENT_TYPE_NAME (event));
  ret =
      GST_BASE_TRANSFORM_CLASS (parent_class)->sink_event (trans,
      gst_event_ref (event));

  if (GST_EVENT_TYPE (event) == GST_EVENT_CAPS) {
    filter->got_sink_caps = TRUE;
    if (filter->caps_change_mode == GST_CAPS_FILTER_CAPS_CHANGE_MODE_DELAYED) {
      GList *l;
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);

      /* Remove all previous caps up to one that works.
       * Note that this might keep some leftover caps if there
       * are multiple compatible caps */
      GST_OBJECT_LOCK (filter);
      for (l = g_list_last (filter->previous_caps); l; l = l->prev) {
        if (gst_caps_can_intersect (caps, l->data)) {
          while (l->next) {
            gst_caps_unref (l->next->data);
            l = g_list_delete_link (l, l->next);
          }
          break;
        }
      }
      if (!l && gst_caps_can_intersect (caps, filter->filter_caps)) {
        g_list_free_full (filter->previous_caps,
            (GDestroyNotify) gst_caps_unref);
        filter->previous_caps = NULL;
        filter->filter_caps_used = TRUE;
      }
      GST_OBJECT_UNLOCK (filter);
    }
  }
  gst_event_unref (event);

  return ret;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
linker_init (GstPlugin * linker)
{
  return GST_ELEMENT_REGISTER (linker, linker);
}

/* gstreamer looks for this structure to register linkers
 *
 * FIXME:exchange the string 'Template linker' with you linker description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    linker,
    "linker",
    linker_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
