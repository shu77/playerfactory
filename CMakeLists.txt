cmake_minimum_required(VERSION 2.8)
project (pf)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(SUB_DIR src tests)

option(OPTION_MMUTILS "If set to ON, -DUSE_GLIB_MMSUTILS will be defined" OFF)

find_package(Glib)
find_package(GIO)
find_package(GStreamer)

message(STATUS "now check mmutils at main root")
if (OPTION_MMUTILS)
  message(STATUS "DEFINE_MMUTILS enabled...")
  add_definitions( -DUSE_GLIB_MMUTILS)
  find_package(Glibmm)
  find_package(GlibmmUtils)
endif (OPTION_MMUTILS)

add_custom_target(clean-cmake-files
   COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
)

subdirs(${SUB_DIR})

