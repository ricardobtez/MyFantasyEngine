project('filament', 'cpp', version : 'v1.23.0')

cpp = meson.get_compiler('cpp')

project_dir  = meson.current_source_dir()

filament_dep = []

filament_dep += declare_dependency(
	dependencies : cpp.find_library('backend', dirs : join_paths(project_dir, 'lib/x86_64/mt/')),
	include_directories : include_directories('include/'))

filament_dep += declare_dependency(
	dependencies : cpp.find_library('filament', dirs : join_paths(project_dir, 'lib/x86_64/mt/')),
	include_directories : include_directories('include/'))
