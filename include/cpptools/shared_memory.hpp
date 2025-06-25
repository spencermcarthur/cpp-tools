#pragma once

#include <linux/limits.h>

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include "cpptools/macros.hpp"
#include "cpptools/semaphore_lock.hpp"

namespace cpptools {

// Class for managing POSIX shared memory
class shared_memory {
    static constexpr size_t REF_COUNT_OFFSET = CPPTOOLS_CACHELINE_SIZE;

public:
    // See "DESCRIPTION" at
    // https://man7.org/linux/man-pages/man3/shm_open.3.html
    static constexpr size_t MAX_NAME_LEN = NAME_MAX;

    shared_memory(std::string_view shMemName, size_t requestedSize);
    ~shared_memory();

    // No default/copy/move construction
    CPPTOOLS_NO_COPY_OR_MOVE(shared_memory);

    template <typename T>
    [[nodiscard]] T *as_struct() const {
        T *data{nullptr};

        // Make sure that m_Data is mapped, and check that it fits exactly into
        // type T
        if (m_data && sizeof(T) == m_data_size) {
            data = reinterpret_cast<T *>(m_data);
        }

        return data;
    }

    template <typename T>
    [[nodiscard]] std::span<T> as_span() const {
        T *data{nullptr};
        size_t size{0};

        // Make sure that m_Data is mapped
        if (m_data) {
            data = reinterpret_cast<T *>(m_data);
            size = m_data_size;
        }

        return std::span<T>(data, size);
    }

    [[nodiscard]] std::string name() const { return m_name; }
    [[nodiscard]] void *data() const { return m_data; }
    [[nodiscard]] size_t size() const { return m_data_size; }
    [[nodiscard]] int reference_count() const;

private:
    bool open_shared_mem_file(std::string_view name);
    void close_shared_mem_file() noexcept;

    void alloc_shared_mem(std::string_view name);
    void free_shared_mem() noexcept;

    void map_shared_mem(std::string_view name = {});
    void unmap_shared_mem() noexcept;

    std::string m_name;
    int *m_reference_count{nullptr};
    void *m_data{nullptr};
    const size_t m_data_size;
    const size_t m_total_size;
    int m_file_desc{-1};
    semaphore_lock m_semlock;
};

}  // namespace cpptools
