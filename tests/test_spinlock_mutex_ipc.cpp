#include <gtest/gtest.h>

#include <string>

#include "cpptools/spinlock_mutex_ipc.hpp"

using cpptools::spinlock_mutex_ipc;

const std::string g_name = "/testing";

TEST(spinlock_mutex_ipc, constructor) {
    ASSERT_NO_FATAL_FAILURE(spinlock_mutex_ipc{g_name});

    spinlock_mutex_ipc lock(g_name);
    EXPECT_EQ(lock.name(), g_name);
    EXPECT_EQ(lock.reference_count(), 1);
}

TEST(spinlock_mutex_ipc, construct_multiple) {
    spinlock_mutex_ipc lock1(g_name);
    EXPECT_EQ(lock1.reference_count(), 1);

    {
        spinlock_mutex_ipc lock2(g_name);
        EXPECT_EQ(lock1.reference_count(), 2);
        EXPECT_EQ(lock2.reference_count(), 2);
    }

    EXPECT_EQ(lock1.reference_count(), 1);
}

TEST(spinlock_mutex_ipc, lock_unlock) {
    spinlock_mutex_ipc lock1(g_name);
    EXPECT_EQ(lock1.reference_count(), 1);

    EXPECT_TRUE(lock1.try_lock());

    {
        spinlock_mutex_ipc lock2(g_name);
        EXPECT_EQ(lock1.reference_count(), 2);
        EXPECT_EQ(lock2.reference_count(), 2);

        EXPECT_FALSE(lock2.try_lock());
    }

    EXPECT_EQ(lock1.reference_count(), 1);

    EXPECT_FALSE(lock1.try_lock());
    EXPECT_NO_THROW(lock1.unlock());
}
