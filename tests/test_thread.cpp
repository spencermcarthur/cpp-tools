#include <gtest/gtest.h>

#include <thread>
#include <utility>

#include "cpptools/thread.hpp"

using namespace tools;

TEST(thread, default_constructor) {
    EXPECT_NO_THROW(thread{});

    thread t;

    // Not joinable
    EXPECT_FALSE(t.joinable());
}

TEST(thread, name) {
    thread t;

    // Valid name
    const char valid[] = "abc";
    EXPECT_TRUE(t.set_name(valid));

    // Invalid name - max length 16 including null terminator
    const char invalid[] = "abcdefghijklmnop";
    EXPECT_FALSE(t.set_name(invalid));

    // Check name
    EXPECT_EQ(t.name(), valid);

    // Not joinable
    EXPECT_FALSE(t.joinable());
}

TEST(thread, core) {
    thread t;

    // Valid core
    const int valid = 0;
    EXPECT_TRUE(t.set_core(valid));

    // Invalid core - cores numbered 0 thru n-1
    const int invalid = std::thread::hardware_concurrency();
    EXPECT_FALSE(t.set_core(invalid));

    // Check core
    EXPECT_EQ(t.core(), valid);

    // Not joinable
    EXPECT_FALSE(t.joinable());
}

TEST(thread, move_constructor) {
    thread t1;

    // Set core and name
    const int core = 0;
    const char name[] = "abc";
    t1.set_core(core);
    t1.set_name(name);

    // Move-construct t2
    thread t2(std::move(t1));

    // Check expected move behavior
    EXPECT_EQ(t1.core(), -1);
    EXPECT_EQ(t1.name(), "");
    EXPECT_EQ(t2.core(), core);
    EXPECT_EQ(t2.name(), name);

    // Not joinable
    EXPECT_FALSE(t1.joinable());
    EXPECT_FALSE(t2.joinable());
}

TEST(thread, function_and_args_constructor) {
    int res{0};
    const auto f = [&res](int a, int b) { res = a + b; };

    ASSERT_EQ(res, 0);

    thread t1(f, 123, 456);

    // Joinable
    EXPECT_TRUE(t1.joinable());
    EXPECT_NO_FATAL_FAILURE(t1.join());

    // Expected result
    EXPECT_EQ(res, 579);
}
