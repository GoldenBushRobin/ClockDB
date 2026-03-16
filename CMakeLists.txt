cmake_minimum_required(VERSION 3.15)

project(ClockDB C CXX)

set(CMAKE_C_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated -fopenmp -O3")


include_directories(${CMAKE_SOURCE_DIR}/SFMT)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_library(sfmt STATIC ${CMAKE_SOURCE_DIR}/SFMT/SFMT.c)
add_library(utils STATIC ${SOURCE_DIR}/utils.cpp)

add_executable(calcclocks ${SOURCE_DIR}/calcclocks.cpp)
target_link_libraries(calcclocks utils sfmt)