#include "cpptools/spinlock_mutex.hpp"

#include <benchmark/benchmark.h>

#include <mutex>

void bm_std_mutex(benchmark::State& s) {
    std::mutex m;
    for (auto _ : s) {
        std::scoped_lock l(m);
    }
    s.SetItemsProcessed(s.iterations());
}
BENCHMARK(bm_std_mutex);

void bm_spinlock_mutex(benchmark::State& s) {
    using tools::spinlock_mutex;

    spinlock_mutex m;
    for (auto _ : s) {
        std::scoped_lock l(m);
    }
    s.SetItemsProcessed(s.iterations());
}
BENCHMARK(bm_spinlock_mutex);

BENCHMARK_MAIN();
