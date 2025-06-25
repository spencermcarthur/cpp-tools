enable_testing()

find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_libraries(cpptools GTest::gtest GTest::gtest_main)



# semaphore_lock
add_executable(test_semaphore_lock EXCLUDE_FROM_ALL test_semaphore_lock.cpp)
add_test(NAME "semaphore_lock" COMMAND test_semaphore_lock)

# shared_memory
add_executable(test_shared_memory EXCLUDE_FROM_ALL test_shared_memory.cpp)
add_test(NAME "shared_memory" COMMAND test_shared_memory)

# spinlock_mutex
add_executable(test_spinlock_mutex EXCLUDE_FROM_ALL test_spinlock_mutex.cpp)
add_test(NAME "spinlock_mutex" COMMAND test_spinlock_mutex)

# spinlock_mutex_ipc
add_executable(test_spinlock_mutex_ipc EXCLUDE_FROM_ALL test_spinlock_mutex_ipc.cpp)
add_test(NAME "spinlock_mutex_ipc" COMMAND test_spinlock_mutex_ipc)

# thread
add_executable(test_thread EXCLUDE_FROM_ALL test_thread.cpp)
add_test(NAME "thread" COMMAND test_thread)


# Build tests
add_custom_target(build_tests DEPENDS
    test_semaphore_lock
    test_shared_memory
    test_spinlock_mutex
    test_spinlock_mutex_ipc
    test_thread
)

# Run tests
add_custom_target(run_tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS build_tests)
