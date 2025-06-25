#pragma once

#include <atomic>

#include "cpptools/macros.hpp"

namespace cpptools {

// Fast spinlock mutex
class spinlock_mutex {
    std::atomic_flag flag_{ATOMIC_FLAG_INIT};

public:
    spinlock_mutex() = default;
    ~spinlock_mutex() = default;

    CPPTOOLS_NO_COPY_OR_MOVE(spinlock_mutex);

    void lock();
    bool try_lock() noexcept;
    void unlock() noexcept;
};

}  // namespace cpptools
