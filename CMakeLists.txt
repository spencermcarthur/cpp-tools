cmake_minimum_required(VERSION 3.20)
project(cpp-tools)

if(NOT LINUX)
    message(FATAL_ERROR "Requires Linux OS")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Building Release configuration")
    set(BUILD_DEBUG_CONFIG OFF)
    add_compile_options(-Ofast)
else()
    message(STATUS "Building Debug configuration")
    set(BUILD_DEBUG_CONFIG ON)
    add_compile_options(-ggdb3)
    add_compile_definitions(DEBUG=1)
endif()

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(tests)