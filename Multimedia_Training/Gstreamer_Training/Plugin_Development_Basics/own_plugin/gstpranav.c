//Own headerfile
#include "gstpranav.h"

enum
{
	PROP_SILENT,
	PROP_LAYER,
};



//create a category for gstreamer custom plugin debugging info
GST_DEBUG_CATEGORY_STATIC(gst_pranav_debug);
#define GST_CAT_LEVEL_DEBUG gst_pranav_debug

//create a static pad templates for src and sink
//create the src pad template
static GstStaticPadTemplate my_element_src_template =GST_STATIC_PAD_TEMPLATE("src",GST_PAD_SRC,GST_PAD_ALWAYS,GST_STATIC_CAPS("ANY"),NULL);
//create the sink pad template
static GstStaticPadTemplate my_element_sink_template =GST_STATIC_PAD_TEMPLATE("sink",GST_PAD_SINK,GST_PAD_ALWAYS,GST_STATIC_CAPS("ANY"),NULL);

//define the element for hierarchy ,function pointer implementation
G_DEFINE_TYPE(GstPranav,pranav, GST_TYPE_ELEMENT);

//define the elements to register in main init function
GST_ELEMENT_REGISTER_DEFINE (pranav, "pranav", GST_RANK_NONE,GstPranav);

//entry point of every plugin
static gboolean pranav_init (GstPlugin * pranav)
{
	g_print("entry point of every plugin\n");
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template croptech' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_pranav_debug, "pranav",0, "Template pranav");
	return GST_ELEMENT_REGISTER (pranav,pranav);
}
//second point is initilaization of class
static void gst_pranav_class_init (GstPranavClass * klass)
{
	g_print("gst_croptech class is initialized\n");
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;
	gobject_class->set_property = gst_pranav_set_property;
	gobject_class->get_property = gst_pranav_get_property;
	g_object_class_install_property (gobject_class, PROP_SILENT,g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",FALSE, G_PARAM_READWRITE));
	gst_element_class_set_details_simple (gstelement_class,
			"Croptech",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Bhargav Mutyalapalli <<user@hostname.org>>");
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&sink_factory));
}
