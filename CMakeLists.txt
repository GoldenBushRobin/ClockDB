cmake_minimum_required(VERSION 3.15)

project(ClockDB C CXX)

include_directories(${CMAKE_SOURCE_DIR}/SFMT)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_library(sfmt STATIC ${CMAKE_SOURCE_DIR}/SFMT/SFMT.c)
add_library(utils STATIC ${SOURCE_DIR}/utils.cpp)