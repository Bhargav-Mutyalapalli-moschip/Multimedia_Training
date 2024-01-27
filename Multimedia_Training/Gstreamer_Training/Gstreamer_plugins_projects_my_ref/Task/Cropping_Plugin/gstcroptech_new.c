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
	PROP_FORMAT,            //it is filled with six
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
                                         "width = (int) [1, 2147483647], "
                                         "height = (int) [1, 2147483647], "
                                         "framerate = (fraction) [0/1, 2147483647/1], "
                                         "format = (string) { I420, NV12, NV21, YV12,RGB16,NV16,NV24,\
					 RGBx, xRGB, BGRx, xBGR, RGBA, ARGB,BGRA, ABGR, RGB, BGR, AYUV, YUY2, \
					 YVYU, UYVY, I420, YV12, RGB16, RGB15, GRAY8, NV12, NV21,GRAY16_LE, GRAY16_BE }"));
//It creates the sink template for my plugin
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("video/x-raw, "
                                         "width = (int) [1, 2147483647], "
                                         "height = (int) [1, 2147483647], "
                                         "framerate = (fraction) [0/1, 2147483647/1], "
                                         "format = (string) { I420, NV12, NV21, YV12,RGB16,NV16,NV24,\
					 RGBx, xRGB, BGRx, xBGR, RGBA, ARGB,BGRA, ABGR, RGB, BGR, AYUV, YUY2, \
					 YVYU, UYVY, I420, YV12, RGB16, RGB15, GRAY8, NV12, NV21,GRAY16_LE, GRAY16_BE }"));


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
	g_print("First entry point of every plugin:plugin_init\n");
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
	g_print("Second entry point of every plugin:class init\n");
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
	g_object_class_install_property (gobject_class, PROP_FORMAT,g_param_spec_string ("format", "format", "Set the format here ","YV12", G_PARAM_READWRITE));

	/*---If you are writing a simple plugin details or metadata is important to specify the author and other info
	 * which is nontechnical info and technical info.
	 */
	gst_element_class_set_details_simple (gstelement_class,
			"Croptech",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Pranav <<user@hostname.org>>");

	/*-----This function is used to define the input and output pad templates that an element class supports.---*/
	//it add the src template to the element class
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&src_factory));

	//it add the sink template to the element class
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&sink_factory));
}

//Third entry point of every plugin
static void gst_croptech_init (GstCroptech * filter)
{
	g_print("Third entry point of every plugin:gst_plugin_init\n");

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
	strcpy(filter->format,"YV12");
}

//Fourth Entry point of every plugin(If you set the properties dynamically)
static void gst_croptech_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	g_print("gst_custom set property  is invoked\n");
	//It retrieves the elements data from object
	GstCroptech *filter = GST_CROPTECH (object);

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			break;
		case PROP_WIDTH:
			filter->width = g_value_get_int (value);
			break;
		case PROP_HEIGHT:
			filter->height = g_value_get_int (value);
			break;
		case PROP_XCO:
			filter->xco = g_value_get_int (value);
			break;
		case PROP_YCO:
			filter->yco = g_value_get_int (value);
			break;
		case PROP_FORMAT:
			strcpy(filter->format,g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
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
		case PROP_FORMAT:
			g_value_set_string (value, filter->format);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

//Sink event handling function
static gboolean gst_croptech_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
	g_print("gst_custom sink event  is invoked\n");
	GstCroptech *filter;
	gboolean ret;

	//It retrieves the elements data from object
	filter = GST_CROPTECH (parent);

	//For debug info to genarate the log file
	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,GST_EVENT_TYPE_NAME (event), event);
	g_print ("Received %s event: %" GST_PTR_FORMAT,GST_EVENT_TYPE_NAME (event), event);
	g_print("\n");

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CAPS:
			{
				GstCaps *caps;

				//It is used to extract the capabilities (caps) information from a GStreamer event.
				gst_event_parse_caps (event, &caps);

				/* do something with the caps */
				//convert our caps to string for human readable format
				gchar *caps_str = gst_caps_to_string(caps);
				g_print("Received caps: %s\n", caps_str);
				g_free(caps_str); //deallocate the memory for caps_str
				/* and forward */

				//It is used to perform the default handling of a GStreamer event by a pad in a GStreamer element.
				ret = gst_pad_event_default (pad, parent, event);
				break;
			}
		default:
			g_print("default event is handling\n");
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

	o_width=filter->width;
	o_height=filter->height;
	g_print("Format: %s\n", filter->format);

	/*----Retrieving the predfined caps------------------------------*/
	p_caps=gst_pad_get_current_caps(pad);
	
	/*--------------------Predefined buffer--------------------------*/
	if(gst_video_info_from_caps(&p_video_info, p_caps))
	{
		g_print("Successfully videoinfo is taken and updated predefined caps\n");
	}
	else
	{
		g_print("failure videoinfo is mapped in predefined buffer\n");
	}

	
	/*-----Retrieving the format macro number from the video info structure-------*/
	o_format_no=p_video_info.finfo->format;

	/*--------------------------------Video Frame size calculations:start-----------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	switch(o_format_no)
	{
		case GST_VIDEO_FORMAT_NV12:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height)/4;
				o_v_size=(o_width * o_height)/4;
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				o_bpp=12;
				denom1=2;
				denom2=2;
				strcpy(filter->format,"NV12");
				break;
			}
		case GST_VIDEO_FORMAT_YV12:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height)/4;
				o_v_size=(o_width * o_height)/4;
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"YV12");
				o_bpp=12;
				break;
			}

		case GST_VIDEO_FORMAT_NV21:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height)/4;
				o_v_size=(o_width * o_height)/4;
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				denom1=2;
				denom2=2;
				strcpy(filter->format,"NV21");
				o_bpp=12;
				break;
			}

		case GST_VIDEO_FORMAT_NV16:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height)/2;
				o_v_size=(o_width * o_height)/2;
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				denom1=1;
				denom2=2;
				strcpy(filter->format,"NV16");
				o_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_NV24:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				denom1=1;
				denom2=1;
				strcpy(filter->format,"NV24");
				o_bpp=24;
				break;
			}
		case GST_VIDEO_FORMAT_RGB:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"RGB");
				o_bpp=24;
				ind=0;
				break;
			}
		case GST_VIDEO_FORMAT_BGR:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"BGR");
				o_bpp=24;
				break;
			}

		case GST_VIDEO_FORMAT_GRAY8:
			{
				o_y_size=(o_width * o_height);
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"GRAY8");
				o_bpp=8;
				break;
			}
		case GST_VIDEO_FORMAT_RGBx:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"RGBx");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_xRGB:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=3;
				strcpy(filter->format,"xRGB");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_BGRx:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"BGRx");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_xBGR:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=3;
				strcpy(filter->format,"xBGR");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_RGBA:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"RGBA");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_ARGB:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=3;
				strcpy(filter->format,"ARGB");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_BGRA:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"BGRA");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_ABGR:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=3;
				strcpy(filter->format,"ABGR");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_RGB16:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"RGB16");
				o_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_RGB15:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"RGB15");
				o_bpp=15;
				ind=0;
				break;
			}
		case GST_VIDEO_FORMAT_I420:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height)/4;
				o_v_size=(o_width * o_height)/4;
				o_size=o_y_size + o_u_size + o_v_size;
				g_print("Buffer creating size:%d\n",o_size);
				denom1=2;
				denom2=2;
				strcpy(filter->format,"I420");
				o_bpp=12;
				break;
			}
		case GST_VIDEO_FORMAT_GRAY16_LE:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"GRAY16_LE");
				o_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_GRAY16_BE:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"GRAY16_BE");
				o_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_AYUV:
			{
				o_y_size=(o_width * o_height);
				o_u_size=(o_width * o_height);
				o_v_size=(o_width * o_height);
				o_e_size=(o_width * o_height);
				o_size=o_y_size + o_u_size + o_v_size + o_e_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=3;
				strcpy(filter->format,"AYUV");
				o_bpp=32;
				break;
			}
		case GST_VIDEO_FORMAT_YUY2:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				ind=0;
				strcpy(filter->format,"YUY2");
				o_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_YVYU:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"YVYU");
				o_bpp=16;
				break;
			}
		case GST_VIDEO_FORMAT_UYVY:
			{
				o_y_size=(o_width * o_height)*2;
				o_size=o_y_size;
				g_print("Buffer creating size:%d\n",o_size);
				strcpy(filter->format,"UYVY");
				o_bpp=16;
				break;
			}
		default:
			g_print("Please give valid format\n");
			break;
	}
	/*----------------Caps for my own buffer -----------------*/
	// Create a caps structure to describe the frame format
	o_caps = gst_caps_new_simple("video/x-raw",
			"format", G_TYPE_STRING,filter->format,
			"width", G_TYPE_INT, o_width,
			"height", G_TYPE_INT, o_height,
			"bpp",G_TYPE_INT,o_bpp,
			NULL);

	/*----------------------------------Video info from caps:start-------------------------------------------*/
	/*--------------------own buffer--------------------------*/
	/*-------------------------------------------------------*/
	if(gst_video_info_from_caps(&o_video_info, o_caps))
	{
		g_print("Successfully videoinfo is taken and updated own caps\n");
	}
	else
	{
		g_print("failure videoinfo is mapped in own buffer\n");
	}
	/*----------------------------------Video info from caps:end-------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/*--------------------------------Video Frame size calculations:end------------------------------------*/

	/*------------------------------------Corner points calculations:start---------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	gint corner_x = filter->xco;
	gint corner_y = filter->yco;
	g_print("corner x:%d    corner y:%d\n",corner_x,corner_y);

	// Calculate the coordinates of the top-left and bottom-right corners
	gint rect_bottom = corner_y + o_height;
	gint rect_right = corner_x + o_width;
	g_print("rect bot:%d    rect right:%d\n",rect_bottom,rect_right);

	/*-----------------------------------------------------------------------------------------------------*/
	/*------------------------------------Corner points calculations:end----------------------------------*/

	/*--------------------------------------Buffer creation:start----------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/
	// Allocate a buffer with the specified size
	my_buffer= gst_buffer_new_allocate(NULL, o_size, NULL );
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
	gst_buffer_memset(my_buffer,0,0,o_size); //it clears the garbage values in created buffer

	/*---------------------------------------------------------------------------------------------------*/
	/*--------------------------------------Buffer creation:end------------------------------------------*/

	/*--------------------------------------Video frame mapping:start------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/
	//Predefined buffer video map
	if(gst_video_frame_map(&p_vframe, &p_video_info, buf, GST_MAP_READ))
	{
		g_print("Successfully videoframe is mapped for predefined buffer\n");
	}
	else
	{
		g_print("failure videoframe is mapped for predefined buffer\n");
	}

	//Own buffer video map
	if(gst_video_frame_map(&o_vframe, &o_video_info, my_buffer, GST_MAP_WRITE))
	{
		g_print("Successfully videoframe is mapped for own buffer\n");
	}
	else
	{
		g_print("failure videoframe is mapped for own buffer\n");
	}
	/*---------------------------------------------------------------------------------------------------*/
	/*--------------------------------------Video frame mapping:end--------------------------------------*/


	/*--------------------------------------Errors checking:start----------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/
	p_width = GST_VIDEO_FRAME_WIDTH(&p_vframe); //it retrieves the predefined width
	p_height = GST_VIDEO_FRAME_HEIGHT(&p_vframe); //it retrieves the predefined height
	gint planes=GST_VIDEO_FRAME_N_PLANES(&o_vframe); //it retrieves the no of planes 
	g_print("The no of planes :%d\n",planes);

	if(rect_right > p_width || rect_bottom > p_height)
	{
		g_warning("exceeded caps\n");
		gst_video_frame_unmap(&o_vframe);
		gst_video_frame_unmap(&p_vframe);
		exit(1);
	}
	/*---------------------------------------------------------------------------------------------------*/
	/*--------------------------------------Errors checking:end------------------------------------------*/

	/*--------------------------------------Main cropping logic:start------------------------------------*/
	/*---------------------------------------------------------------------------------------------------*/

	if(planes == 1)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		o_n_pixels = GST_VIDEO_FRAME_PLANE_DATA(&o_vframe, 0); // Y plane

		o_rgb_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&o_vframe, 0); // Y plane stride

		o_pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&o_vframe,0);

		o_width = GST_VIDEO_FRAME_WIDTH(&o_vframe);
		o_height = GST_VIDEO_FRAME_HEIGHT(&o_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for own buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",o_width,o_height);
		g_print("The y stride:%d    The y pixel stride:%d\n",o_rgb_stride,o_pixel_stride);


		/*-------------------Getting strides for predefined buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		p_n_pixels = GST_VIDEO_FRAME_PLANE_DATA(&p_vframe, 0); // Y plane

		p_rgb_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&p_vframe, 0); // Y plane stride

		p_pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(&p_vframe,0);

		p_width = GST_VIDEO_FRAME_WIDTH(&p_vframe);
		p_height = GST_VIDEO_FRAME_HEIGHT(&p_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for predefined buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",p_width,p_height);
		g_print("The rgb stride:%d    The rgb pixel stride:%d\n",p_rgb_stride,p_pixel_stride);




		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<o_height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<o_width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				o_rgb_pixel = o_n_pixels + h1 * o_rgb_stride + w1 * o_pixel_stride ;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_rgb_pixel = p_n_pixels + h2 * p_rgb_stride + w2 * p_pixel_stride ;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
				{
					if(o_format_no == GST_VIDEO_FORMAT_GRAY8)
					{
						o_rgb_pixel[0]=p_rgb_pixel[0];
					}
					else if( o_format_no == GST_VIDEO_FORMAT_GRAY16_BE || o_format_no == GST_VIDEO_FORMAT_GRAY16_LE)
					{
						o_rgb_pixel[0]=p_rgb_pixel[0];
						o_rgb_pixel[1]=p_rgb_pixel[1];
					}
					else if( o_format_no == GST_VIDEO_FORMAT_UYVY || o_format_no == GST_VIDEO_FORMAT_YVYU)
					{


						o_rgb_pixel[0]=p_rgb_pixel[0];
						o_rgb_pixel[1]=p_rgb_pixel[1];
						o_rgb_pixel[2]=p_rgb_pixel[2];
						o_rgb_pixel[3]=p_rgb_pixel[3];
					}
					else
					{


						o_rgb_pixel[ind]=p_rgb_pixel[ind];
						o_rgb_pixel[1]=p_rgb_pixel[1];
						o_rgb_pixel[2]=p_rgb_pixel[2];
					}

				}
				else
				{
					o_rgb_pixel[0]=0;
					o_rgb_pixel[1]=128;
					o_rgb_pixel[2]=128;
				}

			}


		}
	}

	if(planes == 2)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		o_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&o_vframe, 0); // Y plane
		o_uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&o_vframe, 1); // UV plane (interleaved)

		o_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&o_vframe, 0); // Y plane stride
		o_uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&o_vframe, 1); // UV plane stride
		o_pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

		o_width = GST_VIDEO_FRAME_WIDTH(&o_vframe);
		o_height = GST_VIDEO_FRAME_HEIGHT(&o_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for own buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",o_width,o_height);
		g_print("The y stride:%d    The y pixel stride:%d\n",o_y_stride,o_pixel_stride);
		g_print("The uv  stride:%d    The uv pixel stride:%d\n",o_uv_stride,o_pixel_stride);

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


		gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<o_height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<o_width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				o_y_pixel = o_y_pixels + h1 * o_y_stride + w1;
				o_uv_pixel = o_uv_pixels + (h1/denom1)  * o_uv_stride + (w1/denom2) * o_pixel_stride;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_y_pixel = p_y_pixels + h2 * p_y_stride + w2;
				p_uv_pixel = p_uv_pixels + (h2/denom1) * p_uv_stride + (w2/denom2 ) * p_pixel_stride;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
				{
					o_y_pixel[0]=p_y_pixel[0];
					o_uv_pixel[0]=p_uv_pixel[0];
					o_uv_pixel[1]=p_uv_pixel[1];
				}
				else
				{
					o_y_pixel[0]=0;
					o_uv_pixel[0]=128;
					o_uv_pixel[1]=128;
				}

			}


		}
	}
	if(planes == 3)
	{
		/*-------------------Getting strides for own buffer---------------------------*/
		/*-----------------------------------------------------------------------------*/
		o_y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&o_vframe, 0); // Y plane
		o_u_pixels = GST_VIDEO_FRAME_PLANE_DATA(&o_vframe, 1); // UV plane (interleaved)
		o_v_pixels = GST_VIDEO_FRAME_PLANE_DATA(&o_vframe, 2); // UV plane (interleaved)

		o_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&o_vframe, 0); // Y plane stride
		o_u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&o_vframe, 1); // UV plane stride
		o_v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&o_vframe, 2); // UV plane stride

		o_pixel_stride_y = GST_VIDEO_FRAME_COMP_PSTRIDE(&o_vframe,0);
		o_pixel_stride_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&o_vframe,1);
		o_pixel_stride_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&o_vframe,2);

		o_width = GST_VIDEO_FRAME_WIDTH(&o_vframe);
		o_height = GST_VIDEO_FRAME_HEIGHT(&o_vframe);

		g_print("/*------------------------------------------------------*/\n");
		g_print("Strides and caps info for own buffer\n");
		g_print("The width:%d (which is updated in caps)   The height:%d\n",o_width,o_height);
		g_print("The y stride:%d    The y pixel stride:%d\n",o_y_stride,o_pixel_stride_y);
		g_print("The u  stride:%d    The u pixel stride:%d\n",o_u_stride,o_pixel_stride_u);
		g_print("The v  stride:%d    The v pixel stride:%d\n",o_v_stride,o_pixel_stride_v);

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
		for(h1=0,h2=corner_y;h1<o_height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<o_width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				o_y_pixel = o_y_pixels + h1 * o_y_stride + w1 * o_pixel_stride_y;
				o_u_pixel = o_u_pixels + h1 / 2 * o_u_stride + (w1 / 2) * o_pixel_stride_u;
				o_v_pixel = o_v_pixels + h1 / 2 * o_v_stride + (w1 / 2) * o_pixel_stride_v;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_y_pixel = p_y_pixels + h2 * p_y_stride + w2 * p_pixel_stride_y;
				p_u_pixel = p_u_pixels + h2 / 2 * p_u_stride + (w2 / 2) * p_pixel_stride_u;
				p_v_pixel = p_v_pixels + h2 / 2 * p_v_stride + (w2 / 2) * p_pixel_stride_v;
				if (w2 >= corner_x && w2 < rect_right && h2 >= corner_y && h2 < rect_bottom)
				{
					o_y_pixel[0]=p_y_pixel[0];
					o_u_pixel[0]=p_u_pixel[0];
					o_v_pixel[0]=p_v_pixel[0];
				}
				else
				{
					o_y_pixel[0]=0;
					o_u_pixel[0]=128;
					o_v_pixel[0]=128;
				}

			}


		}
	}

	g_print("Negotiated caps: %s\n", gst_caps_to_string(o_caps));
	// Set the negotiated caps on the source pad
	gst_pad_set_caps(filter->srcpad, o_caps);
	gst_pad_use_fixed_caps (filter->srcpad);
	gst_video_frame_unmap(&o_vframe);
	gst_video_frame_unmap(&p_vframe);



	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, my_buffer);
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
#define PACKAGE "myfirstcroptech"
#endif

/* gstreamer looks for this structure to register croptechs
 *
 * exchange the string 'Template croptech' with your croptech description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		croptech,
		"croptech",
		croptech_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
/*-----------GST_PLUGIN_DEFINE is a macro provided by GStreamer for defining a GStreamer plugin.
 * It simplifies the process of registering the plugin with GStreamer's plugin system.
 */
