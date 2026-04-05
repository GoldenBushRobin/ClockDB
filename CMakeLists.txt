cmake_minimum_required(VERSION 3.15)

project(ClockDB C CXX)

set(DEFINE "-DHAVE_SSE2 -DSFMT_MEXP=19937")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -Wno-deprecated -std=c++20 -fopenmp -O3 -msse2 ${DEFINE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -msse2 ${DEFINE}")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(UTILS_DIR ${SOURCE_DIR}/utils)


add_library(sfmt STATIC ${INCLUDE_DIR}/SFMT/SFMT.c)
target_include_directories(sfmt PUBLIC ${INCLUDE_DIR}/SFMT)

add_library(parallel-radix-sort INTERFACE)
target_include_directories(parallel-radix-sort INTERFACE ${INCLUDE_DIR}/parallel-radix-sort)

add_library(common 
    ${UTILS_DIR}/io_utils.cpp 
    ${UTILS_DIR}/rand_utils.cpp 
    ${UTILS_DIR}/log_utils.cpp
)
target_link_libraries(common PRIVATE sfmt)
target_include_directories(common PUBLIC ${INCLUDE_DIR})

add_library(buildutils ${UTILS_DIR}/build_utils.cpp)
target_link_libraries(buildutils PRIVATE parallel-radix-sort common)
target_include_directories(buildutils PUBLIC ${INCLUDE_DIR})

add_library(searchutils ${UTILS_DIR}/search_utils.cpp)
target_link_libraries(searchutils PRIVATE common)
target_include_directories(searchutils PUBLIC ${INCLUDE_DIR})

add_executable(build ${SOURCE_DIR}/build.cpp)
target_link_libraries(build PRIVATE buildutils)

add_executable(search ${SOURCE_DIR}/search.cpp)
target_link_libraries(search PRIVATE searchutils)