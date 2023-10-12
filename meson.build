project('ufo-gst-comp', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++2a'])

gst_dep = dependency('gstreamer-1.0')
gst_video_dep = dependency('gstreamer-video-1.0')

ufo_dep = dependency('ufo')

gst_deps = [
  gst_dep,
  gst_video_dep,
  dependency('fmt')
]

ufo_deps = [
  ufo_dep,
  dependency('OpenCL'),
]

executable('gst',
           'gst.cpp',
           dependencies : gst_deps,
           install : true)

executable('ufo',
           'ufo.cpp',
           dependencies : ufo_deps,
           install : true)
