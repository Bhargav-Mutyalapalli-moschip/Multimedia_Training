project('gst-template', 'c', version : '1.19.0.1', license : 'LGPL')

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

cc = meson.get_compiler('c')

gst_version = meson.project_version()

api_version = '1.0'

gst_dep = dependency('gstreamer-1.0', version : '>=1.19',
    required : true, fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.19',
  fallback : ['gstreamer', 'gst_base_dep'])
gstbase_dep = dependency('gstreamer-video-1.0', version : '>=1.19',
  fallback : ['gstreamer', 'gst_base_dep'])

# Add the libswscale library and linker flags
libswscale_dep = cc.find_library('swscale')
libavformat_dep = cc.find_library('avformat')
libavcodec_dep = cc.find_library('avcodec')
libavutil_dep = cc.find_library('avutil')

subdir('gst-app')
subdir('gst-plugin')
