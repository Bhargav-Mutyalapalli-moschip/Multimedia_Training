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
 * SECTION:element-resolution
 *
 * FIXME:Describe resolution here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! resolution ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

/* The flow of the code is after inheriting the parent class and forming the structure it will  register to the Gstreamer framework
 * 	then base_init() function was invoked
 * 	after that _class_init() function was initialised. here the pointers for object class and element class are created
 * 	and pad templates are registered .
 * 	then _init() function will get invoked where pads are created based on the pad templated registered with framework
 * 	the above is basic steps done until the buffer reached the sink pad of our custom element*/




#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gst/video/gstvideometa.h>
#include <gst/video/gstvideopool.h>
#include <gst/gst.h>
#include <gst/base/base.h>
#include <gst/controller/controller.h>
#include<gst/video/video.h>
#include<stdlib.h>
#include "gstresolution.h"


#define DEF_WIDTH 320
#define DEF_HEIGHT 240
#define DEF_X	0
#define DEF_Y 	0


GST_DEBUG_CATEGORY_STATIC (gst_resolution_debug);  //this macro is used to print the debug information during compilation and it is initialized with 
						   //name ->gst_resolution_debug
#define GST_CAT_DEFAULT gst_resolution_debug	//this helps to use default debug category to be used for logging debug messages within the plugin.

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
	PROP_HEIGHT,
	PROP_WIDTH,
	PROP_X,
	PROP_Y,
	PROP_INT_ARRAY_PROPERTY,
};

/*
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS("ANY")
		);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("ANY")
		);

#define gst_resolution_parent_class parent_class     //parent class is the place where our element is inherited
G_DEFINE_TYPE (GstResolution, gst_resolution, GST_TYPE_ELEMENT); 

/*this macro generates the necessary code to create and initialize the GObject-based class, including the associated class structure and constructor functions.
  first argument is  the name of the custom element
  second argument is the name of the structure that represents instances of your custom element type
  third one represents type of the parent class from our element is derived*/

//DEFINE_TYPE it defines structure of the class which is type of(BASE_SRC ,BASE_SINK). this was inherited from parent class.
//


GST_ELEMENT_REGISTER_DEFINE (resolution, "resolution", GST_RANK_NONE,
		GST_TYPE_RESOLUTION);	//the functionality of this macro function is will not register just define how and what to register our custom plugin				//name , nickmame, rank,type
/* the flow of the code
   first base_init() was called here the element was registerd*/
static void gst_resolution_set_property (GObject * object,
		guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_resolution_get_property (GObject * object,
		guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_resolution_sink_event (GstPad * pad,
		GstObject * parent, GstEvent * event);
static GstFlowReturn gst_resolution_chain (GstPad * pad,
		GstObject * parent, GstBuffer * buf);



static gboolean gst_resolution_query (GstPad    *pad,
		GstObject *parent,
		GstQuery  *query);


/* initialize the resolution's class */
	static void
gst_resolution_class_init (GstResolutionClass * klass)
{
	g_print("class init function was called\n");

	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	GstBaseTransformClass *trans_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;
	trans_class = (GstBaseTransformClass *)klass;
	if(trans_class==NULL)
		g_print("failed\n");
	g_print("value is %p\n",trans_class);


	gobject_class->set_property = gst_resolution_set_property;    //these two are members of object class
								      //a property for the element is also inherited form the object
	gobject_class->get_property = gst_resolution_get_property;



	g_object_class_install_property (gobject_class, PROP_SILENT,
			g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
				FALSE, G_PARAM_READWRITE));

	/*installing our own property width */

	g_object_class_install_property(gobject_class,PROP_WIDTH,
			g_param_spec_int("width","Width","setting video frame width to 1080p",
				1,1920,DEF_WIDTH,G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class,PROP_HEIGHT,
			g_param_spec_int("height","Height","setting video frame height to 1920",
				1,1080, DEF_HEIGHT,G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class,PROP_X,
			g_param_spec_int("x","Xcordinate","x cordinate value to crop the video ",
				0,1920,DEF_X,G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));	

	g_object_class_install_property(gobject_class,PROP_Y,
			g_param_spec_int("y","Ycordinate","y cordinate value to crop the video ",
				0,1080,DEF_Y,G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));	
				
	
    
   

	gst_element_class_set_details_simple (gstelement_class,
			"Resolution",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Kishorey <<user@hostname.org>>");

	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&sink_factory));

	/**calling the function setcaps**/

	/* trans_class->set_caps = 
	   GST_DEBUG_FUNCPTR(gst_resolution_set_caps);
	   g_print("setcaps function from basetransformclass is set\n");*/


}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
	static void
gst_resolution_init (GstResolution * filter)
{
	g_print("init funcn was called\n");
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_resolution_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_resolution_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);



	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);

	/**adding query function to our element**/

	gst_pad_set_query_function(filter->srcpad, GST_DEBUG_FUNCPTR(gst_resolution_query));


	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	g_print("pads are created\n");

	filter->silent = FALSE;
	filter->width = DEF_WIDTH;
	filter->height = DEF_HEIGHT;
	filter->x = DEF_X;
	filter->y = DEF_Y;

	g_print("all the properties are set to default values\n");
}

static void gst_resolution_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	//g_print("set property function was called\n");

	GstResolution *filter = GST_RESOLUTION (object);
	int i;
	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			break;
		case PROP_HEIGHT:
			filter->height = g_value_get_int (value);
			g_print("height value  is set\n");
			break;
		case PROP_WIDTH:
			filter->width = g_value_get_int(value);
			g_print("the value is set to :%d\n",g_value_get_int(value));
			break;
		case PROP_X:
			filter->x = g_value_get_int(value);
			g_print("the value of coordinates are set\n");
			break;
		case PROP_Y:
			filter->y = g_value_get_int(value);
			g_print("the value of y coordinates are set\n");
			break;	
	
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void gst_resolution_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{

	GstResolution *filter = GST_RESOLUTION (object);

	switch (prop_id) {
		case PROP_SILENT:
			g_value_set_boolean (value, filter->silent);
			break;
		case PROP_HEIGHT:
			g_value_set_int (value, filter->height);
			break;
		case PROP_WIDTH:
			g_value_set_int(value,filter->width);
			break;
		case PROP_X:
			g_value_set_int(value,filter->x);
			break;
		case PROP_Y:
			g_value_set_int(value , filter->y);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_resolution_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
//	g_print("sink event is called\n");
	GstResolution *filter;
	gboolean ret;

	filter = GST_RESOLUTION (parent);

//	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
//			GST_EVENT_TYPE_NAME (event), event);

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CAPS:
			{
				GstCaps *caps;
//				g_print("caps event is called\n");

				gst_event_parse_caps (event, &caps);
				ret = gst_pad_event_default (pad, parent, event);
				break;
			}
		default:
			ret = gst_pad_event_default (pad, parent, event);
			break;
	}
	return ret;
}

/** Query function*/
static gboolean gst_resolution_query (GstPad    *pad,GstObject *parent,GstQuery  *query)
{
	//g_print("query function from source was invoked\n");
	gboolean ret;

	switch (GST_QUERY_TYPE (query)) {
		case GST_QUERY_POSITION:
			//g_print("position query\n");
			/* we should report the current position */
			break;
		case GST_QUERY_DURATION:
			//g_print("duriation query\n");
			/* we should report the duration here */

			break;
		case GST_QUERY_CAPS:
			//g_print("caps  query\n");
			/* we should report the supported caps here */
			break;
		default:
			//g_print("nonne query\n");
			/* just call the default handler */
			ret = gst_pad_query_default (pad, parent, query);
			break;
	}
	return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_resolution_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	g_print("------------------------------------------------------------------------\n");
	GstResolution *filter;

	filter = GST_RESOLUTION (parent);

	if(filter->silent == FALSE)
	{
		g_print ("CHAIN I'm plugged, therefore I'm in.\n");
	} 


	//printing all the properties vlaues
	g_print("width of the new buffer :%d \n",filter->width);
	g_print("height of the new buffer :%d \n",filter->height);

	g_print("x cordinate in the orignal frame to crop:%d\n",filter->x);
	g_print("y cordinate in the orignal frame to crop:%d\n",filter->y);

	gint new_buf_width = filter->width;
	gint new_buf_height = filter->height;


	GstBuffer *buffer_new = NULL;
	GstVideoInfo video_info_new;
	GstVideoFrame vframe_new;
	//GstMapInfo map;
	GstCaps *caps = gst_caps_new_simple("video/x-raw",
			"format", G_TYPE_STRING, "YV12",
			"width", G_TYPE_INT, new_buf_width,
			"height", G_TYPE_INT,new_buf_height,
			"bpp", G_TYPE_INT, 12,
			NULL);
			
	/************scaling caps********************/
	GstCaps *scaps = gst_caps_new_simple("video/x-raw",
			"format", G_TYPE_STRING, "YV12",
			"width", G_TYPE_INT, 1920,
			"height", G_TYPE_INT,1080,
			"bpp", G_TYPE_INT, 12,
			NULL);

	gint size = new_buf_width * new_buf_height * (1.5);

// Allocate a buffer with the specified size
	buffer_new= gst_buffer_new_allocate(NULL, size, NULL );
	if (!gst_buffer_make_writable(buffer_new)) {
		g_print("Failed to make the buffer writable\n");
		gst_buffer_unref(buffer_new);
		return -1;
	}

	if(gst_video_info_from_caps(&video_info_new, caps))
	{
		g_print("Successfully videoinfo is mapped\n");
	}
	else
	{
		g_print("failure videoinfo is mapped\n");
	}

	if(gst_video_frame_map(&vframe_new, &video_info_new, buffer_new, GST_MAP_WRITE))
	{
		g_print("Successfully videoframe is mapped\n");
	}
	else
	{
		g_print("failure videoframe is mapped\n");
	}


	guint8 *y_pixels_new = GST_VIDEO_FRAME_PLANE_DATA(&vframe_new, 0); // Y plane
	guint8 *u_pixels_new = GST_VIDEO_FRAME_PLANE_DATA(&vframe_new, 1); // U plane (interleaved)
	guint8 *v_pixels_new = GST_VIDEO_FRAME_PLANE_DATA(&vframe_new, 2);  //V plane

	guint y_stride_new = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_new, 0); // Y plane stride
	guint u_stride_new = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_new, 1); //U plane
	guint v_stride_new = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_new, 2);// V plane stride

	guint pixel_stride_new = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe_new, 0); 
	guint pixel_stride_new_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe_new, 1);
	guint pixel_stride_new_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe_new, 2);

	gint width_new = GST_VIDEO_FRAME_WIDTH(&vframe_new);
	gint height_new = GST_VIDEO_FRAME_HEIGHT(&vframe_new);

	/********************************mapping the original buffer ***************************************************************/
	GstVideoInfo video_info_org;
	GstVideoFrame vframe_org;

	if(gst_video_info_from_caps(&video_info_org, gst_pad_get_current_caps(pad)))
	{
		g_print("Successfully videoinfo original buff is mapped\n");
	}
	else
	{
		g_print("failure videoinfo of original is mapped\n");
	}
	if(gst_video_frame_map(&vframe_org, &video_info_org, buf, GST_MAP_READ))
	{
		g_print("Successfully videoframe of org is mapped\n");
	}
	else
	{
		g_print("failure videoframe of org is mapped\n");
	}

	guint8 *y_pixels_org = GST_VIDEO_FRAME_PLANE_DATA(&vframe_org, 0); // Y plane
	guint8 *u_pixels_org = GST_VIDEO_FRAME_PLANE_DATA(&vframe_org, 1); // U plane (interleaved)
	guint8 *v_pixels_org = GST_VIDEO_FRAME_PLANE_DATA(&vframe_org, 2);  //V plane

	guint y_stride_org = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_org, 0); // Y plane stride
	guint u_stride_org = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_org, 1); //U plane
	guint v_stride_org = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe_org, 2);// V plane stride

	guint pixel_stride_org = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe_org, 0); 
	guint pixel_stride_org_u = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe_org, 1);
	guint pixel_stride_org_v = GST_VIDEO_FRAME_COMP_PSTRIDE(&vframe_org, 2);

	gint width_org = GST_VIDEO_FRAME_WIDTH(&vframe_org);
        gint height_org = GST_VIDEO_FRAME_HEIGHT(&vframe_org);


	gint corner_x = filter->x;
	gint corner_y = filter->y;
	g_print("corner x:%d    corner y:%d\n",corner_x,corner_y);

	// Calculate the coordinates of the top-left and bottom-right corners
	gint rect_bottom = corner_y + new_buf_height;
        gint rect_right = corner_x + new_buf_width;
	g_print("rect bot:%d    rect right:%d\n",rect_bottom,rect_right);
	
	guint8 *y_pixel,*u_pixel,*v_pixel,*p_y_pixel,*p_u_pixel,*p_v_pixel;
	gint h1=0,w1=0,h2=0,w2=0;
		// Loop through the entire frame
		for(h1=0,h2=corner_y;h1<new_buf_height && h2< rect_bottom ;h1++,h2++)
		{
			for(w1=0,w2=corner_x;w1<new_buf_width && w2 < rect_right;w1++,w2++)
			{
				//----------------------This is for own buffer--------------
				//----------------------------------------------------------
				y_pixel = y_pixels_new + h1 * y_stride_new + w1 * pixel_stride_new;
				u_pixel = u_pixels_new + h1 / 2 * u_stride_new + (w1 / 2) * pixel_stride_new_u;
				v_pixel = v_pixels_new + h1 / 2 * v_stride_new + (w1 / 2) * pixel_stride_new_v;
				//----------------------This is for predefined buffer--------------
				//----------------------------------------------------------
				p_y_pixel = y_pixels_org + h2 * y_stride_org + w2 * pixel_stride_org;
				p_u_pixel = u_pixels_org + h2 / 2 * u_stride_org + (w2 / 2) * pixel_stride_org_u;
				p_v_pixel = v_pixels_org + h2 / 2 * v_stride_org + (w2 / 2) * pixel_stride_org_v;
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
	/*GstSample *samp = gst_sample_new (buffer_new,caps,NULL,NULL);
 		if(!samp)
 		{
 			g_print("failed to create the sample\n");
 		}
 		GstSample *sample = gst_video_convert_sample(samp,scaps,GST_CLOCK_TIME_NONE ,NULL);
 		if(!sample)
 		{
 			g_print("failed to convert the sample\n");
 		}
 		
 		GstBuffer *mybuf = gst_sample_get_buffer(sample);*/

	//gst_pad_set_caps(filter->srcpad , scaps);

	gst_pad_set_caps(filter->srcpad , caps);

	gst_video_frame_unmap(&vframe_new);	

	return gst_pad_push (filter->srcpad, buffer_new);
	//return gst_pad_push (filter->srcpad, mybuf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean resolution_init (GstPlugin * resolution)
{
	g_print("resolution init\n");
	/* debug category for filtering log messages					//this is first to invoke in this element was registered
	 *
	 * exchange the string 'Template resolution' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_resolution_debug, "resolution",
			0, "Template resolution");
	//it is the entry point for the plugin .
	return GST_ELEMENT_REGISTER (resolution, resolution);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstresolution"
#endif

/* gstreamer looks for this structure to register resolutions
 *
 * exchange the string 'Template resolution' with your resolution description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		resolution,
		"resolution",
		resolution_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
