cmake_minimum_required(VERSION 3.14)
project(TigerInC C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GCC_COVERAGE_FLAGS} -Wall -O0 -g -D DEBUG=1 -D DEBUG2=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_FLAGS}")

add_subdirectory(test)
add_subdirectory(src)
include(src/CMakeLists.txt)
add_executable(tigerc ${SRC_FILES})

set_target_properties(tigerc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)