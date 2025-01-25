project(tests)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 2.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif ()
FIND_PACKAGE(GTest)
IF (NOT ${GTest_FOUND})
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)
ENDIF ()

include(${CMAKE_SOURCE_DIR}/src/CMakeLists.txt)

add_library(tigerc_lib ${SRC_FILES})

macro(add_test name src)
    add_executable(
            ${name}
            ${src}
    )
    target_link_libraries(
            ${name}
            GTest::gtest_main
            tigerc_lib
    )
    set(tests_src ${tests_src} ${src})
endmacro()

add_test(env_test env_test.cpp)
add_test(color_test color_test.cpp)
add_test(frame_test frame/frame_test.cpp)
add_test(translate_test translate/translate_test.cpp)
add_test(xv6_test test_in_xv6.cpp)

add_executable(
        all_test
        all_test.cpp
        ${tests_src}
)
target_link_libraries(
        all_test
        GTest::gtest_main
        tigerc_lib
)

include(GoogleTest)
include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/src)
