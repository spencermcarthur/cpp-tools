file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/cpptools/*.hpp)

add_library(cpptools SHARED ${SOURCES})
target_include_directories(cpptools PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_target_properties(cpptools PROPERTIES PUBLIC_HEADER "${HEADERS}")
