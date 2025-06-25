#include "cpptools/shared_memory.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string_view>

#include "cpptools/semaphore_lock.hpp"

#define SCESV static constexpr std::string_view

namespace cpptools {

shared_memory::shared_memory(const std::string_view name,
                             const size_t requestedSize)
    : m_data_size(requestedSize),
      m_total_size(requestedSize + REF_COUNT_OFFSET),
      m_semlock(name) {
    // Validate args
    if (name.empty() || name.length() > MAX_NAME_LEN) {
        throw std::length_error(
            std::format("Shared memory name \"{}\" of length {} is invalid: "
                        "length must be in range [1, {}]\n",
                        name, name.length(), MAX_NAME_LEN));
    }
    if (requestedSize == 0) {
        throw std::logic_error("Requested 0 bytes of shared memory");
    }

    // If we can't open shared memory at m_Name
    if (!open_shared_mem_file(name)) {
        // Try to create it
        alloc_shared_mem(name);

        // If we succeeded in creating it (i.e. didn't crash) but still can't
        // open it, then fail
        if (!open_shared_mem_file(name)) {
            const int err = errno;
            throw std::runtime_error(
                std::format("Failed to open shared memory \"{}\": {}", name,
                            strerror(err)));
        }
    }

    // Map the data to our virtual memory
    map_shared_mem(name);

    // Copy name
    m_name = name;

    // Increment ref counter
    const std::atomic_ref<int> refCounter(*m_reference_count);
    refCounter.fetch_add(1, std::memory_order_release);
}

shared_memory::~shared_memory() {
    // Check before dereferencing
    if (m_reference_count != nullptr) {
        const std::atomic_ref<int> refCounter(*m_reference_count);

        // Decrement ref counter, and capture value before CAS operation
        const int refCount =
            refCounter.fetch_sub(1, std::memory_order_release) - 1;

        // Unmap from process virt mem
        unmap_shared_mem();

        // If ref count is 0, schedule free
        if (refCount == 0) {
            free_shared_mem();
        }
    }

    // Close file handle
    close_shared_mem_file();
}

int shared_memory::reference_count() const {
    int refCount{-1};
    if (m_reference_count != nullptr) {
        refCount = std::atomic_ref<int>(*m_reference_count)
                       .load(std::memory_order_acquire);
    }
    return refCount;
}

bool shared_memory::open_shared_mem_file(std::string_view name) {
    // Try to open shared memory file
    const int file_desc = shm_open(name.data(), O_RDWR, S_IRUSR + S_IWUSR);
    if (file_desc == -1) {
        // Failed
        const int err = errno;
        std::cerr << std::format("Failed to open shared memory \"{}\": {}\n",
                                 m_name, strerror(err));
        return false;
    }

    // Get file info
    struct stat buf;
    if (fstat(file_desc, &buf) == -1) {
        // Failed
        const int err = errno;
        throw std::runtime_error(std::format(
            "fstat failed for shared memory {}: {}", name, strerror(err)));
    }

    // Check that existing shared memory's size is what's expected
    if (static_cast<size_t>(buf.st_size) != m_total_size) {
        throw std::runtime_error(
            std::format("Shared memory \"{}\" exists, but size does not match: "
                        "{} requested vs. {} existing",
                        name, m_total_size, buf.st_size));
    }

    m_file_desc = file_desc;
    return true;
}

void shared_memory::close_shared_mem_file() noexcept {
    if (close(m_file_desc) == -1) {
        // Failed
        const int err = errno;
        std::cerr << std::format("Failed to close shared memory \"{}\": {}\n",
                                 m_name, strerror(err));
        return;
    }

    m_file_desc = -1;
}

void shared_memory::alloc_shared_mem(std::string_view name) {
    // Try to lock semaphore
    if (!m_semlock.lock()) {
        return;
    }

    // Create new shared memory in system
    const int fileDesc =
        shm_open(name.data(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR + S_IWUSR);
    if (fileDesc == -1) {
        // Failed
        const int err = errno;
        throw std::runtime_error(std::format(
            "Failed to create shared memory \"{}\": {}", name, strerror(err)));
    }

    // Allocate m_Size bytes
    if (ftruncate(fileDesc, m_total_size) == -1) {
        // Failed
        const int err = errno;
        throw std::runtime_error(
            std::format("Failed to allocate shared memory \"{}\": {}", name,
                        strerror(err)));
    }

    m_semlock.unlock();
}

void shared_memory::free_shared_mem() noexcept {
    // Try to lock semaphore before unlinking
    if (!m_semlock.lock()) {
        std::cerr << std::format(
            "Failed to free shared memory \"{}\": can't lock semaphore\n",
            m_name);
        return;
    }

    if (shm_unlink(m_name.c_str()) == -1) {
        // Failed
        const int err = errno;
        std::cerr << std::format("Failed to free shared memory \"{}\": {}\n",
                                 m_name, strerror(err));
        return;
    }

    m_semlock.unlock();
}

void shared_memory::map_shared_mem(std::string_view name) {
    // Map shared memory to our process's virtual memory
    void *data = mmap(nullptr, m_total_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                      m_file_desc, 0);
    if (data == MAP_FAILED) {
        // Failed to map
        const int err = errno;
        throw std::runtime_error(std::format(
            "Failed to map shared memory \"{}\": {}", name, strerror(err)));
    }

    m_reference_count = reinterpret_cast<int *>(data);
    m_data = reinterpret_cast<void *>(m_reference_count + REF_COUNT_OFFSET);
}

void shared_memory::unmap_shared_mem() noexcept {
    if (munmap(reinterpret_cast<void *>(m_reference_count), m_total_size) !=
        0) {
        // Failed to unmap
        const int err = errno;
        std::cerr << std::format("Failed to unmap shared memory \"{}\": {}\n",
                                 m_name, strerror(err));
        return;
    }

    m_reference_count = nullptr;
    m_data = nullptr;
}

}  // namespace cpptools
