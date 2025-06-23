find_package(benchmark REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_libraries(cpptools benchmark)


# spinlock_mutex
add_executable(spinlock_mutex_benchmark EXCLUDE_FROM_ALL spinlock_mutex.cpp)


# Build benchmarks
add_custom_target(build_benchmarks DEPENDS
    spinlock_mutex_benchmark
)
add_custom_target(run_benchmarks DEPENDS build_benchmarks COMMAND
    spinlock_mutex_benchmark
)
