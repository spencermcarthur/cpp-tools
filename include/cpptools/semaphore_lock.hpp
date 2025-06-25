#pragma once

#include <linux/limits.h>
#include <semaphore.h>

#include <cstddef>
#include <string>
#include <string_view>

#include "cpptools/macros.hpp"

namespace cpptools {

// POSIX semaphore lock for IPC synchronization
class semaphore_lock {
public:
    // See "DESCRIPTION/Named semaphores" at
    // https://man7.org/linux/man-pages/man7/sem_overview.7.html
    static constexpr size_t MAX_SEMAPHORE_NAME_LEN = NAME_MAX - 4;

    explicit semaphore_lock(std::string_view name);
    ~semaphore_lock();

    CPPTOOLS_NO_COPY_OR_MOVE(semaphore_lock);

    bool lock() noexcept;
    bool lock(int &err) noexcept;

    bool unlock() noexcept;
    bool unlock(int &err) noexcept;

private:
    std::string m_name;
    sem_t *m_semaphore{nullptr};
    bool m_owned{false};
};

}  // namespace cpptools
