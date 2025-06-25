#pragma once

#include <string_view>

#include "cpptools/macros.hpp"
#include "cpptools/shared_memory.hpp"
#include "cpptools/spinlock_mutex.hpp"

namespace cpptools {

// Fast spinlock mutex for IPC via POSIX shared memory
class spinlock_mutex_ipc {
public:
    spinlock_mutex_ipc() = delete;
    spinlock_mutex_ipc(std::string_view name)
        : m_shmem(name, sizeof(spinlock_mutex)),
          m_mutex(m_shmem.as_struct<spinlock_mutex>()) {}
    ~spinlock_mutex_ipc() { m_mutex = nullptr; }

    CPPTOOLS_NO_COPY_OR_MOVE(spinlock_mutex_ipc);

    auto lock() { return m_mutex->lock(); }
    auto try_lock() noexcept { return m_mutex->try_lock(); }
    auto unlock() noexcept { return m_mutex->unlock(); }
    auto name() const { return m_shmem.name(); }
    auto reference_count() const { return m_shmem.reference_count(); }

private:
    shared_memory m_shmem;
    spinlock_mutex* m_mutex{nullptr};
};

}  // namespace cpptools
