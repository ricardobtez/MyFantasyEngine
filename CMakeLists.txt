cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyFantasyEngine)

message("Build type: = ${CMAKE_BUILD_TYPE}")

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/source)

file(GLOB_RECURSE FANTASY_ENGINE_SRCS	"${PROJECT_SOURCE_DIR}/source/*.cpp"
										"${PROJECT_SOURCE_DIR}/source/*.h")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#Json settings
set(JSON_BuildTests OFF)

#GLFW settings
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)

#Filament settings
set(FILAMENT_SKIP_SAMPLES ON)
set(FILAMENT_SKIP_SDL2 ON)

if(EMSCRIPTEN)
	set(WEBGL ON)
	set(FILAMENT_DEPS	filament
							backend
							bluegl
							filabridge
							filaflat
							utils
							geometry
							ibl
							filameshio
							image
							ktxreader)
elseif(WIN32)
	set(FILAMENT_SUPPORTS_VULKAN ON)
	set(USE_STATIC_CRT  OFF)
	set(FILAMENT_DEPS	filament 
						backend
						bluegl
						filabridge
						filaflat
						utils
						geometry
						smol-v
						vkshaders
						ibl
						filameshio
						image
						ktxreader)
elseif(UNIX AND NOT APPLE)
	set(FILAMENT_DEPS	filament
						backend
						bluegl
						bluevk
						filabridge
						filaflat
						utils
						geometry
						smol-v
						vkshaders
						ibl
						filameshio
						image
						ktxreader)
else()
	message(FATAL_ERROR "Platform not supported")
endif()


find_package(Threads)

add_executable(MyFantasyEngine ${FANTASY_ENGINE_SRCS})

add_subdirectory(subprojects/filament)
add_subdirectory(subprojects/json)
add_subdirectory(subprojects/glfw)

target_link_libraries(MyFantasyEngine nlohmann_json::nlohmann_json ${FILAMENT_DEPS} glfw)
