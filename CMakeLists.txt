cmake_minimum_required(VERSION 3.15)

project(ClockDB C CXX)

set(CMAKE_C_FLAGS "-O3")

set(DEFINE "-DHAVE_SSE2 -DSFMT_MEXP=19937")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -Wno-deprecated -std=c++20 -fopenmp -O3 -msse2 ${DEFINE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -msse2 ${DEFINE}")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_library(sfmt STATIC ${CMAKE_SOURCE_DIR}/SFMT/SFMT.c)
target_include_directories(sfmt PUBLIC ${CMAKE_SOURCE_DIR}/SFMT)

add_library(parallel-radix-sort INTERFACE)
target_include_directories(parallel-radix-sort INTERFACE ${CMAKE_SOURCE_DIR}/parallel-radix-sort)

add_library(utils STATIC ${SOURCE_DIR}/utils.cpp)
target_link_libraries(utils PRIVATE parallel-radix-sort sfmt)
target_include_directories(utils 
    PUBLIC ${CMAKE_SOURCE_DIR}/include
           ${CMAKE_SOURCE_DIR}/parallel-radix-sort
           ${CMAKE_SOURCE_DIR}/SFMT
)


add_executable(builder ${SOURCE_DIR}/builder.cpp)
target_link_libraries(builder utils)

add_executable(quickbuild ${SOURCE_DIR}/quickbuild.cpp)
target_link_libraries(quickbuild utils)