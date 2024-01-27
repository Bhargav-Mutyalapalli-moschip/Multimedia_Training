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
 * SECTION:element-croptech
 *
 * FIXME:Describe croptech here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! croptech ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */
/*---------The above brief of licence info----------------*/




#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

// Predefined headerfile for gstreamer api's
#include <gst/gst.h>
#include <gst/video/video.h>


#include "gstcroptech.h"

#include "my_own_headers.h"

//creating category for own plugin which is used for debugging info
GST_DEBUG_CATEGORY_STATIC (gst_croptech_debug);
#define GST_CAT_DEFAULT gst_croptech_debug

/*-----If you want to add the own signals and own properties-----*/
enum
{
	/* FILL ME */
	LAST_SIGNAL
};

enum
{
	PROP_0,                  //it is filled with zero
	PROP_SILENT,    	//it is filled with one
	PROP_WIDTH,             //it is filled with two
	PROP_HEIGHT,            //it is filled with three
	PROP_XCO,               //it is filled with four
	PROP_YCO,               //it is filled with five
};



/*--------------------------defining the static pad templates--------------------*/
/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
//It creates the src template for my plugin
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("video/x-raw, "
			"format = (string) { I420, NV12, NV21, YV12,RGB16,NV16,NV24,\
			RGBx, xRGB, BGRx, xBGR, RGBA, ARGB,BGRA, ABGR, RGB, BGR, AYUV, YUY2, \
			YVYU, UYVY, I420, YV12, RGB16, RGB15, GRAY8, NV12, NV21,GRAY16_LE, GRAY16_BE },"
			"width = (int) [1, 2147483647], "
			"height = (int) [1, 2147483647], "
			"framerate = (fraction) [0/1, 2147483647/1]"));
//It creates the sink template for my plugin
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("video/x-raw, "
			"format = (string) { I420, NV12, NV21, YV12,RGB16,NV16,NV24,\
			RGBx, xRGB, BGRx, xBGR, RGBA, ARGB,BGRA, ABGR, RGB, BGR, AYUV, YUY2, \
			YVYU, UYVY, I420, YV12, RGB16, RGB15, GRAY8, NV12, NV21,GRAY16_LE, GRAY16_BE },"
			"width = (int) [1, 2147483647], "
			"height = (int) [1, 2147483647], "
			"framerate = (fraction) [0/1, 2147483647/1]"));


/*--------substitute the parent_class in place of own variable gst_custom_parent_class-------*/
#define gst_croptech_parent_class parent_class

/*-------It sets up the type hierarchy, initializes function pointers and  
 *  provides default implementations for object initialization and class initialization.
 */
G_DEFINE_TYPE (GstCroptech, gst_croptech, GST_TYPE_ELEMENT);

/*----------------it is used to define a new GStreamer element----------------------*/
GST_ELEMENT_REGISTER_DEFINE (croptech, "croptech", GST_RANK_NONE,GST_TYPE_CROPTECH);

/*--------------------Gobject class virtual functions signatures-----------------------------------------------*/
static void gst_croptech_set_property (GObject * object,guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_croptech_get_property (GObject * object,guint prop_id, GValue * value, GParamSpec * pspec);

/*--------------------Element class virtual functions signatures-----------------------------------------------*/
static gboolean gst_croptech_sink_event (GstPad * pad,GstObject * parent, GstEvent * event);
static GstFlowReturn gst_croptech_chain (GstPad * pad,GstObject * parent, GstBuffer * buf);



/*-------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------Function definitions---------------------------------------------------*/

//Every plugin first entry point
static gboolean croptech_init (GstPlugin * croptech)
{
	//GST_DEBUG_OBJECT(filter,"First entry point of every plugin:plugin_init\n");
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template croptech' with your description
	 */
	//It initializes the debug info through created category variable
	GST_DEBUG_CATEGORY_INIT (gst_croptech_debug, "croptech",0, "Template croptech");

	//It register the element which is defined
	return GST_ELEMENT_REGISTER (croptech, croptech);
}

//Second entry point of every plugin
static void gst_croptech_class_init (GstCroptechClass * klass)
{
	//GST_DEBUG_OBJECT(filter,"Second entry point of every plugin:class init\n");
	/*It is the GObjectClass  variable is a pointer to the class structure of a GObject-derived class and 
	 * it represents the blueprint or definition of a class and contains function pointers, class-specific data,
	 * and other information necessary for working with instances of that class.
	 */
	GObjectClass *gobject_class;

	/*GstElementClass is a fundamental data structure that
	 * represents the class definition for a GStreamer element.
	 */
	GstElementClass *gstelement_class;

	//it the retrieving the gobject class info by typecasting the generic klass pointer
	gobject_class = (GObjectClass *) klass;

	//it the retrieving the gstelement class info by typecasting the generic klass pointer
	gstelement_class = (GstElementClass *) klass;

	//It is initializing the own set property function base address to generic gobject class set property
	gobject_class->set_property = gst_croptech_set_property;

	//It is initializing the own get property function base address to generic gobject class get property
	gobject_class->get_property = gst_croptech_get_property;

	/*---If you want write your own proprty for your custom plugin then,you have install the property
	 * by using g_object_class_install_property you are going to install property.
	 * In the property you have input may boolen,int char can be defined by g_param_sepc_(input type)
	 */
	//it install the silence property 
	g_object_class_install_property (gobject_class, PROP_SILENT,g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",FALSE, G_PARAM_READWRITE));

	//it install the width property
	g_object_class_install_property (gobject_class, PROP_WIDTH,g_param_spec_int ("width", "width", "The width is to display the cropped part",DEF_START,DEF_END,DEF_WVAL,G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_HEIGHT,g_param_spec_int ("height", "height", "The height is to display the cropped part",DEF_START,DEF_END,DEF_HVAL,G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_XCO,g_param_spec_int ("xco", "xco", "The xco co-ord is to display the cropped part",DEF_XSTART,DEF_XEND,DEF_XVAL,G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_YCO,g_param_spec_int ("yco", "yco", "The yco co-ord is to display the cropped part",DEF_YSTART,DEF_YEND,DEF_YVAL,G_PARAM_READWRITE));

	/*---If you are writing a simple plugin details or metadata is important to specify the author and other info
	 * which is nontechnical info and technical info.
	 */
	gst_element_class_set_details_simple (gstelement_class,
			"Video crop",
			"Filter/Effect/Video",
			"Crops video into a user defined region", "Bhargav <mbrpranav659@gmail.com>");

	/*-----This function is used to define the input and output pad templates that an element class supports.---*/
	//it add the src template to the element class
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&src_factory));

	//it add the sink template to the element class
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&sink_factory));
}

//Third entry point of every plugin
static void gst_croptech_init (GstCroptech * filter)
{
	GST_DEBUG_OBJECT(filter,"Third entry point of every plugin:gst_plugin_init\n");

	/*--------------PAD RETRIEVAL-----------------------*/
	//it retrieves the src pad from static pad templates
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

	//it retrieves the sink pad from static pad templates
	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");

	/*--------------event function callback creation-------------------*/
	/*----GStreamer provides a mechanism for handling events on pads.
	 * Various types of events, such as EOS (End of Stream), flush, and custom application-
	 * specific events, can be sent and received through pads. Handling events can be crucial
	 * for proper pipeline behavior and synchronization.
	 */
	// Set the event handling callback function for the sink pad
	gst_pad_set_event_function (filter->sinkpad,GST_DEBUG_FUNCPTR (gst_croptech_sink_event));

	/*--------------chain function callback creation-------------------*/
	/*---It is used to set a callback function that gets called when data buffers are pushed or
	 * chained  through a GStreamer pad.
	 */
	// Set the chaining callback function for the sink pad
	gst_pad_set_chain_function (filter->sinkpad,GST_DEBUG_FUNCPTR (gst_croptech_chain));

	/*---------Set the pads to proxy pads-------*/
	/*----it automatically adopts the capabilities of the source pad
	 * it is connected to, ensuring that data flow and processing remain consistent
	 * and well-aligned throughout the pipeline.
	 */
	//set the src pads
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);

	//set the sink pads
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);

	/*------------Add the pads to the elements------*/
	// Add the source pad to the element
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	// Add the sink pad to the element
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);


	filter->silent = FALSE;
	filter->width = DEF_WVAL;
	filter->height = DEF_HVAL;
	filter->xco = DEF_XVAL ;
	filter->yco = DEF_YVAL;
}

//Fourth Entry point of every plugin(If you set the properties dynamically)
static void gst_croptech_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	//It retrieves the elements data from object
	GstCroptech *filter = GST_CROPTECH (object);
	GST_DEBUG_OBJECT(filter,"gst_croptech set property  is invoked\n");

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			break;
		case PROP_WIDTH:
			if (g_value_get_int(value) < 0) 
			{
				flag1=1;
			}
			filter->width = g_value_get_int (value);
			break;
		case PROP_HEIGHT:
			if (g_value_get_int(value) < 0) 
			{
				flag2=1;
			}
			filter->height = g_value_get_int (value);
			break;
		case PROP_XCO:
			if (g_value_get_int(value) < 0) 
			{
				flag3=1;
			}
			filter->xco = g_value_get_int (value);
			break;
		case PROP_YCO:
			if (g_value_get_int(value) < 0) 
			{
				flag4=1;
			}
			filter->yco = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
	if( flag1 == 1 || flag2 == 1 || flag3 == 1 || flag4 == 1)
	{
		if(flag1 == 1)
		{
			g_warning("Width cannot be set to a negative value");
			g_message("Please give the positive value\n");
		}
		else if(flag2 == 1)
		{
			g_warning("Height cannot be set to a negative value");
			g_message("Please give the positive value\n");
		}
		else if(flag3 == 1)
		{
			g_warning("X cordinate cannot be set to a negative value");
			g_message("Please give the positive value\n");
		}
		else if(flag4 == 1)
		{
			g_warning("Y cordinate cannot be set to a negative value");
			g_message("Please give the positive value\n");
		}
		exit(-1);
	}

}

//Fifth Entry point of every plugin(If you request the properties)
static void gst_croptech_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
	//It retrieves the elements data from object
	GstCroptech *filter = GST_CROPTECH (object);

	switch (prop_id) {
		case PROP_SILENT:
			g_value_set_boolean (value, filter->silent);
			break;
		case PROP_WIDTH:
			g_value_set_int (value, filter->width);
			break;
		case PROP_HEIGHT:
			g_value_set_int (value, filter->height);
			break;
		case PROP_XCO:
			g_value_set_int (value, filter->xco);
			break;
		case PROP_YCO:
			g_value_set_int (value, filter->yco);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

//Sink event handling function
static gboolean gst_croptech_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
	GstCroptech *filter;
	gboolean ret;

	//It retrieves the elements data from object
	filter = GST_CROPTECH (parent);
	GST_DEBUG_OBJECT(filter,"croptech plugin sink event is invoked");

	//For debug info to genarate the log file
	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,GST_EVENT_TYPE_NAME (event), event);

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CAPS:
			{
				GstCaps *caps;

				//It is used to extract the capabilities (caps) information from a GStreamer event.
				gst_event_parse_caps (event, &caps);

				/* do something with the caps */
				//convert our caps to string for human readable format
				gchar *caps_str = gst_caps_to_string(caps);
				if(!caps_str)
				{
					GST_DEBUG_OBJECT(filter,"The received caps is :NULL\n");
				}
				GST_DEBUG_OBJECT(filter,"Received caps: %s\n", caps_str);
				g_free(caps_str); //deallocate the memory for caps_str
				/* and forward */

				//It is used to perform the default handling of a GStreamer event by a pad in a GStreamer element.
				ret = gst_pad_event_default (pad, parent, event);
				break;
			}
		default:
			GST_DEBUG_OBJECT(filter,"default event is handling\n");
			//It is used to perform the default handling of a GStreamer event by a pad in a GStreamer element.
			ret = gst_pad_event_default (pad, parent, event);
			break;
	}
	return ret;
}

//chain function this function does the actual processing
static GstFlowReturn gst_croptech_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstCroptech *filter;

	//It retrieves the elements data from object
	filter = GST_CROPTECH (parent); 

	org_width=filter->width; 
	org_height=filter->height;


	/*----Retrieving the predfined caps------------------------------*/
	pre_caps=gst_pad_get_current_caps(pad);
	if(!pre_caps)
	{
		GST_DEBUG_OBJECT(filter,"Failed to retrieve the current cap \n");
		return GST_FLOW_ERROR;

	}
	/*--------------------Predefined buffer--------------------------*/
	if(gst_video_info_from_caps(&pre_video_info, pre_caps))
	{
		GST_DEBUG_OBJECT(filter,"Successfully caps parsed and video info is updated \n");
	}
	else
	{
		GST_DEBUG_OBJECT(filter,"failed  to Parse caps and update video info \n");
		return GST_FLOW_ERROR;
	}

	/*----------PREDEFINED CAPS GETS SUBSTITUTED--------------*/
	if(filter->width == DEF_WVAL ) //if it is default means we have to update the incoming caps
	{
		org_width=pre_video_info.width;
	}
	if(filter-> height == DEF_HVAL) //if it is default means we have to update the incoming caps
	{
		org_height=pre_video_info.height;
	}
	/*--------------------------------------------------------*/

	/*-----Retrieving the format macro number from the video info structure-------*/
	org_format_no=pre_video_info.finfo->format;

	/*--------------------------------Video Frame size calculations:start-----------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	switch(org_format_no)
	{
		case GST_VIDEO_FORMAT_NV12:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height)/4;
				org_v_size=(org_width * org_height)/4;
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for NV12 : %d\n",org_size);
				org_bpp=12;
				denom1=2;
				denom2=2;
				strcpy(filter->format,"NV12");
				break;
			}
		case GST_VIDEO_FORMAT_YV12:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height)/4;
				org_v_size=(org_width * org_height)/4;
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for YV12 : %d\n",org_size);
				strcpy(filter->format,"YV12");
				org_bpp=12;
				break;
			}

		case GST_VIDEO_FORMAT_NV21:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height)/4;
				org_v_size=(org_width * org_height)/4;
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for NV21 : %d\n",org_size);
				denom1=2;
				denom2=2;
				strcpy(filter->format,"NV21");
				org_bpp=12;
				break;
			}

		case GST_VIDEO_FORMAT_NV16:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height)/2;
				org_v_size=(org_width * org_height)/2;
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for NV16 : %d\n",org_size);
				denom1=1;
				denom2=2;
				strcpy(filter->format,"NV16");
				org_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_NV24:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for NV24 : %d\n",org_size);
				denom1=1;
				denom2=1;
				strcpy(filter->format,"NV24");
				org_bpp=24;
				break;
			}
		case GST_VIDEO_FORMAT_RGB:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for RGB : %d\n",org_size);
				strcpy(filter->format,"RGB");
				org_bpp=24;
				ind=0;
				break;
			}
		case GST_VIDEO_FORMAT_BGR:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for BGR : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"BGR");
				org_bpp=24;
				break;
			}

		case GST_VIDEO_FORMAT_GRAY8:
			{
				org_y_size=(org_width * org_height);
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for GRAY8 : %d\n",org_size);
				strcpy(filter->format,"GRAY8");
				org_bpp=8;
				break;
			}
		case GST_VIDEO_FORMAT_RGBx:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for RGBx : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"RGBx");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_xRGB:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for xRGB : %d\n",org_size);
				ind=3;
				strcpy(filter->format,"xRGB");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_BGRx:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for BGRx : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"BGRx");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_xBGR:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for xBGR : %d\n",org_size);
				ind=3;
				strcpy(filter->format,"xBGR");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_RGBA:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for RGBA : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"RGBA");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_ARGB:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for ARGB : %d\n",org_size);
				ind=3;
				strcpy(filter->format,"ARGB");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_BGRA:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for BGRA : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"BGRA");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_ABGR:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for ABGR : %d\n",org_size);
				ind=3;
				strcpy(filter->format,"ABGR");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_RGB16:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for RGB16 : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"RGB16");
				org_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_RGB15:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for RGB15 : %d\n",org_size);
				strcpy(filter->format,"RGB15");
				org_bpp=15;
				ind=0;
				break;
			}
		case GST_VIDEO_FORMAT_I420:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height)/4;
				org_v_size=(org_width * org_height)/4;
				org_size=org_y_size + org_u_size + org_v_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for I420 : %d\n",org_size);
				denom1=2;
				denom2=2;
				strcpy(filter->format,"I420");
				org_bpp=12;
				break;
			}
		case GST_VIDEO_FORMAT_GRAY16_LE:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for GRAY16_LE : %d\n",org_size);
				strcpy(filter->format,"GRAY16_LE");
				org_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_GRAY16_BE:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for GRAY16_BE : %d\n",org_size);
				strcpy(filter->format,"GRAY16_BE");
				org_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_AYUV:
			{
				org_y_size=(org_width * org_height);
				org_u_size=(org_width * org_height);
				org_v_size=(org_width * org_height);
				org_e_size=(org_width * org_height);
				org_size=org_y_size + org_u_size + org_v_size + org_e_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for AYUV : %d\n",org_size);
				ind=3;
				strcpy(filter->format,"AYUV");
				org_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_YUY2:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for YUY2 : %d\n",org_size);
				ind=0;
				strcpy(filter->format,"YUY2");
				org_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_YVYU:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for YVYU : %d\n",org_size);
				strcpy(filter->format,"YVYU");
				org_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_UYVY:
			{
				org_y_size=(org_width * org_height)*2;
				org_size=org_y_size;
				GST_DEBUG_OBJECT(filter,"Buffer creating size for UYVY : %d\n",org_size);
				strcpy(filter->format,"UYVY");
				org_bpp=16;
				break;
			}
		default:
			g_print("Please give valid format\n");
			break;
	}
	/*-----------------------------------------------------------------------------------------------------*/
	/*--------------------------------Video Frame size calculations:end------------------------------------*/

	/*----------------Caps for my own buffer -----------------*/
	// Create a caps structure to describe the frame format
	org_caps = gst_caps_copy(pre_caps); //copy the caps from predefined caps(metadata also copied)
	if(!org_caps)
	{
		GST_DEBUG_OBJECT(filter,"Failed to copy the predefined caps\n");
		return GST_FLOW_ERROR;
	}
	GstStructure *structure = gst_caps_get_structure(org_caps, 0); //retrieve the caps structure

	/*----It is used the some parameters in the structure ---*/
	gst_structure_set(structure, "width", G_TYPE_INT, org_width, \
			"height", G_TYPE_INT, org_height, \
			"format", G_TYPE_STRING, filter->format,\
			"bpp",G_TYPE_INT,org_bpp, \
			NULL);
	/*-------------------------------------------------------*/


	/*----------------------------------Video info from caps:start-------------------------------------------*/
	/*--------------------own buffer--------------------------*/
	/*-------------------------------------------------------*/
	if(gst_video_info_from_caps(&org_video_info, org_caps))
	{
		GST_DEBUG_OBJECT(filter,"Successfully caps parsed and video info is updated \n");
	}
	else
	{
		GST_DEBUG_OBJECT(filter,"Failed  parse caps and video info  updation \n");
		return GST_FLOW_ERROR;
	}
	/*----------------------------------Video info from caps:end-------------------------------------------*/


	/*------------------------------------Corner points calculations:start---------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	gint corner_x = filter->xco;
	gint corner_y = filter->yco;
	GST_DEBUG_OBJECT(filter,"corner x:%d    corner y:%d\n",corner_x,corner_y);

	// Calculate the coordinates of the top-left and bottom-right corners
	gint rect_bottom = corner_y + org_height;
	gint rect_right = corner_x + org_width;
	GST_DEBUG_OBJECT(filter,"The max height : %d   The max width : %d\n",rect_bottom,rect_right);

	/*-----------------------------------------------------------------------------------------------------*/
	/*------------------------------------Corner points calculations:end----------------------------------*/

	/*--------------------------------------Buffer creation:start----------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/
	// Allocate a buffer with the specified size
	my_buffer= gst_buffer_new_allocate(NULL, org_size, NULL );
	if (!gst_buffer_make_writable(my_buffer))  //Give the writable permissions to newly created buffer
	{
		GST_DEBUG_OBJECT(filter,"Failed  to make the buffer writable \n");
		gst_buffer_unref(my_buffer);
		return GST_FLOW_ERROR;
	}
	else
	{
		GST_DEBUG_OBJECT(filter,"Successfully writable permissions are given to own buffer\n");
	}
	gst_buffer_memset(my_buffer,0,0,org_size); //it clears the garbage values in created buffer

	/*---------------------------------------------------------------------------------------------------*/
	/*--------------------------------------Buffer creation:end------------------------------------------*/

	/*--------------------------------------Video frame mapping:start------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/
	GstMapInfo in_map, out_map;
	//Predefined buffer video map
	/*if(gst_video_frame_map(&pre_vframe, &pre_video_info, buf, GST_MAP_READ))
	  {
	  GST_DEBUG_OBJECT(filter,"Successfilly videoframe is mapped for predefined buffer \n");
	  }
	  else
	  {
	  GST_DEBUG_OBJECT(filter,"Failed to map the video frame for predefined buffer\n");
	  return GST_FLOW_ERROR;
	  }*/
	if (!gst_buffer_map (buf, &in_map, GST_MAP_READWRITE)) {
		g_warning("Failed to do buffer map\n");
	}

	gboolean ret = FALSE;

	// Map input buffer
	if (!gst_buffer_map (my_buffer, &out_map, GST_MAP_WRITE)) {
		g_warning("Failed to do buffer map\n");
	}


	//Own buffer video map
	/*if(gst_video_frame_map(&org_vframe, &org_video_info, my_buffer, GST_MAP_WRITE))
	  {
	  GST_DEBUG_OBJECT(filter,"Successfilly videoframe is mapped for own buffer \n");
	  }
	  else
	  {
	  GST_DEBUG_OBJECT(filter,"Failed to map the video frame for own buffer\n");
	  g_warning("Unable to map this caps(width & height)\n");
	  g_message("please follow the some standards\n");
	  return GST_FLOW_ERROR;

	  }*/
	/*---------------------------------------------------------------------------------------------------*/
	/*--------------------------------------Video frame mapping:end--------------------------------------*/


	/*--------------------------------------Errors checking:start----------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/
	//pre_width = GST_VIDEO_FRAME_WIDTH(&pre_vframe); //it retrieves the predefined width
	//pre_height = GST_VIDEO_FRAME_HEIGHT(&pre_vframe); //it retrieves the predefined height
	pre_width = GST_VIDEO_INFO_WIDTH(&pre_video_info); //it retrieves the predefined width
	pre_height = GST_VIDEO_INFO_HEIGHT(&pre_video_info); //it retrieves the predefined height
							     // Get the first structure from the caps
	structure = gst_caps_get_structure(org_caps, 0);
	gint planes=0;
	planes=org_video_info.finfo->n_planes;
	//g_print("No of planes1:%d\n",planes1);
	//gint planes=GST_VIDEO_FRAME_N_PLANES(&org_vframe); //it retrieves the no of planes 
	//g_print("No of planes:%d\n",planes);
	GST_DEBUG_OBJECT(filter,"No of planes : %d\n",planes);

	if(rect_right > pre_width || rect_bottom > pre_height)
	{
		g_warning("Please give the width,height,x cordinate and y cordinate within the limits only\n");
		g_message("The valid caps : Width : %d  Height : %d\n",pre_width,pre_height);
		g_message("Give the inputs within this caps limit\n");
		//gst_video_frame_unmap(&org_vframe);
		gst_buffer_unmap (my_buffer, &out_map);
		gst_buffer_unmap (buf, &in_map);
		//gst_video_frame_unmap(&pre_vframe);
		return GST_FLOW_ERROR;
	}
	/*---------------------------------------------------------------------------------------------------*/
	/*--------------------------------------Errors checking:end------------------------------------------*/

	/*--------------------------------------Main cropping logic:start------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/

	if(planes == 1)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		org_n_pixels = GST_VIDEO_FRAME_PLANE_DATA(&org_vframe, 0); // Pixels data starting base address

		org_rgb_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&org_vframe, 0); // single plane stride

		org_pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&org_vframe,0); // single plane pixel stride

		org_width = GST_VIDEO_FRAME_WIDTH(&org_vframe); //retrieves the width
		org_height = GST_VIDEO_FRAME_HEIGHT(&org_vframe); //retrieves the height

		GST_DEBUG_OBJECT(filter,"Strides of a own buffer\n");
		GST_DEBUG_OBJECT(filter,"The width:%d (which is updated in caps)   The height:%d\n",org_width,org_height);
		GST_DEBUG_OBJECT(filter,"The rgb stride:%d    The rgb pixel stride:%d\n",org_rgb_stride,org_pixel_stride);


		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		pre_n_pixels = GST_VIDEO_FRAME_PLANE_DATA(&pre_vframe, 0); // Pixels data starting base address

		pre_rgb_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&pre_vframe, 0); // single plane stride

		pre_pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&pre_vframe,0); //single plane pixel stride

		pre_width = GST_VIDEO_FRAME_WIDTH(&pre_vframe); //retrieves the width
		pre_height = GST_VIDEO_FRAME_HEIGHT(&pre_vframe); //retrieves the height

		GST_DEBUG_OBJECT(filter,"Strides and caps info for predefined buffer\n");
		GST_DEBUG_OBJECT(filter,"The width:%d (which is updated in caps)   The height:%d\n",pre_width,pre_height);
		GST_DEBUG_OBJECT(filter,"The rgb stride:%d    The rgb pixel stride:%d\n",pre_rgb_stride,pre_pixel_stride);




		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<org_height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<org_width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				org_rgb_pixel = org_n_pixels + h1 * org_rgb_stride + w1 * org_pixel_stride ;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				pre_rgb_pixel = pre_n_pixels + h2 * pre_rgb_stride + w2 * pre_pixel_stride ;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
				{
					if(org_format_no == GST_VIDEO_FORMAT_GRAY8)
					{
						org_rgb_pixel[0]=pre_rgb_pixel[0];
					}
					else if( org_format_no == GST_VIDEO_FORMAT_GRAY16_BE || org_format_no == GST_VIDEO_FORMAT_GRAY16_LE)
					{
						org_rgb_pixel[0]=pre_rgb_pixel[0];
						org_rgb_pixel[1]=pre_rgb_pixel[1];
					}
					else if( org_format_no == GST_VIDEO_FORMAT_UYVY || org_format_no == GST_VIDEO_FORMAT_YVYU)
					{


						org_rgb_pixel[0]=pre_rgb_pixel[0];
						org_rgb_pixel[1]=pre_rgb_pixel[1];
						org_rgb_pixel[2]=pre_rgb_pixel[2];
						org_rgb_pixel[3]=pre_rgb_pixel[3];
					}
					else
					{


						org_rgb_pixel[ind]=pre_rgb_pixel[ind];
						org_rgb_pixel[1]=pre_rgb_pixel[1];
						org_rgb_pixel[2]=pre_rgb_pixel[2];
					}

				}
				else
				{
					org_rgb_pixel[0]=0;
					org_rgb_pixel[1]=128;
					org_rgb_pixel[2]=128;
				}

			}


		}
	}

	if(planes == 2)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		org_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&org_vframe, 0); // Y plane
		g_print("The y address:%p\n",org_y_pixels);
		org_uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&org_vframe, 1); // UV plane (interleaved)
		g_print("The uv address:%p\n",org_uv_pixels);

		org_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&org_vframe, 0); // Y plane stride
		org_uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&org_vframe, 1); // UV plane stride
		org_pixel_stride = 2; // In NV-Formates, UV values are interleaved every 2 bytes

		org_width = GST_VIDEO_FRAME_WIDTH(&org_vframe);
		org_height = GST_VIDEO_FRAME_HEIGHT(&org_vframe);

		GST_DEBUG_OBJECT(filter,"Strides and caps info for Own buffer\n");
		GST_DEBUG_OBJECT(filter,"The width:%d (which is updated in caps)   The height:%d\n",org_width,org_height);
		GST_DEBUG_OBJECT(filter,"The y stride:%d    The y pixel stride:%d\n",org_y_stride,org_pixel_stride);
		GST_DEBUG_OBJECT(filter,"The uv  stride:%d    The uv pixel stride:%d\n",org_uv_stride,org_pixel_stride);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		pre_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&pre_vframe, 0); // Y plane
		pre_uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&pre_vframe, 1); // UV plane (interleaved)

		pre_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&pre_vframe, 0); // Y plane stride
		pre_uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&pre_vframe, 1); // UV plane stride
		pre_pixel_stride = 2; // In NV-Formates, UV values are interleaved every 2 bytes

		pre_width = GST_VIDEO_FRAME_WIDTH(&pre_vframe);
		pre_height = GST_VIDEO_FRAME_HEIGHT(&pre_vframe);

		GST_DEBUG_OBJECT(filter,"Strides and caps info for Predefined buffer\n");
		GST_DEBUG_OBJECT(filter,"The width:%d (which is updated in caps)   The height:%d\n",pre_width,pre_height);
		GST_DEBUG_OBJECT(filter,"The y stride:%d    The y pixel stride:%d\n",pre_y_stride,pre_pixel_stride);
		GST_DEBUG_OBJECT(filter,"The uv  stride:%d    The uv pixel stride:%d\n",pre_uv_stride,pre_pixel_stride);


		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<org_height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<org_width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				org_y_pixel = org_y_pixels + h1 * org_y_stride + w1;
				org_uv_pixel = org_uv_pixels + (h1/denom1)  * org_uv_stride + (w1/denom2) * org_pixel_stride;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				pre_y_pixel = pre_y_pixels + h2 * pre_y_stride + w2;
				pre_uv_pixel = pre_uv_pixels + (h2/denom1) * pre_uv_stride + (w2/denom2 ) * pre_pixel_stride;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
				{
					org_y_pixel[0]=pre_y_pixel[0];
					org_uv_pixel[0]=pre_uv_pixel[0];
					org_uv_pixel[1]=pre_uv_pixel[1];
				}
				else
				{
					org_y_pixel[0]=0;
					org_uv_pixel[0]=128;
					org_uv_pixel[1]=128;
				}

			}


		}
	}
	if(planes == 3)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		//org_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&org_vframe, 0); // Y plane
		//org_u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&org_vframe, 1); // U plane 
		//org_v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&org_vframe, 2); // V plane 

		//org_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&org_vframe, 0); // Y plane stride
		gint out_stride_y = GST_VIDEO_INFO_PLANE_STRIDE (&org_video_info, 0);
		g_print("The frame stride:%d the info stride:%d\n",org_y_stride,out_stride_y);
		//org_u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&org_vframe, 1); // U plane stride
		gint out_stride_u = GST_VIDEO_INFO_PLANE_STRIDE (&org_video_info, 1);
		g_print("The frame stride:%d the info stride:%d\n",org_u_stride,out_stride_u);
		//org_v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&org_vframe, 2); // V plane stride
		gint out_stride_v = GST_VIDEO_INFO_PLANE_STRIDE (&org_video_info, 2);
		g_print("The frame stride:%d the info stride:%d\n",org_v_stride,out_stride_v);

		//org_pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&org_vframe,0); //Y plane pixel stride 
		gint out_pixel_stride_y = GST_VIDEO_INFO_COMP_PSTRIDE (&org_video_info, 0);
		g_print("The frame stride pixel:%d the info stride pixel:%d\n",org_pixel_stride_y,out_pixel_stride_y);
		//org_pixel_stride_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&org_vframe,1); //U plane pixel stride
		gint out_pixel_stride_u = GST_VIDEO_INFO_COMP_PSTRIDE (&org_video_info, 1);
		g_print("The frame stride pixel:%d the info stride pixel:%d\n",org_pixel_stride_u,out_pixel_stride_u);
		//org_pixel_stride_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&org_vframe,2); //V plane pixel stride
		gint out_pixel_stride_v = GST_VIDEO_INFO_COMP_PSTRIDE (&org_video_info, 2);
		g_print("The frame stride pixel:%d the info stride pixel:%d\n",org_pixel_stride_v,out_pixel_stride_v);

		//org_width = GST_VIDEO_FRAME_WIDTH(&org_vframe); //retrieves the width
		//org_height = GST_VIDEO_FRAME_HEIGHT(&org_vframe); //retreves the height
		org_width = GST_VIDEO_INFO_WIDTH(&org_video_info); //retrieves the width
		g_print("The width:%d\n",org_width);
		org_height = GST_VIDEO_INFO_HEIGHT(&org_video_info); //retreves the height
		g_print("The height:%d\n",org_height);



		GST_DEBUG_OBJECT(filter,"Strides and caps info for Own buffer\n");
		GST_DEBUG_OBJECT(filter,"The width:%d (which is updated in caps)   The height:%d\n",org_width,org_height);
		GST_DEBUG_OBJECT(filter,"The y  stride:%d    The y pixel stride:%d\n",org_y_stride,org_pixel_stride_y);
		GST_DEBUG_OBJECT(filter,"The u  stride:%d    The u pixel stride:%d\n",org_u_stride,org_pixel_stride_u);
		GST_DEBUG_OBJECT(filter,"The v  stride:%d    The v pixel stride:%d\n",org_v_stride,org_pixel_stride_v);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		//pre_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&pre_vframe, 0); // Y plane
		//pre_u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&pre_vframe, 1); // UV plane (interleaved)
		//pre_v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&pre_vframe, 2); // UV plane (interleaved)

		//pre_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&pre_vframe, 0); // Y plane stride
		gint in_stride_y = GST_VIDEO_INFO_PLANE_STRIDE (&pre_video_info, 0);
		g_print("The frame stride:%d the info stride:%d\n",pre_y_stride,in_stride_y);
		//pre_u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&pre_vframe, 1); // UV plane stride
		gint in_stride_u = GST_VIDEO_INFO_PLANE_STRIDE (&pre_video_info, 1);
		g_print("The frame stride:%d the info stride:%d\n",pre_y_stride,in_stride_u);
		//pre_v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&pre_vframe, 2); // UV plane stride
		gint in_stride_v = GST_VIDEO_INFO_PLANE_STRIDE (&pre_video_info, 2);
		g_print("The frame stride:%d the info stride:%d\n",pre_y_stride,in_stride_v);

		//pre_pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&pre_vframe,0);
		gint in_pixel_stride_y = GST_VIDEO_INFO_COMP_PSTRIDE (&pre_video_info, 0);
		g_print("The frame stride pixel:%d the info stride pixel:%d\n",pre_pixel_stride_y,in_pixel_stride_y);
		//pre_pixel_stride_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&pre_vframe,1);
		gint in_pixel_stride_u = GST_VIDEO_INFO_COMP_PSTRIDE (&pre_video_info, 1);
		g_print("The frame stride pixel:%d the info stride pixel:%d\n",pre_pixel_stride_y,in_pixel_stride_u);
		//pre_pixel_stride_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&pre_vframe,2);
		gint in_pixel_stride_v = GST_VIDEO_INFO_COMP_PSTRIDE (&pre_video_info, 2);
		g_print("The frame stride pixel:%d the info stride pixel:%d\n",pre_pixel_stride_y,in_pixel_stride_v);

		//pre_width = GST_VIDEO_FRAME_WIDTH(&pre_vframe);
		//pre_height = GST_VIDEO_FRAME_HEIGHT(&pre_vframe);
		pre_width = GST_VIDEO_INFO_WIDTH(&pre_video_info);
		pre_height = GST_VIDEO_INFO_HEIGHT(&pre_video_info);

		GST_DEBUG_OBJECT(filter,"Strides and caps info for Predefined buffer\n");
		GST_DEBUG_OBJECT(filter,"The width:%d (which is updated in caps)   The height:%d\n",pre_width,pre_height);
		GST_DEBUG_OBJECT(filter,"The y  stride:%d    The y pixel stride:%d\n",pre_y_stride,pre_pixel_stride_y);
		GST_DEBUG_OBJECT(filter,"The u  stride:%d    The u pixel stride:%d\n",pre_u_stride,pre_pixel_stride_u);
		GST_DEBUG_OBJECT(filter,"The v  stride:%d    The v pixel stride:%d\n",pre_v_stride,pre_pixel_stride_v);

		/*---------------------------------------------------------------------------------------------------------------------------------*/

		gint h1=0,w1=0,h2=0,w2=0;

		// Loop through the entire frame
		/*for(h1=0,h2=corner_y;h1<org_height && h2< rect_bottom ;h1++,h2++)
		  {
		  for(w1=0,w2=corner_x;w1<org_width && w2 < rect_right;w1++,w2++)
		  {
		//----------------------This is for own buffer--------------
		//----------------------------------------------------------
		org_y_pixel = org_y_pixels + h1 * org_y_stride + w1 * org_pixel_stride_y;
		org_u_pixel = org_u_pixels + h1 / 2 * org_u_stride + (w1 / 2) * org_pixel_stride_u;
		org_v_pixel = org_v_pixels + h1 / 2 * org_v_stride + (w1 / 2) * org_pixel_stride_v;

		//----------------------This is for predefined buffer--------------
		//----------------------------------------------------------
		pre_y_pixel = pre_y_pixels + h2 * pre_y_stride + w2 * pre_pixel_stride_y;
		pre_u_pixel = pre_u_pixels + h2 / 2 * pre_u_stride + (w2 / 2) * pre_pixel_stride_u;
		pre_v_pixel = pre_v_pixels + h2 / 2 * pre_v_stride + (w2 / 2) * pre_pixel_stride_v;
		if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
		{
		org_y_pixel[0]=pre_y_pixel[0];
		org_u_pixel[0]=pre_u_pixel[0];
		org_v_pixel[0]=pre_v_pixel[0];
		}
		else
		{
		org_y_pixel[0]=0;
		org_u_pixel[0]=128;
		org_v_pixel[0]=128;
		}

		}


		}*/
		
                /*guint8 *in_data=in_map.data;
		guint8 *out_data=out_map.data;
		guint8 *y_data = in_map.data;
		guint8 *u_data = in_map.data + pre_width * pre_height;
		guint8 *v_data = in_map.data + pre_width * pre_height;
		guint8 *oy_data = out_map.data;
		guint8 *ou_data = out_map.data + org_width * org_height;
		guint8 *ov_data = out_map.data + org_width * org_height;*/
		
		/*for(h1=0,h2=corner_y;h1<org_height && h2< rect_bottom ;h1++,h2++)
		   {
		   for(w1=0,w2=corner_x;w1<org_width && w2 < rect_right;w1++,w2++)
		   {

		   if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
		   {
		   guint8 y_pixel = y_data[h2 * pre_width + w2];
		   guint8 u_pixel = u_data[(h2 / 2) * pre_width + 2 * (w2 / 2)];
		   guint8 v_pixel = v_data[(h2 / 2) * pre_width + 2 * (w2 / 2) + 1];
		   
		   
		   //g_print("The y val:%u\n",y_pixel);
		   //g_print("The u val:%u\n",u_pixel);
		   //g_print("The v val:%u\n",v_pixel);


		   oy_data[h1 * org_width + w1]=y_pixel;
		   ou_data[(h1 / 2) * org_width + 2 * (w1 / 2)] = u_pixel;
		   ov_data[(h1 / 2) * org_width + 2 * (w1 / 2) + 1] = v_pixel;
		   
		   //g_print("The y val:%u\n",y_pixel);
		   //g_print("The u val:%u\n",u_pixel);
		   //g_print("The v val:%u\n",v_pixel);


		   }


		   }


	    }*/
	    
	guint8 *y_plane = in_map.data;                           // Y plane
        guint8 *u_plane = y_plane + (pre_width * pre_height);        // U plane
        guint8 *v_plane = u_plane + (pre_width * pre_height) / 4;    // V plane
        guint8 *oy_plane = out_map.data;                           // Y plane
        guint8 *ou_plane = oy_plane + (org_width * org_height);        // U plane
        guint8 *ov_plane = ou_plane + (org_width * org_height) / 4;    // V plane
         // Modify pixel values (e.g., set a solid color)
        guint8 color_y = 20; // Y value (luminance)
        guint8 color_u = 128; // U value (chrominance)
        guint8 color_v = 128; // V value (chrominance)

        // Set a solid color for the Y plane
        memcpy(oy_plane, y_plane, org_width * org_height);

        // Set a solid color for the U plane
        memcpy(ou_plane, u_plane, (org_width * org_height) /4);

        // Set a solid color for the V plane
        memcpy(ov_plane, v_plane, (org_width * org_height) / 4);
	    /*for (int y = 0; y < org_height; y++) {
		  for (int x = 0; x < org_width; x++) {
		  guint8 y_pixel = y_data[y * org_width + x];
		  guint8 u_pixel = u_data[(y / 2) * org_width + 2 * (x / 2)];
		  guint8 v_pixel = v_data[(y / 2) * org_width + 2 * (x / 2) + 1];


		  oy_data[y * org_width + x]=y_pixel;
		  ou_data[(y / 2) * org_width + 2 * (x / 2)] = u_pixel;
		  ov_data[(y / 2) * org_width + 2 * (x / 2) + 1] = v_pixel;




		// Process y_pixel, u_pixel, v_pixel as needed
		}
		}*/
	      // Allocate memory for the cropped frame
              //guint8 *cropped_data = (guint8 *)malloc(org_width * crop_height * 3 / 2); // YV12 format
              
              // Assuming YV12 format: Y plane is first, followed by V and U planes
             
    

		

		GST_DEBUG_OBJECT(filter,"Negotiated caps: %s\n", gst_caps_to_string(org_caps));
		// Set the negotiated caps on the source pad
		gst_pad_set_caps(filter->srcpad, org_caps);
		//gst_video_frame_unmap(&org_vframe);
		gst_buffer_unmap (my_buffer, &out_map);
		gst_buffer_unmap (buf, &in_map);
		//gst_video_frame_unmap(&pre_vframe);



		/* just push out the incoming buffer without touching it */
		return gst_pad_push (filter->srcpad, my_buffer);
	}
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */


/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "gst-plugins-bad"
#endif

/* gstreamer looks for this structure to register croptechs
 *
 * exchange the string 'Template croptech' with your croptech description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		croptech,
		"Crops video into a user defined region",
		croptech_init,
		PACKAGE_VERSION, GST_LICENSE, "Gstreamer Bad plugins (Ubuntu) ", "https://launchpad.net/ubuntu/+source/gst-plugins-bad1.0")
/*-----------GST_PLUGIN_DEFINE is a macro provided by GStreamer for defining a GStreamer plugin.
 * It simplifies the process of registering the plugin with GStreamer's plugin system.
 */
