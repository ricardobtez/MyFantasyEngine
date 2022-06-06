cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyFantasyEngine)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/source)

file(GLOB FANTASY_ENGINE_SRCS "${PROJECT_SOURCE_DIR}/include/*.h"
     "${PROJECT_SOURCE_DIR}/include/*.hpp" "${PROJECT_SOURCE_DIR}/source/*.cpp"
     "${PROJECT_SOURCE_DIR}/source/*.c")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(FILAMENT_SKIP_SAMPLES ON)
set(JSON_BuildTests OFF)

if (WIN32)
     set(FILAMENT_SUPPORTS_VULKAN OFF)
     set(USE_STATIC_CRT  OFF)
     set(FILAMENT_DEPS filament backend bluegl filabridge filaflat utils geometry smol-v vkshaders ibl)
elseif (UNIX AND NOT APPLE)
     set(FILAMENT_DEPS filament backend bluegl bluevk filabridge filaflat utils geometry smol-v vkshaders ibl)
else()
     message(FATAL_ERROR "Platform not supported. Only Windows and Linux are supported")
endif()
 

find_package(Threads)

add_executable(MyFantasyEngine ${FANTASY_ENGINE_SRCS})

add_subdirectory(subprojects/filament)
add_subdirectory(subprojects/json)

target_link_libraries(MyFantasyEngine nlohmann_json::nlohmann_json ${FILAMENT_DEPS})