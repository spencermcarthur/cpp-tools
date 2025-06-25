#include <fcntl.h>
#include <gtest/gtest.h>
#include <linux/limits.h>
#include <sys/mman.h>

#include <cstddef>
#include <cstring>
#include <format>
#include <span>
#include <stdexcept>
#include <string_view>

#include "cpptools/shared_memory.hpp"

using cpptools::shared_memory;
const char *g_valid_name = "/testing";
const size_t g_size = 1024 * 1024;  // 1 MiB

// convenience functions
bool shared_mem_exists(const char *name);
void request_shared_mem(const char *name, size_t size);
void free_shared_mem(const char *name);

TEST(shared_memory, constructor_new_memory) {
    // Make sure shared memory doesn't exist
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Create shared memory and verify that everything was initialized as
    // expected
    {
        shared_memory shmem(g_valid_name, g_size);

        EXPECT_STREQ(g_valid_name, shmem.name().c_str());
        EXPECT_EQ(g_size, shmem.size());
        EXPECT_EQ(shmem.reference_count(), 1);

        // Check that shared memory was created
        EXPECT_TRUE(shared_mem_exists(g_valid_name));
    }
}

TEST(shared_memory, constructor_existing_memory) {
    // Make sure shared memory doesn't exist
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Create an instance of shared_memory
    shared_memory shmem1(g_valid_name, g_size);

    // Check that shared memory was created
    EXPECT_TRUE(shared_mem_exists(g_valid_name));

    // Make sure we are the only instance
    EXPECT_EQ(shmem1.reference_count(), 1);

    {
        // Create a second instance
        shared_memory shmem2(g_valid_name, g_size);

        // Verify name/size
        EXPECT_STREQ(g_valid_name, shmem2.name().c_str());
        EXPECT_EQ(g_size, shmem2.size());

        // Make sure ref count increased
        EXPECT_EQ(shmem2.reference_count(), 2);
    }

    // Make sure we didn't free the memory
    EXPECT_TRUE(shared_mem_exists(g_valid_name));

    // Make sure ref count decreased
    EXPECT_EQ(shmem1.reference_count(), 1);
}

TEST(shared_memory, destructor_free_memory_on_ref_count_zero) {
    // Make sure shared memory doesn't exist
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    {
        // Create shared memory
        shared_memory shmem(g_valid_name, g_size);

        // Check that shared memory was created
        EXPECT_TRUE(shared_mem_exists(g_valid_name));

        // Verify that we have the only handle
        EXPECT_EQ(shmem.reference_count(), 1);
    }

    // Check that shared memory was unlinked again when shmem was destroyed
    EXPECT_FALSE(shared_mem_exists(g_valid_name));
}

TEST(shared_memory, constructor_fail_invalid_size_requested) {
    // Make sure shared memory doesn't exist
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Size 0
    EXPECT_THROW(shared_memory(g_valid_name, 0), std::logic_error);

    // Check that shared memory was not created
    EXPECT_FALSE(shared_mem_exists(g_valid_name));
}

TEST(shared_memory, constructor_fail_existing_memory_wrong_size) {
    // Make sure shared memory doesn't exist
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Create an instance of shared_memory
    shared_memory shmem1(g_valid_name, g_size);

    // Check that shared memory was created
    EXPECT_TRUE(shared_mem_exists(g_valid_name));

    // Make sure we are the only instance
    EXPECT_EQ(shmem1.reference_count(), 1);

    // Try to create a second instance with same name, different size
    EXPECT_THROW(shared_memory(g_valid_name, g_size + 1), std::runtime_error);
}

TEST(shared_memory, constructor_fail_invalid_name) {
    // Make sure shared memory doesn't exist
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Try to create with blank name
    EXPECT_THROW(shared_memory("", g_size), std::length_error);

    // Try to create with name too long
    char name[shared_memory::MAX_NAME_LEN + 2]{};
    std::memset(name, 'a', shared_memory::MAX_NAME_LEN + 1);
    EXPECT_THROW(shared_memory(name, g_size), std::length_error);
}

TEST(shared_memory, construct_multiple) {
    // Unlink shared memory if it already exists
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }

    // Make sure it doesn't exist
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Construct multiple shared memory regions and verify that reference
    // counter is increasing and decreasing as expected
    {
        shared_memory shmem1(g_valid_name, g_size);
        EXPECT_EQ(shmem1.reference_count(), 1);

        {
            shared_memory shmem2(g_valid_name, g_size);
            EXPECT_EQ(shmem1.reference_count(), 2);

            {
                shared_memory shmem3(g_valid_name, g_size);
                EXPECT_EQ(shmem1.reference_count(), 3);
            }

            EXPECT_EQ(shmem1.reference_count(), 2);
        }

        EXPECT_EQ(shmem1.reference_count(), 1);
    }

    // Check that shared memory was unlinked again when shmem1 was destroyed
    EXPECT_FALSE(shared_mem_exists(g_valid_name));
}

TEST(shared_memory, construct_multiple_verify_memory_actually_shared) {
    // Unlink shared memory if it already exists
    if (shared_mem_exists(g_valid_name)) {
        free_shared_mem(g_valid_name);
    }

    // Make sure it doesn't exist
    ASSERT_FALSE(shared_mem_exists(g_valid_name));

    // Construct multiple shared memory regions and verify that the memory is
    // changed when written
    shared_memory shmem1(g_valid_name, g_size);
    EXPECT_EQ(shmem1.reference_count(), 1);

    // Get a span
    std::span<std::byte> span1 = shmem1.as_span<std::byte>();

    // Verify memory initialized correctly
    EXPECT_EQ(span1.size(), g_size);
    for (std::byte byte : span1) {
        EXPECT_EQ(byte, std::byte('\0'));
    }

    // Set the memory to 'a' and verify
    std::memset(span1.data(), 'a', span1.size());
    for (std::byte byte : span1) {
        EXPECT_EQ(byte, std::byte('a'));
    }

    {
        // Create new shared memory on the same data
        shared_memory shmem2(g_valid_name, g_size);
        EXPECT_EQ(shmem1.reference_count(), 2);
        EXPECT_EQ(shmem2.reference_count(), 2);

        // Get a another span
        std::span<std::byte> span2 = shmem1.as_span<std::byte>();
        EXPECT_EQ(span1.size(), span2.size());

        // Verify the data is all 'a'
        for (std::byte byte : span2) {
            EXPECT_EQ(byte, std::byte('a'));
        }

        // Set the data to 'b'
        std::memset(span2.data(), 'b', span2.size());

        // Verify
        for (std::byte byte : span2) {
            EXPECT_EQ(byte, std::byte('b'));
        }
    }

    // shmem2 goes out of scope
    EXPECT_EQ(shmem1.reference_count(), 1);

    // Verify span1 was updated to 'b'
    for (std::byte byte : span1) {
        EXPECT_EQ(byte, std::byte('b'));
    }
}

// Testing utils
bool shared_mem_exists(const char *name) {
    bool exists{true};
    int fileDesc = shm_open(name, O_RDONLY, 0);
    if (fileDesc == -1 && errno == ENOENT) {
        exists = false;
    }
    close(fileDesc);
    return exists;
}

void request_shared_mem(const char *name, size_t size) {
    int fileDesc = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0);
    if (fileDesc == -1) {
        return;
    }

    if (ftruncate(fileDesc, size) == -1) {
        throw std::runtime_error(std::format(
            "({}:{}) Failed to allocate memory for shared memory {}", __FILE__,
            __LINE__, name));
    }
}

void free_shared_mem(const char *name) { shm_unlink(name); }
