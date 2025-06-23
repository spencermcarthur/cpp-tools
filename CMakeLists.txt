cmake_minimum_required(VERSION 3.20)

project(Template)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_subdirectory(bin)
add_subdirectory(lib)