cmake_minimum_required(VERSION 3.15)

project(ClockDB C CXX)

set(DEFINE "-DHAVE_SSE2 -DSFMT_MEXP=19937")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -Wno-deprecated -std=c++20 -fopenmp -O3 -msse2 ${DEFINE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -msse2 ${DEFINE}")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


add_library(sfmt STATIC ${INCLUDE_DIR}/SFMT/SFMT.c)
target_include_directories(sfmt PUBLIC ${INCLUDE_DIR}/SFMT)

add_library(parallel-radix-sort INTERFACE)
target_include_directories(parallel-radix-sort INTERFACE ${INCLUDE_DIR}/parallel-radix-sort)

add_library(utils STATIC ${SOURCE_DIR}/utils/utils.cpp)
target_link_libraries(utils PRIVATE parallel-radix-sort sfmt)
target_include_directories(utils PUBLIC ${INCLUDE_DIR})

add_library(searchutils STATIC ${SOURCE_DIR}/utils/search_utils.cpp)
target_link_libraries(searchutils PRIVATE utils)
target_include_directories(searchutils PUBLIC ${INCLUDE_DIR})

add_executable(build ${SOURCE_DIR}/build.cpp)
target_link_libraries(build PRIVATE utils)

add_executable(search ${SOURCE_DIR}/search.cpp)
target_link_libraries(search PRIVATE utils searchutils)