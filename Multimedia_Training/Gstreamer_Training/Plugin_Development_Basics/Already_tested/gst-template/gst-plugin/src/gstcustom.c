//Own header file
#include "gstcustom.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>


//creating category for own plugin which is used for debugging info
GST_DEBUG_CATEGORY_STATIC (gst_custom_debug);
#define GST_CAT_DEFAULT gst_custom_debug



/*----------------For signals and adding properties----------*/
/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};
//the below enum is for properties
enum
{
	PROP_0,         //it is filled with zero
	PROP_SILENT,     //it is filled with one
	PROP_LAYER,      //it is filled with two
	PROP_WORD	//it is filled with three
};
/*------------------------------*/
// Define the GType for the pattern enumeration
#define GST_TYPE_CUSTOM_LAYER (gst_custom_layer_get_type ())

/*-----------defining the static pad templates-----------*/
//It creates the src template for my plugin
static GstStaticPadTemplate src_factory =GST_STATIC_PAD_TEMPLATE("src",GST_PAD_SRC,GST_PAD_ALWAYS,GST_STATIC_CAPS("ANY"));
//It creates the sink template for my plugin
static GstStaticPadTemplate sink_factory =GST_STATIC_PAD_TEMPLATE("sink",GST_PAD_SINK,GST_PAD_ALWAYS,GST_STATIC_CAPS("ANY"));

/*------substitute the parent_class in place of own variable gst_custom_parent_class----*/
#define gst_custom_parent_class parent_class

/*-------It sets up the type hierarchy, initializes function pointers,*/
//and provides default implementations for object initialization and class initialization.
G_DEFINE_TYPE(GstCustom, gst_custom, GST_TYPE_ELEMENT);

/*--------------it is used to define a new GStreamer element--------*/
GST_ELEMENT_REGISTER_DEFINE (custom, "custom-plugin", GST_RANK_NONE,GST_TYPE_CUSTOM);

//function signatures
static void gst_custom_set_property (GObject * object,guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_custom_get_property (GObject * object,guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean gst_custom_sink_event (GstPad * pad,GstObject * parent, GstEvent * event);
static GstFlowReturn gst_custom_chain (GstPad * pad,GstObject * parent, GstBuffer * buf);
static GstStateChangeReturn gst_custom_change_state (GstElement *element, GstStateChange transition);
//gpointer gst_custom_allocate_memory(gsize size);
//void gst_custom_free_memory(gpointer mem);



/*--------------------------------------------------------------------------*/
/*----------------------------------FUNCTIONS---------------------------------*/
/*--------Every plugin entry point-----*/
static gboolean custom_init (GstPlugin * custom)
{
	g_print("First Entry point of every plugin\n");
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template custom' with your description
	 */
	//It initializes the debug info through created category variable
	GST_DEBUG_CATEGORY_INIT (gst_custom_debug, "custom",0, "Template custom");
	//It register the element which is defined
	return GST_ELEMENT_REGISTER (custom,custom);
}


/*-----------------------------------------------------------------------*/
/*---------------Second entry point of every plugin----------------------*/
static void gst_custom_class_init (GstCustomClass * klass)
{
	//g_print("Second entry point of every plugin:class init\n");
	//It is the GObjectClass  variable is a pointer to the class structure of a GObject-derived class. 
	/*It represents the blueprint or definition of a class and contains function pointers, class-specific data, 
	 * and other information necessary for working with instances of that class
	 *
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
	gobject_class->set_property = gst_custom_set_property;
	//It is initializing the own get property function base address to generic gobject class get property
	gobject_class->get_property = gst_custom_get_property;

	//It is initializing the own change state function base address to generic element class change state
	 gstelement_class->change_state = gst_custom_change_state;


	/*---If you want write your own proprty for your custom plugin then,you have install the property
	 * by using g_object_class_install_property you are going to install property
	 * In the property you have input may boolen,int char can be defined by g_param_sepc_(input type)
	 */
	g_object_class_install_property (gobject_class, PROP_SILENT,g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",FALSE, G_PARAM_READWRITE));
	//My own property
	g_object_class_install_property (gobject_class, PROP_LAYER,g_param_spec_enum("layer", "layer-plugin", "Normal checking for integer",GST_TYPE_CUSTOM_LAYER,GST_CUSTOM_SMPTE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));


	g_object_class_install_property (gobject_class, PROP_WORD,g_param_spec_string("word", "word-plugin", "Normal checking for string","Pranav", G_PARAM_READWRITE));

	/*---If you are writing a simple plugin details or metadata is important to specify the author and other info 
	 * which is nontechnical info and technical info
	 */
	gst_element_class_set_details_simple (gstelement_class,
			"Custom",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Bhargav Mutyalapalli <<mbrpranav@hal.com>>");

	/*-----This function is used to define the input and output pad templates that an element class supports.---*/
	//it add the src template to the element class 
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&src_factory));
	//it add the sink template to the element class 
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&sink_factory));
}

/*---------------------------------------------------------------------*/
/*------------------Third entry point of every plugin*-----------------*/
static void gst_custom_init (GstCustom *filter)
{
	//g_print("Third entry point of every plugin:gst_custom_init\n");
	 /*-----------PAD RETRIEVAL-----------------------*/
	//it retrieves the src pad from static pad templates
	filter->srcpad=gst_pad_new_from_static_template(&src_factory,"src");
	//it retrieves the sink pad from static pad templates
	filter->sinkpad=gst_pad_new_from_static_template(&sink_factory,"sink");

	/*--------------event function callback creation-------------------*/
	// Set the event handling callback function for the sink pad
	gst_pad_set_event_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_custom_sink_event));
	/*----GStreamer provides a mechanism for handling events on pads.
	 * Various types of events, such as EOS (End of Stream), flush, and custom application-
	 * specific events, can be sent and received through pads. Handling events can be crucial
	 * for proper pipeline behavior and synchronization.
	 */

	/*--------------chain function callback creation-------------------*/
	// Set the chaining callback function for the sink pad
	gst_pad_set_chain_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_custom_chain));
	/*---It is used to set a callback function that gets called when data buffers are pushed or 
	 * "chained" through a GStreamer pad.
	 */

	/*------------- query function callback creation-------------------*/
	// Set the query callback function for the src pad
	//gst_pad_set_query_function (filter->srcpad,GST_DEBUG_FUNCPTR(gst_custom_src_query));
	/*. These are queries like position, duration but also about the supported formats and 
	 * scheduling modes your element supports. Queries can travel both upstream and downstream, 
	 * so you can receive them on sink pads as well as source pads.
	 */

	/*---------Set the pads to proxy pads-------*/
	//set the src pads
	GST_PAD_SET_PROXY_CAPS(filter->srcpad);
	//set the sink pads
	GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
	/*----it automatically adopts the capabilities of the source pad 
	 * it is connected to, ensuring that data flow and processing remain consistent 
	 * and well-aligned throughout the pipeline.
	 */

	/*------------Add the pads to the elements------*/
	// Add the source pad to the element
	gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);
	// Add the sink pad to the element
	gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

	filter->silent=FALSE;
	strcpy(filter->word,"Pranav");

}

/*------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------Fourth Entry point of every plugin(If you set the properties dynamically)-------------------------------*/
static void gst_custom_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	//g_print("gst_custom set property  is invoked\n");
	//It retrieves the elements data from object
	GstCustom *filter = GST_CUSTOM (object);

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			break;
		case PROP_LAYER:
			filter->layer = g_value_get_enum (value);
			break;
		case PROP_WORD:
			//filter->layer = g_value_get_int (value);
			strcpy(filter->word, g_value_get_string (value));
			g_print("The stored str:%s\n",filter->word);
			g_print("The passed str:%s\n",g_value_get_string(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------Fifth Entry point of every plugin(If you request the properties)-------------------------------*/
static void gst_custom_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
	//g_print("gst_custom get property  is invoked\n");
	//It retrieves the elements data from object
	GstCustom *filter = GST_CUSTOM (object);

	switch (prop_id) 
	{
		case PROP_SILENT:
			g_value_set_boolean (value, filter->silent);
			break;
		case PROP_LAYER:
			g_value_set_enum (value, filter->layer);
			break;
		case PROP_WORD:
			g_value_set_string (value, filter->word);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}

}


/* GstElement vmethod implementations */

/*----------------------------------------------------------------*/
/*----------------------Element change state-----------------------*/
/*static GstStateChangeReturn gst_custom_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstCustom *filter = GST_CUSTOM (element);

	switch (transition) {
		case GST_STATE_CHANGE_NULL_TO_READY:
			g_print("Element class :state change from NULL to READY\n");
			//if (!gst_custom_allocate_memory (filter))
				//return GST_STATE_CHANGE_FAILURE;
			break;
		default:
			break;
	}
	ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE)
		return ret;

	switch (transition) {
		case GST_STATE_CHANGE_READY_TO_NULL:
			g_print("Element class :state change from READY to NULL\n");
			//gst_custom_free_memory (filter);
			break;
		default:
			break;
	}
	return ret;
}*/
static GstStateChangeReturn gst_custom_change_state(GstElement *element, GstStateChange transition) 
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstCustom *filter = GST_CUSTOM(element);

	switch (transition) {
		case GST_STATE_CHANGE_NULL_TO_READY:
			// Allocate resources or perform initialization when transitioning to READY state
			// If allocation fails, return GST_STATE_CHANGE_FAILURE
			g_print("Element class :state change from NULL to READY\n");
			break;
		case GST_STATE_CHANGE_READY_TO_PAUSED:
			// Handle the transition from READY to PAUSED (e.g., start processing data)
			g_print("Element class :state change from READY to PAUSED\n");
			break;
		case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
			// Handle the transition from PAUSED to PLAYING (e.g., start playback)
			g_print("Element class :state change from PAUSED to PLAYING\n");
			break;
		case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
			// Handle the transition from PLAYING to PAUSED (e.g., pause playback)
			g_print("Element class :state change from PLAYING to PAUSED\n");
			break;
		case GST_STATE_CHANGE_PAUSED_TO_READY:
			// Handle the transition from PAUSED to READY (e.g., stop processing data)
			g_print("Element class :state change from PAUSED to READY\n");
			break;
		case GST_STATE_CHANGE_READY_TO_NULL:
			// Free allocated resources when transitioning from READY to NULL state
			g_print("Element class :state change from READY to NULL\n");
			break;
		default:
			break;
	}

	// Call the parent class's change_state function to handle state changes
	ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		return ret;
	}

	return ret;
}

/*-----------------------------------------*/
// Function to allocate memory
/*gpointer gst_custom_allocate_memory(gsize size) {
  return g_malloc(size);
  }*/

/*----------------------------------------*/
// Function to free memory
/*void gst_custom_free_memory(gpointer mem) {
  g_free(mem);
  }*/

/*---------------------------------------------*/
/*-----------------Testing properties-----------*/
static GType gst_custom_layer_get_type (void)
{
  static GType custom_layer_type = 0;

  if (!custom_layer_type) {
    static GEnumValue layer_types[] = {
      { GST_CUSTOM_SMPTE, "SMPTE 100% color bars",    "smpte" },
      { GST_CUSTOM_SNOW,  "Random (television snow)", "snow"  },
      { GST_CUSTOM_BLACK, "0% Black",                 "black" },
      { 0, NULL, NULL },
    };

    custom_layer_type =
    g_enum_register_static ("GstCustomLayer", layer_types);
  }

  return custom_layer_type;
}
/*----------------------------------------------------------------*/
/*---------------------Sink event handling function---------------*/
/* this function handles sink events */
static gboolean gst_custom_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
	//g_print("gst_custom sink event  is invoked\n");
	GstCustom *filter;
	gboolean ret;

	//It retrieves the elements data from object
	filter = GST_CUSTOM (parent);

	//For debug info to genarate the log file
	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,GST_EVENT_TYPE_NAME (event), event);
	//g_print ("Received %s event: %" GST_PTR_FORMAT,GST_EVENT_TYPE_NAME (event), event);
	//g_print("\n");

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CAPS:
			{
				GstCaps *caps;

				/*--It is used to extract the capabilities (caps) information from a GStreamer event.*/
				gst_event_parse_caps (event, &caps);

				/* do something with the caps */
				//convert our caps to string for human readable format
				gchar *caps_str = gst_caps_to_string(caps);
				//g_print("Received caps: %s\n", caps_str);
				g_free(caps_str); //deallocate the memory for caps_str
				/* and forward */

				/*-----It is used to perform the default handling of a GStreamer event by a pad in a GStreamer element.--*/
				ret = gst_pad_event_default (pad, parent, event);
				break;
			}
		default:
			g_print("default event is handling\n");
			/*-----It is used to perform the default handling of a GStreamer event by a pad in a GStreamer element.--*/
			ret = gst_pad_event_default (pad, parent, event);
			break;
	}
	return ret;

}
/*-------------------------------------------------------*/
/*--------------Chain function --------------------------*/
/* chain function this function does the actual processing*/
static GstFlowReturn gst_custom_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	g_print("gst_custom chain function  is invoked\n");
	GstCustom *filter;


	//It retrieves the elements data from object
	filter = GST_CUSTOM (parent);

	/*if (filter->silent == FALSE)
	  g_print ("silent is false condition\n");
	  if (filter->silent == TRUE)
	  g_print ("silent is true condition\n");
	  if (filter->layer == 112)
	  g_print ("Default value:%d\n",filter->layer);
	  else
	  g_print ("Normal given value:%d\n",filter->layer);
	  if ((strcmp(filter->word,"Pranav") == 0))
	  g_print ("Default string:%s\n",filter->word);
	  else
	  g_print ("Normal string:%s\n",filter->word);
	  gint x, y;
	  GstMapInfo map;
	  guint8 *ptr;

	  buf = gst_buffer_make_writable (buf);

	  if (gst_buffer_map (buf, &map, GST_MAP_WRITE)) {
	  ptr = (guint8 *) map.data;
	// invert data 
	for (y = 0; y < 288; y++) {
	for (x = 0; x < 384; x++) 
	{
	ptr[x]=0;
	g_print("  %d",(0xfff&ptr[x]));

	}
	g_print("\n");
	ptr += 384;

	}
	gst_buffer_unmap (buf, &map);
	}*/







	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);
}


#ifndef PACKAGE
#define PACKAGE "myfirstcustom"
#endif

/* gstreamer looks for this structure to register custom
 *
 * exchange the string 'Template custom' with your custom description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		custom,
		"custom",
		custom_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
/*-----------GST_PLUGIN_DEFINE is a macro provided by GStreamer for defining a GStreamer plugin. 
 * It simplifies the process of registering the plugin with GStreamer's plugin system.
 */
