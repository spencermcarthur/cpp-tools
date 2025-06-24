#include "cpptools/thread.hpp"

#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

namespace tools {

thread::thread(thread&& other) noexcept : core_(other.core_) {
    other.core_ = -1;

    this->name_.swap(other.name_);
    this->swap(other);
}

bool thread::set_name(const std::string_view& name) noexcept {
    if (name.length() <= MAX_NAME_LEN) {
        const int ret = pthread_setname_np(pthread_self(), name_.c_str());
        if (ret == 0) {
            name_ = name;
            return true;
        }
    }

    return false;
}

bool thread::set_core(int core) noexcept {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    const int ret =
        pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    if (ret == 0) {
        core_ = core;
        return true;
    }

    return false;
}

}  // namespace tools
