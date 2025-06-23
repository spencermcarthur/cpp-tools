file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(cpptools SHARED ${SOURCES})
target_include_directories(cpptools PUBLIC ${CMAKE_SOURCE_DIR}/include)
