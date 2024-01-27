/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 Balaji Gudala <<user@hostname.org>>
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
 * SECTION:element-drawshapes
 *
 * FIXME:Describe drawshapes here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! drawshapes ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include<gst/video/video.h>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <libs/bytestream/bytestream.h>



#include "gstdrawshapes.h"

GST_DEBUG_CATEGORY_STATIC (gst_drawshapes_debug);
#define GST_CAT_DEFAULT gst_drawshapes_debug

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
	PROP_COLOUR,
	PROP_LENGTH,
	PROP_BREADTH,
	PROP_RADIUS

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

#define gst_drawshapes_parent_class parent_class
G_DEFINE_TYPE (GstDrawshapes, gst_drawshapes, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (drawshapes, "drawshapes", GST_RANK_NONE,
		GST_TYPE_DRAWSHAPES);

static void gst_drawshapes_set_property (GObject * object,
		guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_drawshapes_get_property (GObject * object,
		guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_drawshapes_sink_event (GstPad * pad,
		GstObject * parent, GstEvent * event);
static GstFlowReturn gst_drawshapes_chain (GstPad * pad,
		GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the drawshapes's class */
	static void
gst_drawshapes_class_init (GstDrawshapesClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_drawshapes_set_property;
	gobject_class->get_property = gst_drawshapes_get_property;

	g_object_class_install_property (gobject_class, PROP_SILENT,
			g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
				FALSE, G_PARAM_READWRITE));



	g_object_class_install_property (gobject_class, PROP_XCO,
			g_param_spec_int ("xco", "X-co", "required point on x-axis",
				0,720,360, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_YCO,
			g_param_spec_int ("yco", "Y-co", "required point on y-axis",
				0,480,240, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SHAPE,
			g_param_spec_string ("shape", "Shape", "required shape",
				"square", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_COLOUR,
			g_param_spec_string ("colour", "Colour", "required colour",
				"red", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_LENGTH,
			g_param_spec_int ("length", "Length", "length of shape",
				0,450,100, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_BREADTH,
			g_param_spec_int ("breadth", "Breadth", "breadth of shape",
				0,700,200, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_RADIUS,
			g_param_spec_int ("radius", "Radius", "radius of circle to draw on plane",
				0,700,400, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));







	gst_element_class_set_details_simple (gstelement_class,
			"Drawshapes",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Balaji Gudala <<user@hostname.org>>");

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
gst_drawshapes_init (GstDrawshapes * filter)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_drawshapes_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_drawshapes_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	filter->silent = FALSE;
	filter->xco= 360;
	filter->length= 100;
	filter->breadth= 200;
	filter->yco= 240;
	filter->radius=400;
	strcpy(filter->shape,"square");
	strcpy(filter->colour,"red");

}

	static void
gst_drawshapes_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec)
{
	GstDrawshapes *filter = GST_DRAWSHAPES (object);

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
		case PROP_LENGTH:
			filter->length = g_value_get_int (value);
			break;
		case PROP_BREADTH:
			filter->breadth = g_value_get_int (value);
			break;
		case PROP_RADIUS:
			filter->radius = g_value_get_int (value);
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
gst_drawshapes_get_property (GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec)
{
	GstDrawshapes *filter = GST_DRAWSHAPES (object);

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
		case PROP_LENGTH:
			g_value_set_int (value ,filter->length);
			break;
		case PROP_BREADTH:
			g_value_set_int (value ,filter->breadth);
			break;
		case PROP_RADIUS:
			g_value_set_int (value ,filter->radius);
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
gst_drawshapes_sink_event (GstPad * pad, GstObject * parent,
		GstEvent * event)
{
	GstDrawshapes *filter;
	gboolean ret;

	filter = GST_DRAWSHAPES (parent);

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
	static GstFlowReturn
gst_drawshapes_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstDrawshapes *filter;
	GstVideoInfo video_info;
	GstVideoFrame vframe;

	filter = GST_DRAWSHAPES (parent);
	//printing all the requirements for debugging
	g_print("\nxcordinate given is %d\n",filter->xco);
	g_print("ycoordinate given is %d\n",filter->yco);
	g_print("lenth =%d  breadth=%d\n",filter->length,filter->breadth);
	g_print("the given shape is %s\n",filter->shape);
	g_print("the given color is %s\n",filter->colour);
	g_print("the radius given is %d\n",filter->radius);


	buf=gst_buffer_make_writable(buf);
	if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) && gst_video_frame_map(&vframe, &video_info, buf, GST_MAP_WRITE)) 
	{
		guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
		guint8 *uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)

		guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
		guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride
		guint pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

		gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
		gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);
		gint framesize=GST_VIDEO_FRAME_SIZE(&vframe);
		g_print("width=%d height=%d framesize=%d\n",width,height,framesize);
		gint y=0,u=0,v=0;
		if(!(strcmp("red",filter->colour)))
		{
			y=63;
			u=102;
			v=240;
		}
		if(!(strcmp("green",filter->colour)))
		{
			y=173;
			u=42;
			v=26;
		}
		if(!(strcmp("blue",filter->colour)))
		{
			y=32;
			u=240;
			v=118;
		}
		if( !(strcmp("square",filter->shape)))
		{
			// Define the coordinates and size of the square
			gint square_center_x =filter->xco ;
			gint square_center_y = filter->yco;
			gint square_size = filter->length;

			// Calculate the coordinates of the top-left and bottom-right corners
			gint square_top_left_x = square_center_x - square_size / 2;
			gint square_top_left_y = square_center_y - square_size / 2;
			gint square_bottom_right_x = square_center_x + square_size / 2;
			gint square_bottom_right_y = square_center_y + square_size / 2;

			// Loop through the entire frame
			guint h, w;
			for (h = 0; h < height; ++h) {
				for (w = 0; w < width; ++w) {
					guint8 *y_pixel = y_pixels + h * y_stride + w;
					guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;


					// Check if the pixel is on the border of the square
					if (((w == square_top_left_x || w == square_bottom_right_x) &&
								(h >= square_top_left_y && h <= square_bottom_right_y)) ||
							((h == square_top_left_y || h == square_bottom_right_y) &&
							 (w >= square_top_left_x && w <= square_bottom_right_x))) {
						// Set Y, U, and V values to white
						y_pixel[0] = y; // Y component
						uv_pixel[0] = u; // U component (or the first byte of UV)
						uv_pixel[1] = v; // V component (or the second byte of UV)
					}
				}
				}
			}
			if( !(strcmp("rectangle",filter->shape)))
			{
				// Define the coordinates and size of the square
				gint square_center_x =filter->xco ;
				gint square_center_y = filter->yco;
				gint shape_length= filter->length;
				gint shape_breadth=filter->breadth;
				g_print("rec l=%d  rec br=%d",shape_length,shape_breadth);

				// Calculate the coordinates of the top-left and bottom-right corners
				gint square_top_left_x = square_center_x - shape_breadth / 2;
				gint square_top_left_y = square_center_y - shape_length/ 2;
				gint square_bottom_right_x = square_center_x + shape_breadth / 2;
				gint square_bottom_right_y = square_center_y + shape_length / 2;

				// Loop through the entire frame
				guint h, w;
				for (h = 0; h < height; ++h) {
					for (w = 0; w < width; ++w) {
						guint8 *y_pixel = y_pixels + h * y_stride + w;
						guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;


						// Check if the pixel is on the border of the shape
						if ((((w == square_top_left_x || w == square_bottom_right_x) && (h >= square_top_left_y && h <= square_bottom_right_y))) ||
								(((h == square_top_left_y || h == square_bottom_right_y) && (w >= square_top_left_x && w <= square_bottom_right_x))))
						{
							// Set Y, U, and V values to white
							y_pixel[0] =y; // Y component
							uv_pixel[0] =u; // U component (or the first byte of UV)
							uv_pixel[1] =v; // V component (or the second byte of UV)
						}
					}
				}
			}

			if(!(strcmp("triangle",filter->shape)))
			{
				//define coordinates and size of traingle
				gint triangle_center_x=filter->xco;
				gint triangle_center_y=filter->yco;
				gint triangle_h=filter->length;
				gint triangle_bas=filter->breadth;
				g_print("height of triangle=%d\n",filter->length);
				g_print("base of triangle=%d\n",filter->breadth);

				int x1= triangle_center_x;
				int y1= triangle_center_y - (triangle_h / 2);
				int x2= triangle_center_x - (triangle_bas / 2);
				int y2= triangle_center_y + (triangle_h / 2);
				int x3= triangle_center_x + (triangle_bas / 2);
				int y3= triangle_center_y + (triangle_h / 2);
				g_print("x1=%d y1=%d\nx2=%d y2=%d\nx3=%d y3=%d \n",x1,y1,x2,y2,x3,y3);
				//Calculate the area of the triangle
				gint triangle_area = abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2);
				//loop through entire frame
				guint h,w;
				for (h = 0; h < height; ++h)
				{
					for (w = 0; w < width; ++w) 
					{
						guint8 *y_pixel = y_pixels + h * y_stride + w;
						guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;

						gint sub_area1 = abs((w * (y1 - y2) + x1 * (y2 - h) + x2 * (h - y1)) / 2);
						gint sub_area2 = abs((w * (y2 - y3) + x2 * (y3 - h) + x3 * (h - y2)) / 2);
						gint sub_area3 = abs((w * (y3 - y1) + x3 * (y1 - h) + x1 * (h - y3)) / 2);
						// Check if the pixel is on the border of the triangle

						if ( ((sub_area1 == 0 || sub_area2 == 0 || sub_area3 == 0) && (sub_area1 + sub_area2 + sub_area3 != triangle_area))  && ((h<=y2 && h>=y1 ) && ((w>=x2) && (w<=x3) )))
						{

							y_pixel[0] = y; // Y component
							uv_pixel[0] =u; // U component (or the first byte of UV)
							uv_pixel[1] =v; // V component (or the second byte of UV)
						}
					}
				}
			}
			if(!(strcmp("circle",filter->shape)))
			{
				//define coordinates and size of traingle
				//gint circle_center_x=filter->xco;
				//gint circle_center_y=filter->yco;
				//gint radius=filter->radius;
				//g_print("radius is %d center (%d %d) ",radius,circle_center_x,circle_center_y);

				//loop through entire frame
				//double h,w;
				//double x1=circle_center_x,y1=circle_center_y,d1,d;//,d2,d3,d4=0;
				/*for (h = 0; h < height; h++)
				{
					for (w = 0; w < width; w++)
					{
						guint8 *y_pixel = y_pixels + (guint8)h * y_stride + (guint)w;
						guint8 *uv_pixel = uv_pixels + (guint8)h / 2 * uv_stride + ((guint8)w / 2) * pixel_stride;
						//if((x1>w) && (y1>h))
						d1=abs(((x1-w) * (x1-w)) + ((y1-h) * (y1-h)));
						d=sqrt(d1);
						d2=((w-x1) * (w-x1)) + ((h-y1) * (h-y1));
						d3=((x1-w) * (x1-w)) + ((h-y1) * (h-y1));
						d4=((w-x1) * (w-x1)) + ((y1-h) * (y1-h));

						if((d1*d1) == (radius * radius)|| (d2*d2) == (radius * radius) || (d3*d3) == (radius * radius)|| (d4*d4) == (radius * radius))
						if (abs(((d) - (radius)) <= 1)) 
						
						{
							// Set Y, U, and V values
							y_pixel[0] = y; // Y component
							uv_pixel[0] =u; // U component (or the first byte of UV)
							uv_pixel[1] =v; // V component (or the second byte of UV)
						}
					}
				}*/
				gint h,w;
				gint circle_center_x = 100;
                                gint circle_center_y = 100;
                                gint circle_radius = 50;
                                gint border_thickness = 2; // Adjust the thickness of the circle border
				for (h = 0; h < height; ++h) 
				{
					for (w = 0; w < width; ++w) 
					{
						guint8 *y_pixel = y_pixels + h * y_stride + w;
						guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;

						// Calculate the distance between the current pixel and the circle's center
						gint dx = w - circle_center_x;
						gint dy = h - circle_center_y;
						double distance = sqrt(dx * dx + dy * dy);

						// Check if the pixel is on the border of the circle
						if (fabs(distance - circle_radius) <= border_thickness / 2) {
							// Set Y, U, and V values to white
							y_pixel[0] = 0; // Y component
							uv_pixel[0] = 128; // U component (or the first byte of UV)
							uv_pixel[1] = 128; // V component (or the second byte of UV)
						}
					}
				}

			}
			gst_video_frame_unmap(&vframe);
	}


	if (filter->silent == FALSE)
		g_print ("\n<<<<<<<<<<-------------------plugin connected--------------------->>>>>>>>>>>--\n");

	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);


}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean drawshapes_init (GstPlugin * drawshapes)
{
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template drawshapes' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_drawshapes_debug, "drawshapes",
			0, "Template drawshapes");

	return GST_ELEMENT_REGISTER (drawshapes, drawshapes);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstdrawshapes"
#endif

/* gstreamer looks for this structure to register drawshapess
 *
 * exchange the string 'Template drawshapes' with your drawshapes description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		drawshapes,
		"drawshapes",
		drawshapes_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
