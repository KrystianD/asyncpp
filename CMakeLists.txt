cmake_minimum_required(VERSION 3.10)
project(asyncpp)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)

add_subdirectory(asyncpp)
add_subdirectory(asyncpp_uv)
add_subdirectory(asyncpp_uv_curl)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()