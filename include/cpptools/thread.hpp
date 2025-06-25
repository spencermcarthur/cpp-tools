#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <thread>

#include "cpptools/macros.hpp"

namespace cpptools {

// Nameable, pinnable thread
class thread : public std::thread {
    static constexpr size_t MAX_NAME_LEN = 15;

public:
    thread() noexcept = default;
    ~thread() = default;

    thread(thread&& other) noexcept;

    template <class F, class... Args>
    explicit thread(F&& f, Args&&... args) : std::thread(f, args...) {}

    CPPTOOLS_NO_COPY(thread);

    bool set_name(const std::string_view&) noexcept;
    std::string_view name() const { return name_; }

    bool set_core(int core) noexcept;
    int core() const { return core_; }

private:
    std::string name_;
    int core_{-1};
};

}  // namespace cpptools
