project('test', 'cpp')


cpp = meson.get_compiler('cpp')
libmeep = cpp.find_library(
  'libmeep',
  dirs: '/usr/local/lib/'
)

libmeep_dep = declare_dependency(
  include_directories: include_directories('/usr/local/include/'),
  dependencies: [libmeep]
)

executable('demo', 'main.cpp', dependencies : [libmeep_dep])
