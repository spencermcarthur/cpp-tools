#include "cpptools/semaphore_lock.hpp"

#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstring>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <system_error>

#define SCESV static constexpr std::string_view

namespace cpptools {

std::system_error make_sys_err(const int err, const std::string_view what) {
    return std::system_error(std::make_error_code(static_cast<std::errc>(err)),
                             what.data());
}

semaphore_lock::semaphore_lock(std::string_view name) {
    if (name.empty() || name.length() > MAX_SEMAPHORE_NAME_LEN) {
        SCESV fmt =
            "Semaphore name \"{}\" of length {} is invalid: length must be [1, "
            "{})";
        throw std::length_error(
            std::format(fmt, name, name.length(), MAX_SEMAPHORE_NAME_LEN));
    }

    m_name = name;

    // Open semaphore fd with value 1
    sem_t *sem =
        sem_open(m_name.c_str(), O_CREAT | O_EXCL, S_IRUSR + S_IWUSR, 1);

    // Check if failed
    if (sem == SEM_FAILED) {
        // Semaphore already exists
        if (errno == EEXIST) {
            sem = sem_open(m_name.c_str(), 0, S_IRUSR + S_IWUSR);
        }

        // Check if failed again
        if (sem == SEM_FAILED) {
            const int err = errno;
            SCESV fmt = "Can't access semaphore \"{}\": {}";
            const std::string what = std::format(fmt, m_name, strerror(err));
            throw make_sys_err(err, what);
        }
    }

    m_semaphore = sem;
}

semaphore_lock::~semaphore_lock() {
    // Check ownership
    if (m_owned) {
        // Try to unlock
        int err;
        if (!unlock(err)) {
            std::cerr << std::format("Failed to unlock semaphore \"{}\": {}",
                                     m_name, strerror(err));
        }
    }

    // Try to close
    const int ret = sem_close(m_semaphore);
    if (ret != 0) {
        const int err = errno;
        std::cerr << std::format("Failed to close semaphore \"{}\" fd: {}",
                                 m_name, strerror(err));
    }
}

bool semaphore_lock::lock() noexcept {
    if (m_owned) return true;

    // Try to lock
    const bool locked = (sem_trywait(m_semaphore) == 0);
    if (locked) {
        m_owned = true;
    }

    return locked;
}

bool semaphore_lock::lock(int &err) noexcept {
    if (m_owned) return true;

    // Try to lock - set error code on failure
    const bool locked = (sem_trywait(m_semaphore) == 0);
    if (locked) {
        m_owned = true;
    } else {
        err = errno;
    }

    return locked;
}

bool semaphore_lock::unlock() noexcept {
    if (!m_owned) {
        return false;
    }

    // Try to unlock
    const bool unlocked = (sem_post(m_semaphore) == 0);
    if (unlocked) {
        m_owned = false;
    }

    return unlocked;
}

bool semaphore_lock::unlock(int &err) noexcept {
    if (!m_owned) {
        return false;
    }

    // Try to unlock - set error code on failure
    const bool unlocked = (sem_post(m_semaphore) == 0);
    if (unlocked) {
        m_owned = false;
    } else {
        err = errno;
    }

    return unlocked;
}

}  // namespace cpptools
