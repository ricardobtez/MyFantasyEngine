cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyFantasyEngine)

message("Build type: = ${CMAKE_BUILD_TYPE}")

#Set build platform
if(EMSCRIPTEN)
	add_compile_definitions(MFE_PLATFORM_WEB)
elseif(WIN32)
	add_compile_definitions(MFE_PLATFORM_WIN)
elseif(APPLE)
	add_compile_definitions(MFE_PLATFORM_MAC)
elseif(UNIX)
	add_compile_definitions(MFE_PLATFORM_LINUX)
else()
	message(FATAL_ERROR "Platform not supported")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/source)

file(GLOB_RECURSE FANTASY_ENGINE_SRCS	
	"${PROJECT_SOURCE_DIR}/source/*.cpp"
	"${PROJECT_SOURCE_DIR}/source/*.h")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#Json settings
set(JSON_BuildTests OFF)

#GLFW settings
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

#Filament settings
set(FILAMENT_SKIP_SAMPLES ON)
set(FILAMENT_SKIP_SDL2 ON)
set(FILAMENT_DEPS	
	filament
	backend
	filabridge
	filaflat
	utils
	geometry
	ibl
	filameshio
	image
	ktxreader)

if(EMSCRIPTEN)
	set(WEBGL ON)
elseif(WIN32)
	set(FILAMENT_SUPPORTS_VULKAN OFF)
	set(USE_STATIC_CRT OFF)
	list(APPEND FILAMENT_DEPS 
		bluegl
		smol-v
		vkshaders
		stb)
elseif(APPLE)
	list(APPEND FILAMENT_DEPS 
		bluegl
		smol-v
		vkshaders
		stb)
endif()

find_package(Threads)

add_executable(MyFantasyEngine ${FANTASY_ENGINE_SRCS})

add_subdirectory(subprojects/filament)

add_subdirectory(subprojects/json)

if(EMSCRIPTEN)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s TOTAL_MEMORY=32MB -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1")
	set(GLFW_DEPS "")
else()
	add_subdirectory(subprojects/glfw)
	set(GLFW_DEPS glfw)
endif()

target_link_libraries(MyFantasyEngine nlohmann_json::nlohmann_json ${FILAMENT_DEPS} ${GLFW_DEPS})
