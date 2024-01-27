#include <gst/gst.h>
#include <gst/video/video.h>

GST_DEBUG_CATEGORY_STATIC (videocrop2_debug);
#define GST_CAT_DEFAULT videocrop2_debug

// Define the properties for the videocrop2 element
enum {
  PROP_0,
  PROP_LEFT,
  PROP_RIGHT,
  PROP_TOP,
  PROP_BOTTOM,
  NUM_PROPERTIES
};

// Define the signals for the videocrop2 element
enum {
  SIGNAL_LAST,
  NUM_SIGNALS
};

static guint gst_videocrop2_signals[NUM_SIGNALS];

#define gst_videocrop2_parent_class parent_class
G_DEFINE_TYPE (GstVideoCrop2, gst_videocrop2, GST_TYPE_BASE_TRANSFORM);

static void gst_videocrop2_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_videocrop2_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

static GstCaps *gst_videocrop2_transform_caps (GstBaseTransform * trans, GstPadDirection direction, GstCaps * caps, GstCaps * filter);
static GstCaps *gst_videocrop2_fixate_caps (GstBaseTransform * trans, GstPadDirection direction, GstCaps * caps, GstCaps * othercaps);
static gboolean gst_videocrop2_set_info (GstVideoCrop2 * filter, GstCaps * incaps, GstCaps * outcaps);

static gboolean gst_videocrop2_transform (GstBaseTransform * trans, GstBuffer * inbuf, GstBuffer * outbuf);
static gboolean gst_videocrop2_start (GstBaseTransform * trans);
static gboolean gst_videocrop2_stop (GstBaseTransform * trans);

// Property defaults
#define DEFAULT_LEFT 0
#define DEFAULT_RIGHT 0
#define DEFAULT_TOP 0
#define DEFAULT_BOTTOM 0

static void
gst_videocrop2_class_init (GstVideoCrop2Class * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *trans_class = GST_BASE_TRANSFORM_CLASS (klass);

  // Set up debug category
  GST_DEBUG_CATEGORY_INIT (videocrop2_debug, "videocrop2", 0, "Video crop element");

  // Set up class signals
  gst_videocrop2_signals[SIGNAL_LAST] =
      g_signal_new ("frame-dropped", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
      G_STRUCT_OFFSET (GstVideoCrop2Class, frame_dropped),
      NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
      G_TYPE_POINTER);

  // Set up class properties
  gobject_class->set_property = gst_videocrop2_set_property;
  gobject_class->get_property = gst_videocrop2_get_property;

  g_object_class_install_property (gobject_class, PROP_LEFT,
      g_param_spec_int ("left", "Left", "Number of pixels to crop from the left",
          0, G_MAXINT, DEFAULT_LEFT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_RIGHT,
      g_param_spec_int ("right", "Right", "Number of pixels to crop from the right",
          0, G_MAXINT, DEFAULT_RIGHT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_TOP,
      g_param_spec_int ("top", "Top", "Number of pixels to crop from the top",
          0, G_MAXINT, DEFAULT_TOP, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_BOTTOM,
      g_param_spec_int ("bottom", "Bottom", "Number of pixels to crop from the bottom",
          0, G_MAXINT, DEFAULT_BOTTOM, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  // Set up transform virtual functions
  trans_class->transform_caps = GST_DEBUG_FUNCPTR (gst_videocrop2_transform_caps);
  trans_class->fixate_caps = GST_DEBUG_FUNCPTR (gst_videocrop2_fixate_caps);
  trans_class->set_info = GST_DEBUG_FUNCPTR (gst_videocrop2_set_info);
  trans_class->transform = GST_DEBUG_FUNCPTR (gst_videocrop2_transform);
  trans_class->start = GST_DEBUG_FUNCPTR (gst_videocrop2_start);
  trans_class->stop = GST_DEBUG_FUNCPTR (gst_videocrop2_stop);
}

static void
gst_videocrop2_init (GstVideoCrop2 * filter)
{
  filter->left = DEFAULT_LEFT;
  filter->right = DEFAULT_RIGHT;
  filter->top = DEFAULT_TOP;
  filter->bottom = DEFAULT_BOTTOM;
}

static void
gst_videocrop2_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstVideoCrop2 *filter = GST_VIDEOCROP2 (object);

  switch (prop_id) {
    case PROP_LEFT:
      filter->left = g_value_get_int (value);
      break;
    case PROP_RIGHT:
      filter->right = g_value_get_int (value);
      break;
    case PROP_TOP:
      filter->top = g_value_get_int (value);
      break;
    case PROP_BOTTOM:
      filter->bottom = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_videocrop2_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstVideoCrop2 *filter = GST_VIDEOCROP2 (object);

  switch (prop_id) {
    case PROP_LEFT:
      g_value_set_int (value, filter->left);
      break;
    case PROP_RIGHT:
      g_value_set_int (value, filter->right);
      break;
    case PROP_TOP:
      g_value_set_int (value, filter->top);
      break;
    case PROP_BOTTOM:
      g_value_set_int (value, filter->bottom);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_videocrop2_start (GstBaseTransform * trans)
{
  return TRUE;
}

static gboolean
gst_videocrop2_stop (GstBaseTransform * trans)
{
  return TRUE;
}

static gboolean
gst_videocrop2_set_caps (GstBaseTransform * trans, GstCaps * incaps, GstCaps * outcaps)
{
  GstVideoCrop2 *filter = GST_VIDEOCROP2 (trans);

  if (!gst_video_info_from_caps (&filter->in_info, incaps) ||
      !gst_video_info_from_caps (&filter->out_info, outcaps))
    return FALSE;

  return TRUE;
}

static gboolean
gst_videocrop2_transform_caps (GstBaseTransform * trans, GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
  GstVideoCrop2 *filter2 = GST_VIDEOCROP2 (trans);
  GstStructure *structure;
  gboolean fixed = FALSE;

  // Use gst_caps_foreach to iterate over each structure in caps
  // You may need to adjust this based on the specifics of your plugin
  gst_caps_foreach (caps, (GstCapsForeachFunc) gst_videocrop2_iterate_caps,
      &fixed);

  if (fixed) {
    // Add the crop properties to the caps
    structure = gst_caps_get_structure (caps, 0);
    gst_structure_set (structure, "left", G_TYPE_INT, filter2->left, NULL);
    gst_structure_set (structure, "right", G_TYPE_INT, filter2->right, NULL);
    gst_structure_set (structure, "top", G_TYPE_INT, filter2->top, NULL);
    gst_structure_set (structure, "bottom", G_TYPE_INT, filter2->bottom, NULL);
  }

  return caps;
}

static gboolean
gst_videocrop2_iterate_caps (GQuark field, GValue * value, gpointer user_data)
{
  gboolean *fixed = (gboolean *) user_data;

  // Check if the caps already contain cropping properties
  if (g_str_has_prefix (g_quark_to_string (field), "left") ||
      g_str_has_prefix (g_quark_to_string (field), "right") ||
      g_str_has_prefix (g_quark_to_string (field), "top") ||
      g_str_has_prefix (g_quark_to_string (field), "bottom")) {
    *fixed = TRUE;
  }

  return TRUE;
}

static gboolean
gst_videocrop2_fixate_caps (GstBaseTransform * trans, GstPadDirection direction, GstCaps * caps, GstCaps * othercaps)
{
  GstVideoCrop2 *filter = GST_VIDEOCROP2 (trans);
  gboolean fixed = FALSE;

  // Use gst_caps_foreach to iterate over each structure in caps
  // You may need to adjust this based on the specifics of your plugin
  gst_caps_foreach (caps, (GstCapsForeachFunc) gst_videocrop2_iterate_caps,
      &fixed);

  if (!fixed) {
    GstStructure *structure;
    gint width, height;

    // If cropping properties are not present, fixate the width and height
    structure = gst_caps_get_structure (caps, 0);

    if (gst_structure_get_int (structure, "width", &width) &&
        gst_structure_get_int (structure, "height", &height)) {
      gst_structure_set (structure, "left", G_TYPE_INT,
          MIN (filter->left, width - 1), NULL);
      gst_structure_set (structure, "right", G_TYPE_INT,
          MIN (filter->right, width - 1), NULL);
      gst_structure_set (structure, "top", G_TYPE_INT,
          MIN (filter->top, height - 1), NULL);
      gst_structure_set (structure, "bottom", G_TYPE_INT,
          MIN (filter->bottom, height - 1), NULL);
    }
  }

  return caps;
}

static gboolean
gst_videocrop2_set_info (GstVideoCrop2 * filter, GstCaps * incaps, GstCaps * outcaps)
{
  gst_video_info_from_caps (&filter->in_info, incaps);
  gst_video_info_from_caps (&filter->out_info, outcaps);

  return TRUE;
}

static gboolean
gst_videocrop2_transform (GstBaseTransform * trans, GstBuffer * inbuf, GstBuffer * outbuf)
{
  GstVideoCrop2 *filter = GST_VIDEOCROP2 (trans);
  GstMapInfo in_map, out_map;
  gboolean ret = FALSE;

  // Map input buffer
  if (!gst_buffer_map (inbuf, &in_map, GST_MAP_READ)) {
    GST_ERROR_OBJECT (trans, "Failed to map input buffer");
    goto out;
  }

  // Map output buffer
  if (!gst_buffer_map (outbuf, &out_map, GST_MAP_WRITE)) {
    GST_ERROR_OBJECT (trans, "Failed to map output buffer");
    goto unmap_inbuf;
  }

  // Perform cropping based on filter properties
  crop_video_frame(in_map.data, out_map.data, &filter->in_info, &filter->out_info,
                   filter->left, filter->right, filter->top, filter->bottom);

  ret = TRUE;

  // Unmap output buffer
  gst_buffer_unmap (outbuf, &out_map);

unmap_inbuf:
  // Unmap input buffer
  gst_buffer_unmap (inbuf, &in_map);

out:
  return ret;
}

static void
crop_video_frame (guint8 * in_data, guint8 * out_data, GstVideoInfo * in_info,
    GstVideoInfo * out_info, gint left, gint right, gint top, gint bottom)
{
  // Example cropping logic
  gint in_width = GST_VIDEO_INFO_WIDTH (in_info);
  gint in_height = GST_VIDEO_INFO_HEIGHT (in_info);
  gint in_stride = GST_VIDEO_INFO_PLANE_STRIDE (in_info, 0);
  gint out_stride = GST_VIDEO_INFO_PLANE_STRIDE (out_info, 0);
  gint in_pixel_stride = GST_VIDEO_INFO_COMP_STRIDE (in_info, 0);
  gint out_pixel_stride = GST_VIDEO_INFO_COMP_STRIDE (out_info, 0);

  guint8 *in_pixel = in_data + top * in_stride + left * in_pixel_stride;
  guint8 *out_pixel = out_data;

  for (gint y = 0; y < out_info->height; y++) {
    memcpy (out_pixel, in_pixel, out_info->width * out_pixel_stride);
    in_pixel += in_stride;
    out_pixel += out_stride;
  }
}

gboolean
gst_videocrop2_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "videocrop2", GST_RANK_NONE,
      GST_TYPE_VIDEOCROP2);
}
static void
crop_video_frame (guint8 * in_data, guint8 * out_data, GstVideoInfo * in_info,
    GstVideoInfo * out_info, gint left, gint right, gint top, gint bottom)
{
  gint in_width = GST_VIDEO_INFO_WIDTH (in_info);
  gint in_height = GST_VIDEO_INFO_HEIGHT (in_info);

  // Y plane
  guint8 *in_y_pixel = in_data + top * GST_VIDEO_INFO_PLANE_STRIDE (in_info, 0) + left;
  guint8 *out_y_pixel = out_data;

  // U and V planes
  guint8 *in_uv_pixel = in_data + in_height / 2 + top / 2 * GST_VIDEO_INFO_PLANE_STRIDE (in_info, 1) + left / 2;
  guint8 *out_uv_pixel = out_data + out_info->height * GST_VIDEO_INFO_PLANE_STRIDE (out_info, 0);

  for (gint y = 0; y < out_info->height; y++) {
    // Copy Y plane
    memcpy (out_y_pixel, in_y_pixel, out_info->width);

    // Copy U and V planes
    memcpy (out_uv_pixel, in_uv_pixel, out_info->width / 2);

    in_y_pixel += GST_VIDEO_INFO_PLANE_STRIDE (in_info, 0);
    out_y_pixel += GST_VIDEO_INFO_PLANE_STRIDE (out_info, 0);

    in_uv_pixel += GST_VIDEO_INFO_PLANE_STRIDE (in_info, 1);
    out_uv_pixel += GST_VIDEO_INFO_PLANE_STRIDE (out_info, 0);
  }
}
#include <gst/gst.h>

// Assuming the format is NV12
#define WIDTH 640
#define HEIGHT 480

void process_nv12_data(const GstMapInfo *map) {
    // Assuming NV12 format: Y plane is first, followed by interleaved UV plane
    guint8 *y_data = map->data;
    guint8 *uv_data = map->data + WIDTH * HEIGHT;

    // Access pixel data
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            guint8 y_pixel = y_data[y * WIDTH + x];
            guint8 u_pixel = uv_data[(y / 2) * WIDTH + 2 * (x / 2)];
            guint8 v_pixel = uv_data[(y / 2) * WIDTH + 2 * (x / 2) + 1];

            // Process y_pixel, u_pixel, v_pixel as needed
        }
    }
}

void process_video_frame(GstSample *sample) {
    GstBuffer *buffer = gst_sample_get_buffer(sample);

    if (buffer) {
        GstMapInfo map;
        if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
            // Process the pixel data based on the format
            process_nv12_data(&map);

            gst_buffer_unmap(buffer, &map);
        }
    }
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create pipeline, set up elements, and play the pipeline

    // Assume you have a callback for each video frame
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, (GstBusFunc)bus_callback, NULL);

    // Main loop
    g_main_loop_run(loop);

    // Cleanup
    g_main_loop_unref(loop);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}


