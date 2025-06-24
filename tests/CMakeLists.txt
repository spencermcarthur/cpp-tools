enable_testing()

find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_libraries(cpptools GTest::gtest GTest::gtest_main)


# spinlock_mutex
add_executable(test_spinlock_mutex EXCLUDE_FROM_ALL test_spinlock_mutex.cpp)
add_test(NAME "spinlock_mutex" COMMAND test_spinlock_mutex)


# spinlock_mutex
add_executable(test_thread EXCLUDE_FROM_ALL test_thread.cpp)
add_test(NAME "thread" COMMAND test_thread)


# Build tests
add_custom_target(build_tests DEPENDS
    test_spinlock_mutex
    test_thread
)

# Run tests
add_custom_target(run_tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS build_tests)
