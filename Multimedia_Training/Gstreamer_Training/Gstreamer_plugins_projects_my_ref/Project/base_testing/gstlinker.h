/* 
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2020 Niels De Graef <niels.degraef@gmail.com>
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
 
#ifndef __GST_LINKER_H__
#define __GST_LINKER_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_LINKER (gst_linker_get_type())
G_DECLARE_FINAL_TYPE (GstLinker, gst_linker,
    GST, LINKER, GstBaseTransform)

struct _GstLinker {
  GstBaseTransform element;
  GstCaps *filter_caps;
  gboolean filter_caps_used;
  GstCapsFilterCapsChangeMode caps_change_mode;
  gboolean got_sink_caps;
  GList *pending_events;
  GList *previous_caps;

  gboolean silent;
};
typedef enum {
  GST_LINKER_CAPS_CHANGE_MODE_IMMEDIATE,
  GST_LINKER_CAPS_CHANGE_MODE_DELAYED
} GstLinkerCapsChangeMode;

/**
 * GstCapsFilter:
 *
 * The opaque #GstCapsFilter data structure.
 */
struct _GstLinkerClass {
  GstBaseTransformClass trans_class;
};

G_END_DECLS

#endif /* __GST_LINKER_H__ */
