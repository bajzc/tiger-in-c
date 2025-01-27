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

macro(reg_test name src)
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
    gtest_discover_tests(
            ${name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_target_properties(${name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}
    )
endmacro()

reg_test(env_test env_test.cpp)
reg_test(color_test color_test.cpp)
reg_test(frame_test frame/frame_test.cpp)
reg_test(translate_test translate/translate_test.cpp)
reg_test(semant_test semant/semant_test.cpp)
reg_test(xv6_test test_in_xv6.cpp)

add_executable(
        all_test
        all_test.cpp
        ${tests_src}
)
target_link_libraries(
        all_test
        GTest::gtest
        tigerc_lib
)
gtest_discover_tests(
        all_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(all_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(all_test tigerc)

include(GoogleTest)
include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/src)
