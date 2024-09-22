cmake_minimum_required(VERSION 3.21)
project(AnimeEffects C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(get_cpm)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
else()
    set(B_PRODUCTION_MODE ON)
endif()
CPMAddPackage("gh:batterycenter/embed#v1.2.19")

include(GNUInstallDirs)
# You need to add the Qt directory to your path if you're on Windows, otherwise you'll have a very bad time
find_package(Qt6 REQUIRED COMPONENTS Core5Compat Core Gui OpenGL OpenGLWidgets Xml Concurrent Widgets Multimedia)
# Get sources
add_subdirectory(src/util)
add_subdirectory(src/thr)
add_subdirectory(src/cmnd)
add_subdirectory(src/gl)
add_subdirectory(src/img)
add_subdirectory(src/core)
add_subdirectory(src/ctrl)
add_subdirectory(src/gui)
