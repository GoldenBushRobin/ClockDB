cmake_minimum_required(VERSION 3.15)

project(ClockDB LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

# set(DEFINE "-DHAVE_SSE2 -DSFMT_MEXP=19937")
set(SHARED "-Wall -O3 -Wno-deprecated")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SHARED}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED}")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(UTILS_DIR ${SOURCE_DIR}/utils)


add_library(sfmt STATIC ${INCLUDE_DIR}/SFMT/SFMT.c)
target_include_directories(sfmt PUBLIC ${INCLUDE_DIR}/SFMT)
target_compile_options(sfmt PRIVATE -msse2 -DHAVE_SSE2=1 -DSFMT_MEXP=19937)


add_library(parallel-radix-sort INTERFACE)
target_include_directories(parallel-radix-sort INTERFACE ${INCLUDE_DIR}/parallel-radix-sort)
target_link_libraries(parallel-radix-sort INTERFACE OpenMP::OpenMP_CXX)

add_library(common 
    ${UTILS_DIR}/io_utils.cpp 
    ${UTILS_DIR}/rand_utils.cpp 
    ${UTILS_DIR}/log_utils.cpp
    ${UTILS_DIR}/path_utils.cpp
)
target_link_libraries(common PRIVATE sfmt)
target_include_directories(common PUBLIC ${INCLUDE_DIR})

add_library(buildutils ${UTILS_DIR}/build_utils.cpp)
target_link_libraries(buildutils PRIVATE parallel-radix-sort common)
target_include_directories(buildutils PUBLIC ${INCLUDE_DIR})
target_link_libraries(buildutils PUBLIC OpenMP::OpenMP_CXX)

add_library(searchutils ${UTILS_DIR}/search_utils.cpp)
target_link_libraries(searchutils PRIVATE common)
target_include_directories(searchutils PUBLIC ${INCLUDE_DIR})

add_executable(build ${SOURCE_DIR}/build.cpp)
target_link_libraries(build PRIVATE buildutils)
target_link_libraries(build PUBLIC OpenMP::OpenMP_CXX)

add_executable(search ${SOURCE_DIR}/search.cpp)
target_link_libraries(search PRIVATE searchutils)

find_package(Crow)

add_library(serverutils ${UTILS_DIR}/server_utils.cpp)
target_link_libraries(serverutils PRIVATE common)
target_include_directories(serverutils PUBLIC ${INCLUDE_DIR})
target_compile_options(serverutils PRIVATE -lpthread)

add_executable(server ${SOURCE_DIR}/searchserver.cpp)
target_link_libraries(server PUBLIC Crow::Crow PRIVATE serverutils searchutils)
target_compile_options(server PRIVATE -lpthread)
