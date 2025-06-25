#include "cpptools/spinlock_mutex.hpp"

#include <atomic>

namespace cpptools {

void spinlock_mutex::lock() {
    // Tries to set flag until test_and_set() returns false, i.e. the mutex was
    // unlocked and we acquired it.
    while (flag_.test_and_set(std::memory_order_acquire));
}

bool spinlock_mutex::try_lock() noexcept {
    // If test_and_set() returns false, it indicates that the flag was acquired
    // by us and the try_lock() returns true.
    // If test_and_set() returns true, the flag was already held by another
    // caller and we failed to acquire it, so try_lock() returns false.
    return !flag_.test_and_set(std::memory_order_acquire);
}

void spinlock_mutex::unlock() noexcept {
    // Clear the flag if we have the mutex.
    flag_.clear(std::memory_order_release);
}

}  // namespace cpptools