#include "cpptools/spinlock_mutex.hpp"

#include <gtest/gtest.h>

using cpptools::spinlock_mutex;

TEST(spinlock_mutex, constructor) { ASSERT_NO_FATAL_FAILURE(spinlock_mutex{}); }

TEST(spinlock_mutex, lock) {
    spinlock_mutex m;

    // Lock once
    EXPECT_NO_THROW(m.lock());
}

TEST(spinlock_mutex, try_lock) {
    spinlock_mutex m;

    bool acquired;

    // Lock once
    EXPECT_NO_THROW(acquired = m.try_lock());
    EXPECT_TRUE(acquired);

    // Can't lock again
    EXPECT_NO_THROW(acquired = m.try_lock());
    EXPECT_FALSE(acquired);
}

TEST(spinlock_mutex, unlock) {
    spinlock_mutex m;

    // Lock once - can't lock again
    EXPECT_TRUE(m.try_lock());
    EXPECT_FALSE(m.try_lock());

    // Unlock - CAN lock again
    m.unlock();
    EXPECT_TRUE(m.try_lock());
}
