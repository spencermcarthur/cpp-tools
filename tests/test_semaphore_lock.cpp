#include <gtest/gtest.h>
#include <linux/limits.h>

#include <cstring>
#include <stdexcept>

#include "cpptools/semaphore_lock.hpp"

const char* g_valid_name = "/testing";

using cpptools::semaphore_lock;

TEST(semaphore_lock, constructor) {
    // Construct semaphore lock on /testing
    EXPECT_NO_THROW(semaphore_lock{g_valid_name});
}

TEST(semaphore_lock, constructor_fail_if_name_blank) {
    // Invalid name - 0 length
    EXPECT_THROW(semaphore_lock{""}, std::length_error);
}

TEST(semaphore_lock, constructor_fail_if_name_too_long) {
    // Invalid name - too long
    char nameTooLong[semaphore_lock::MAX_SEMAPHORE_NAME_LEN + 2]{};
    std::memset(nameTooLong, 'a', semaphore_lock::MAX_SEMAPHORE_NAME_LEN + 1);
    EXPECT_THROW(semaphore_lock{nameTooLong}, std::length_error);
}

TEST(semaphore_lock, lock_unlock) {
    // Construct 2 semaphore locks on /testing
    semaphore_lock semlock1(g_valid_name);
    semaphore_lock semlock2(g_valid_name);

    // sl1 locks, sl2 can't
    EXPECT_TRUE(semlock1.lock());
    EXPECT_FALSE(semlock2.lock());

    // sl1 unlocks, 2 can lock and unlock
    EXPECT_TRUE(semlock1.unlock());
    EXPECT_TRUE(semlock2.lock());
    EXPECT_TRUE(semlock2.unlock());
}

TEST(semaphore_lock, destructor_release) {
    // Construct and test
    semaphore_lock semlock1(g_valid_name);
    EXPECT_TRUE(semlock1.lock());
    EXPECT_TRUE(semlock1.unlock());

    {
        // Construct another lock and acquire before destroying. Expected when
        // locked is to unlock on destruction.
        semaphore_lock semlock2(g_valid_name);
        EXPECT_TRUE(semlock2.lock());
        EXPECT_FALSE(semlock1.lock());
    }

    // First lock should be able to acquire
    EXPECT_TRUE(semlock1.lock());
    EXPECT_TRUE(semlock1.unlock());
}
