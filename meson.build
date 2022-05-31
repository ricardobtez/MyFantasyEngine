project('My Fantasy Engine', 'cpp', version: '1.0.0',
  default_options : ['default_library=static',
                     'cpp_std=c++17',
                     'buildtype=debugoptimized'])

cpp = meson.get_compiler('cpp')

link_args = []
name_suffix = []
dependencies = []

# filament configuration
cmake = import('cmake')
filament_opts = cmake.subproject_options()
filament_opts.set_install(false)
filament_opts.add_cmake_defines({
	# Add other CMake options for filament, if needed
})
if cpp.get_id() == 'emscripten'
	# TODO
else
	# TODO
endif
filament_subproject = cmake.subproject('filament', options: filament_opts)
dependencies += raylib_subproject.dependency('filament')

json_subproject = subproject('json')
dependencies += json_subproject.get_variable('nlohmann_json_dep')

# General configuration
if host_machine.system() == 'windows'
	# TODO
elif host_machine.system() == 'mac'
	# TODO
elif host_machine.system() == 'linux'
	# TODO
elif host_machine.system() == 'emscripten'
	# TODO
endif

# Project files
inc_dir = include_directories(
	'source',
)

sources = files(
	'source/main.cpp',
)

executable('MyFantasyEngine',
sources,
dependencies: dependencies,
include_directories: inc_dir,
link_args: link_args,
name_suffix: name_suffix,
)