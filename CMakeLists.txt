cmake_minimum_required(VERSION 2.8)
project (pf)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(SUB_DIR src tests)

find_package(Glibmm)
find_package(GlibmmUtils)

subdirs(${SUB_DIR})
