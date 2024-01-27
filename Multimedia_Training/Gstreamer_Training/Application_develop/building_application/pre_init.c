#include <stdio.h>
#include <gst/gst.h>

int
main (int   argc,
      char *argv[])
{

  gst_init (&argc, &argv);


  printf ("This program is linked against GStreamer %d.%d.%d\n",GST_VERSION_MAJOR,GST_VERSION_MINOR,GST_VERSION_MICRO);

  return 0;
}
