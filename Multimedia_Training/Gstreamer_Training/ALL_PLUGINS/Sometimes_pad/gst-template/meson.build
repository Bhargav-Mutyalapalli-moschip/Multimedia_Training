project('gst-template', 'c', version : '1.19.0.1', license : 'LGPL')
#include <libs/bytestream/bytestream.h>

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

cc = meson.get_compiler('c')

gst_version = meson.project_version()

api_version = '1.0'

gst_dep = dependency('gstreamer-1.0', version : '>=1.19',
    required : true, fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.19',
  fallback : ['gstreamer', 'gst_base_dep'])
 # Link the math library (libm)
bytestream_dep = dependency('bytestream', required : true, fallback : 'bytestream-fallback')
bytestream_include = bytestream_dep.get_include_directories()
include_directories(bytestream_include)

subdir('gst-app')
subdir('gst-plugin')
