cmake_minimum_required(VERSION 3.14)
project(TigerInC)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0 -g -D DEBUG=1 -D DEBUG2=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")

add_subdirectory(test)
include(src/CMakeLists.txt)
add_executable(tigerc ${SRC_FILES})