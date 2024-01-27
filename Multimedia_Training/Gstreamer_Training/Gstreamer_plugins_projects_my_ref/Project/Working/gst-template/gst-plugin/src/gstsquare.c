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
 * SECTION:element-square
 *
 * FIXME:Describe square here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! square ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <string.h>
#include <gst/tag/tag.h>
#include "gstsquare.h"

GST_DEBUG_CATEGORY_STATIC (gst_square_debug);
#define GST_CAT_DEFAULT gst_square_debug

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
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_FORMAT,

};
#define GST_TAG_CUSTOM_NAME "format details"
/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		 GST_STATIC_CAPS("video/x-raw, "
                                         "width = (int) [1, 2147483647], "
                                         "height = (int) [1, 2147483647], "
                                         "framerate = (fraction) [0/1, 2147483647/1], "
                                         "format = (string) { I420, NV12, NV21, YV12,RGB16,NV16,NV24,\
					 RGBx, xRGB, BGRx, xBGR, RGBA, ARGB,BGRA, ABGR, RGB, BGR, AYUV, YUY2, \
					 YVYU, UYVY, I420, YV12, RGB16, RGB15, GRAY8, NV12, NV21,GRAY16_LE, GRAY16_BE }"));


static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		 GST_STATIC_CAPS("video/x-raw, "
                                         "width = (int) [1, 2147483647], "
                                         "height = (int) [1, 2147483647], "
                                         "framerate = (fraction) [0/1, 2147483647/1], "
                                         "format = (string) { I420, NV12, NV21, YV12,RGB16,NV16,NV24, \
					 RGBx, xRGB, BGRx, xBGR, RGBA, ARGB,BGRA, ABGR, RGB, BGR, AYUV, YUY2, \
					 YVYU, UYVY, I420, YV12, RGB16, RGB15, GRAY8, NV12, NV21,GRAY16_LE, GRAY16_BE }"));

/*-----------Other details---------------------*/
/*---------------------------------------------*/
#define gst_square_parent_class parent_class
G_DEFINE_TYPE (GstSquare, gst_square, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (square, "square", GST_RANK_NONE,
		GST_TYPE_SQUARE);

static void gst_square_set_property (GObject * object,
		guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_square_get_property (GObject * object,
		guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_square_sink_event (GstPad * pad,
		GstObject * parent, GstEvent * event);
static GstFlowReturn gst_square_chain (GstPad * pad,
		GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the square's class */
static void gst_square_class_init (GstSquareClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_square_set_property;
	gobject_class->get_property = gst_square_get_property;

	/*-------------------------Install the property with specified default values and permissions--------------------------*/
	/*------------------------------------------------------------------------------------------------------------------------------------*/
	g_object_class_install_property (gobject_class, PROP_SILENT,g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_XCO,g_param_spec_int ("xco", "xco", "X-Co-ordinate for the crop ",0,1920,100, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_YCO,g_param_spec_int ("yco", "yco", "Y-Co-ordinate for the crop ",0,1080,100, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_WIDTH,g_param_spec_int ("width", "width", "Width for display output",0,2147483647,640, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_HEIGHT,g_param_spec_int ("height", "height", "Height for display output",0,2147483647,480, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_FORMAT,g_param_spec_string ("format", "format", "format for display output","NV12", G_PARAM_READWRITE));

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	gst_element_class_set_details_simple (gstelement_class,
			"Square",
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
static void gst_square_init (GstSquare * filter)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_square_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_square_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	filter->silent = FALSE;
	filter->xco = 100;
	filter->yco = 100;
	filter->width = 640;
	filter->height = 480;
	strcpy(filter->format,"NV12");
}

static void gst_square_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
	GstSquare *filter = GST_SQUARE (object);

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
		case PROP_WIDTH:
			filter->width = g_value_get_int (value);
			break;
		case PROP_HEIGHT:
			filter->height = g_value_get_int (value);
			break;
		case PROP_FORMAT:
			strcpy(filter->format, g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void gst_square_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
	GstSquare *filter = GST_SQUARE (object);

	switch (prop_id) {
		case PROP_SILENT:
			g_value_set_boolean (value, filter->silent);
			break;
		case PROP_XCO:
			g_value_set_int (value, filter->xco);
			break;
		case PROP_YCO:
			g_value_set_int (value, filter->yco);
			break;
		case PROP_WIDTH:
			g_value_set_int (value, filter->width);
			break;
		case PROP_HEIGHT:
			g_value_set_int (value, filter->height);
			break;
		case PROP_FORMAT:
			g_value_set_string (value, filter->format);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_square_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
	GstSquare *filter;
	gboolean ret;

	filter = GST_SQUARE (parent);

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
static GstFlowReturn gst_square_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstSquare *filter;

	filter = GST_SQUARE (parent);

	if (filter->silent == FALSE)
		g_print ("I'm plugged, therefore I'm in.\n");



	g_print("/*-----------------------------------------------------------------------------------------------------------*/\n");
	g_print("/**********------------My chain is invoked*-------------*/\n");

	/*----------------Gstreamer Variable declarartions----------------------*/
	/*------------------------------------------------------------*/
	GstBuffer *my_buffer = NULL;
	GstVideoInfo video_info,p_video_info;
	GstVideoFrame vframe,p_vframe;
	GstCaps *caps=NULL;


	/*------------------Basic declartions for own buffer-----------------*/
	/*----------------------------------------------------*/
	guint8 *y_pixels=NULL;
	guint8 *y_pixel=NULL;
	guint8 *uv_pixels=NULL;
	guint8 *uv_pixel=NULL;
	//yv12 format
	guint8 *u_pixels=NULL;
	guint8 *v_pixels=NULL;
	guint8 *u_pixel=NULL;
	guint8 *v_pixel=NULL;
	guint y_stride=0;
	guint uv_stride=0;
	guint u_stride=0;
	guint v_stride=0;
	guint pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes
	//yv12
	guint pixel_stride_y = 0; // pixel stride for Y plane in YV12
	guint pixel_stride_u = 0; // pixel stride for U plane in YV12
	guint pixel_stride_v = 0; // pixel stride for V plane in YV12
	gint width=filter->width,height=filter->height;
	gint size=0,bpp=0;
	/*------------------Basic declartions for predefined-----------------*/
	/*----------------------------------------------------*/
	guint8 *p_y_pixels=NULL;
	guint8 *p_y_pixel=NULL;
	guint8 *p_uv_pixels=NULL;
	guint8 *p_uv_pixel=NULL;
	//yv12 format
	guint8 *p_u_pixels=NULL;
	guint8 *p_v_pixels=NULL;
	guint8 *p_u_pixel=NULL;
	guint8 *p_v_pixel=NULL;
	guint p_y_stride=0;
	guint p_uv_stride=0;
	guint p_u_stride=0;
	guint p_v_stride=0;
	guint p_pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes
	//yv12
	guint p_pixel_stride_y = 0; // pixel stride for Y plane in YV12
	guint p_pixel_stride_u = 0; // pixel stride for U plane in YV12
	guint p_pixel_stride_v = 0; // pixel stride for V plane in YV12
	gint p_width=0,p_height=0;
	gint y_size=0,u_size=0,v_size=0;


	/*--------------------------Video Frame size calculations-------------------------*/
	/*--------------------------------------------------------------------------------*/
	if(!(strcmp(filter->format,"NV12"))) {
		y_size=(width*height);
		u_size=(width*height)/4;
		v_size=(width*height)/4;
		size=y_size+u_size+v_size;
	        g_print("Buffer creating size:%d\n",size);
		bpp=12;
	}
	else if(!(strcmp(filter->format,"NV16"))) {
		y_size=(width*height);
		u_size=(width*height)/2;
		v_size=(width*height)/2;
		size=y_size+u_size+v_size;
	        g_print("Buffer creating size:%d\n",size);
		bpp=16;
	}
	else if(!(strcmp(filter->format,"NV24"))) {
		y_size=(width*height);
		u_size=(width*height);
		v_size=(width*height);
		size=y_size+u_size+v_size;
	        g_print("Buffer creating size:%d\n",size);
		bpp=24;
	} 
	else if(!(strcmp(filter->format,"RGB"))) {
		y_size=(width*height);
		u_size=(width*height);
		v_size=(width*height);
		size=y_size+u_size+v_size;
	        g_print("Buffer creating size:%d\n",size);
		bpp=24;
	}
	else if(!(strcmp(filter->format,"GRAY8"))) {	
		y_size=(width*height)*1;
		size=y_size;
	        g_print("Buffer creating size:%d\n",size);
		bpp=8;
	}
	else if(!(strcmp(filter->format,"RGBx"))) {
                y_size=(width*height)*4;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=32;
	}
	else if(!(strcmp(filter->format,"xRGB"))) {
		y_size=(width*height)*4;
		size=y_size;
		g_print("Buffer creating size:%d\n",size);
		bpp=32;
	}
	else if(!(strcmp(filter->format,"BGRx"))) {
		y_size=(width*height)*4;
		size=y_size;
		g_print("Buffer creating size:%d\n",size);
		bpp=32;
	}
	else if(!(strcmp(filter->format,"xBGR"))) {
                y_size=(width*height)*4;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=32;
        }
	else if(!(strcmp(filter->format,"RGBA"))) {
                y_size=(width*height)*4;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=32;
	}
	else if(!(strcmp(filter->format,"ARGB"))) {
		y_size=(width*height)*4;
		size=y_size;
		g_print("Buffer creating size:%d\n",size);
		bpp=32;
	}
	else if(!(strcmp(filter->format,"BGRA"))) {
                y_size=(width*height)*4;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=32;
        }
	else if(!(strcmp(filter->format,"ABGR"))) {
                y_size=(width*height)*4;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=32;
        }
	else if(!(strcmp(filter->format,"BGR"))) {
                y_size=(width*height)*3;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=24;
        }
	else if(!(strcmp(filter->format,"RGB16"))) {
		y_size=(width*height)*2;
		size=y_size;
		g_print("Buffer creating size:%d\n",size);
		bpp=24;
	}
	else if(!(strcmp(filter->format,"RGB15"))) {
                y_size=(width*height)*2;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=24;
        }
	else if(!(strcmp(filter->format,"I420"))) {
                y_size=(width*height)*1.5;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=12;
	}
	else if(!(strcmp(filter->format,"GRAY16_LE"))) {
		y_size=(width*height)*2;
		size=y_size;
		g_print("Buffer creating size:%d\n",size);
		bpp=16;
	}
	else if(!(strcmp(filter->format,"GRAY16_BE"))) {
		y_size=(width*height)*2;
		size=y_size;
		g_print("Buffer creating size:%d\n",size);
		bpp=16;
	}
	else if(!(strcmp(filter->format,"NV21"))) {
                y_size=(width*height)*1.5;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=16;
        }
	else if(!(strcmp(filter->format,"AYUV"))) {
                y_size=(width*height)*4;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=32;
        }
	else if(!(strcmp(filter->format,"YUY2"))) {
                y_size=(width*height)*2;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=16;
        }
	else if(!(strcmp(filter->format,"YVYU"))) {
                y_size=(width*height)*2;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=16;
        }
	else if(!(strcmp(filter->format,"UYVY"))) {
                y_size=(width*height)*2;
                size=y_size;
                g_print("Buffer creating size:%d\n",size);
                bpp=16;
        }





















	/*---------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------------*/

	/*---------------------------Corner points calculations------------------*/
	/*-------------------------------------------------------------------------*/

	/*---------------for corner inputs---------------*/
	gint corner_x = filter->xco;
	gint corner_y = filter->yco;
	g_print("corner x:%d    corner y:%d\n",corner_x,corner_y);

	// Calculate the coordinates of the top-left and bottom-right corners
	gint rect_bottom = corner_y + height;
	gint rect_right = corner_x + width;
	g_print("rect bot:%d    rect right:%d\n",rect_bottom,rect_right);

	/*--------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------*/
	/*---------------Give the writable permissions to incoming buffer----------------------*/
	/*if (!gst_buffer_make_writable(buf))
	  {
	  g_print("Failed to make the buffer writable\n");
	  gst_buffer_unref(buf);
	  return -1;
	  }
	  else
	  {
	  g_print("Successfully writable permissions is given to predefined buffer\n");
	  }*/

	/*------------------------Caps filling----------------------------*/
	/*----------------------------------------------------------------*/
	// Create a caps structure to describe the frame format
	caps = gst_caps_new_simple("video/x-raw",
			"format", G_TYPE_STRING, filter->format,
			"width", G_TYPE_INT, width,
			"height", G_TYPE_INT, height,
			"bpp", G_TYPE_INT, bpp,
			NULL);

	/*------------------------------Buffer creation and video mapping-------*/
	/*-----------------------------------------------------------------------*/
	// Allocate a buffer with the specified size
	my_buffer= gst_buffer_new_allocate(NULL, size, NULL );
	if (!gst_buffer_make_writable(my_buffer)) 
	{
		g_print("Failed to make the buffer writable\n");
		gst_buffer_unref(my_buffer);
		return -1;
	}
	else
	{
		g_print("Successfully writable permissions is given to own buffer\n");
	}
	gst_buffer_memset(my_buffer,0,0,size);

	/*---------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------predefined buffer--------------------------*/
	/*---------------------------------------------------------------*/
	if(gst_video_info_from_caps(&p_video_info, gst_pad_get_current_caps(pad)))
	{
		g_print("Successfully videoinfo is taken and updated predefined caps\n");
	}
	else
	{
		g_print("failure videoinfo is mapped in predefined buffer\n");
	}
	/*--------------------own buffer--------------------------*/
	/*-------------------------------------------------------*/
	if(gst_video_info_from_caps(&video_info, caps))
	{
		g_print("Successfully videoinfo is taken and updated own caps\n");
	}
	else
	{
		g_print("failure videoinfo is mapped in own buffer\n");
	}

	/*----------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------Predefined buffer video map--------------------------*/
	/*------------------------------------------------------------------------*/
	if(gst_video_frame_map(&p_vframe, &p_video_info, buf, GST_MAP_READ))
	{
		g_print("Successfully videoframe is mapped for predefined buffer\n");
	}
	else
	{
		g_print("failure videoframe is mapped for predefined buffer\n");
	}
	/*--------------------------Own buffer video map--------------------------*/
	/*------------------------------------------------------------------------*/
	if(gst_video_frame_map(&vframe, &video_info, my_buffer, GST_MAP_WRITE))
	{
		g_print("Successfully videoframe is mapped for own buffer\n");
	}
	else
	{
		g_print("failure videoframe is mapped for own buffer\n");
	}
	p_width = GST_VIDEO_FRAME_WIDTH(&p_vframe);
	p_height = GST_VIDEO_FRAME_HEIGHT(&p_vframe);
	gint planes=GST_VIDEO_FRAME_N_PLANES(&vframe);
	g_print("The no of planes :%d\n",planes);
	if(rect_right > p_width || rect_bottom > p_height)
	{
		g_print("Rect right:%d   The Rect bot:%d\n",rect_right,rect_bottom);
		g_print("predefined width:%d   height:%d\n",p_width,p_height);
		g_warning("exceeded caps\n");
		gst_video_frame_unmap(&vframe);
		gst_video_frame_unmap(&p_vframe);
		exit(1);
	}
	gint form=0;
	form=video_info.finfo->format;
	g_print("The video format no:%d\n",form);


	/*---------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------NV12 Format code-----------------------------------------------------------------------------------*/

	if(planes == 2)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
		uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)

		y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
		uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride
		pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

		width = GST_VIDEO_FRAME_WIDTH(&vframe);
		height = GST_VIDEO_FRAME_HEIGHT(&vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for own buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",width,height);
		g_print("The y stride:%d    The y pixel stride:%d\n",y_stride,pixel_stride);
		g_print("The uv  stride:%d    The uv pixel stride:%d\n",uv_stride,pixel_stride);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		p_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 0); // Y plane
		p_uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 1); // UV plane (interleaved)

		p_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 0); // Y plane stride
		p_uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 1); // UV plane stride
		p_pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

		p_width = GST_VIDEO_FRAME_WIDTH(&p_vframe);
		p_height = GST_VIDEO_FRAME_HEIGHT(&p_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for predefined buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",p_width,p_height);
		g_print("The y stride:%d    The y pixel stride:%d\n",p_y_stride,p_pixel_stride);
		g_print("The uv  stride:%d    The uv pixel stride:%d\n",p_uv_stride,p_pixel_stride);

		/*---------------------------------------------------------------------------------------------------------------------------------*/


		/*-----------------------Denominator calculation--------------------------*/
		/*------------------------------------------------------------------------*/
		gint denom1=0,denom2=0;
		if(!(strcmp(filter->format,"NV12"))) {
			denom1=2;
			denom2=2;
		}
		else if(!(strcmp(filter->format,"NV16"))) {
			denom1=1;
			denom2=2;
		}
		else if(!(strcmp(filter->format,"NV24"))) {
			denom1=1;
			denom2=1;
		}
		else if(!(strcmp(filter->format,"NV21"))) {
                        denom1=2;
                        denom2=2;
                }


		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				y_pixel = y_pixels + h1 * y_stride + w1;
				uv_pixel = uv_pixels + (h1/denom1)  * uv_stride + (w1/denom2) * pixel_stride;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_y_pixel = p_y_pixels + h2 * p_y_stride + w2;
				p_uv_pixel = p_uv_pixels + (h2/denom1) * p_uv_stride + (w2/denom2 ) * p_pixel_stride;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom) 
				{
					y_pixel[0]=p_y_pixel[0];
					uv_pixel[0]=p_uv_pixel[0];
					uv_pixel[1]=p_uv_pixel[1];
				}
				else
				{
					y_pixel[0]=0;
					uv_pixel[0]=128;
					uv_pixel[1]=128;
				}

			}


		}
	}
	/*-----------------------------------------------------------NV12 code ended------------------------------------------------------*/
	/*---------------------------------------------------------------------------------------------------------------------------------*/

	/*------------------------------------------------------------YV12 code start-------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	if(planes == 3)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
		u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)
		v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 2); // UV plane (interleaved)

		y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
		u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride
		v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 2); // UV plane stride

		pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,0);
		pixel_stride_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,1);
		pixel_stride_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,2);

		width = GST_VIDEO_FRAME_WIDTH(&vframe);
		height = GST_VIDEO_FRAME_HEIGHT(&vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for own buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",width,height);
		g_print("The y stride:%d    The y pixel stride:%d\n",y_stride,pixel_stride_y);
		g_print("The u  stride:%d    The u pixel stride:%d\n",u_stride,pixel_stride_u);
		g_print("The v  stride:%d    The v pixel stride:%d\n",v_stride,pixel_stride_v);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		p_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 0); // Y plane
		p_u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 1); // UV plane (interleaved)
		p_v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 2); // UV plane (interleaved)

		p_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 0); // Y plane stride
		p_u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 1); // UV plane stride
		p_v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 2); // UV plane stride

		p_pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&p_vframe,0);
		p_pixel_stride_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&p_vframe,1);
		p_pixel_stride_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&p_vframe,2);

		p_width = GST_VIDEO_FRAME_WIDTH(&p_vframe);
		p_height = GST_VIDEO_FRAME_HEIGHT(&p_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for predefined buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",p_width,p_height);
		g_print("The y stride:%d    The y pixel stride:%d\n",p_y_stride,p_pixel_stride_y);
		g_print("The u  stride:%d    The u pixel stride:%d\n",p_u_stride,p_pixel_stride_u);
		g_print("The v  stride:%d    The v pixel stride:%d\n",p_v_stride,p_pixel_stride_v);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				y_pixel = y_pixels + h1 * y_stride + w1 * pixel_stride_y;
				u_pixel = u_pixels + h1 / 2 * u_stride + (w1 / 2) * pixel_stride_u;
				v_pixel = v_pixels + h1 / 2 * v_stride + (w1 / 2) * pixel_stride_v;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_y_pixel = p_y_pixels + h2 * p_y_stride + w2 * p_pixel_stride_y;
				p_u_pixel = p_u_pixels + h2 / 2 * p_u_stride + (w2 / 2) * p_pixel_stride_u;
				p_v_pixel = p_v_pixels + h2 / 2 * p_v_stride + (w2 / 2) * p_pixel_stride_v;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom) 
				{
					y_pixel[0]=p_y_pixel[0];
					u_pixel[0]=p_u_pixel[0];
					v_pixel[0]=p_v_pixel[0];
				}
				else
				{
					y_pixel[0]=0;
					u_pixel[0]=128;
					v_pixel[0]=128;
				}

			}


		}
	}
	/*------------------------------------------------------------YV12 code ended-------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------------------------*/


	/*------------------------------------------------------------Single Plane code started---------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	if(planes == 1)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane

		y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride

		pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe,0);

		width = GST_VIDEO_FRAME_WIDTH(&vframe);
		height = GST_VIDEO_FRAME_HEIGHT(&vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for own buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",width,height);
		g_print("The y stride:%d    The y pixel stride:%d\n",y_stride,pixel_stride_y);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		p_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 0); // Y plane

		p_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 0); // Y plane stride

		p_pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&p_vframe,0);

		p_width = GST_VIDEO_FRAME_WIDTH(&p_vframe);
		p_height = GST_VIDEO_FRAME_HEIGHT(&p_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for predefined buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",p_width,p_height);
		g_print("The y stride:%d    The y pixel stride:%d\n",p_y_stride,p_pixel_stride_y);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				y_pixel = y_pixels + h1 * y_stride + w1 * pixel_stride_y ;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_y_pixel = p_y_pixels + h2 * p_y_stride + w2 * p_pixel_stride_y ;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom) 
				{
					//p_y_pixel[0]=255;
					//y_pixel[3]=p_y_pixel[3];
					y_pixel[1]=p_y_pixel[1];
					y_pixel[2]=p_y_pixel[2];
					y_pixel[0]=p_y_pixel[0];
				}
				else
				{
					y_pixel[0]=0;
					y_pixel[1]=128;
					y_pixel[2]=128;
				}

			}


		}
	}
	/*------------------------------------------------------------Single code ended-------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------------------------*/



	g_print("Negotiated caps: %s\n", gst_caps_to_string(caps));
	// Set the negotiated caps on the source pad
	gst_pad_set_caps(filter->srcpad, caps);
	gst_video_frame_unmap(&vframe);
	gst_video_frame_unmap(&p_vframe);
	g_print("/*--------------------------------------------------------------------------------------------------------------------------*/\n");


	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, my_buffer);
}



/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */

static gboolean square_init (GstPlugin * square)
{
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template square' with your description
	 */

	GST_DEBUG_CATEGORY_INIT (gst_square_debug, "square",
			0, "Template square");

	return GST_ELEMENT_REGISTER (square, square);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstsquare"
#endif

/* gstreamer looks for this structure to register squares
 *
 * exchange the string 'Template square' with your square description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		square,
		"square",
		square_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
