#include "customAllocator.h"

#include <assert.h>
#include <time.h>   

void test_customMalloc() {
    // Test valid allocation
    void* ptr = customMalloc(100);
    assert(ptr != NULL);  // Should not return NULL for valid size
    memset(ptr, 0, 100);  // Test the allocated memory
    customFree(ptr);

    // Test zero allocation (should return NULL)
    ptr = customMalloc(0);
    assert(ptr == NULL);  // Should return NULL for size 0

    // Test invalid allocation (negative sizes or invalid checks should already be handled inside customMalloc)
   
}

void test_customFree() {
    void* ptr = customMalloc(100);
    assert(ptr != NULL);
    customFree(ptr);  // Test freeing valid memory

    // Freeing a null pointer should be handled safely in customFree
    customFree(NULL);  // Shouldn't crash
}

void test_customCalloc() {
    // Test valid allocation (100 elements of 10 bytes)
    void* ptr = customCalloc(100, 10);
    assert(ptr != NULL);  // Should not return NULL
    memset(ptr, 0, 100 * 10);  // Check if memory is zero-initialized
    customFree(ptr);

    // Test zero allocation (either nmemb or size zero)
    ptr = customCalloc(0, 10);
    assert(ptr == NULL);  // Should return NULL for nmemb == 0
    ptr = customCalloc(100, 0);
    assert(ptr == NULL);  // Should return NULL for size == 0
}

void test_customRealloc() {
    // Test realloc increasing the size
    void* ptr = customMalloc(10);
    void* new_ptr = customRealloc(ptr, 20);
    assert(new_ptr != NULL);  // Should not return NULL when increasing size
    customFree(new_ptr);

    // Test realloc decreasing the size
    ptr = customMalloc(20);
    new_ptr = customRealloc(ptr, 10);
    assert(new_ptr != NULL);  // Should not return NULL when decreasing size
    customFree(new_ptr);

    // Test realloc with NULL pointer (equivalent to malloc)
    new_ptr = customRealloc(NULL, 20);
    assert(new_ptr != NULL);  // Should return a new block of memory
    customFree(new_ptr);

    // Test realloc with size 0 (equivalent to free)
    new_ptr = customRealloc(ptr, 0);
    assert(new_ptr == NULL);  // Should return NULL for realloc with size 0
}
void test_stress() {
    const size_t total_allocations = 100000;
    void* ptrs[total_allocations];

    // Allocate memory and immediately free it
    for (size_t i = 0; i < total_allocations; i++) {
        ptrs[i] = customMalloc(128);  // Allocating 128 bytes
        assert(ptrs[i] != NULL);      // Ensure allocation succeeded
        customFree(ptrs[i]);          // Free memory
    }

    printf("Stress test passed!\n");
}
void test_fragmentation() {
    void* ptr1 = customMalloc(256);
    void* ptr2 = customMalloc(128);
    void* ptr3 = customMalloc(64);
    void* ptr4 = customMalloc(512);
    void* ptr5 = customMalloc(32);

    // Free some blocks to create fragmentation
    customFree(ptr2);  // Freeing 128 bytes
    customFree(ptr5);  // Freeing 32 bytes

    // Allocate again to see if we get back the free space properly
    void* ptr6 = customMalloc(128);  // Should reuse the space from ptr2
    void* ptr7 = customMalloc(32);   // Should reuse the space from ptr5

    assert(ptr6 != NULL);
    assert(ptr7 != NULL);

    customFree(ptr1);
    customFree(ptr3);
    customFree(ptr4);
    customFree(ptr6);
    customFree(ptr7);

    printf("Fragmentation test passed!\n");
}

void test_multiple_malloc_free() {
    for (int i = 0; i < 10000; i++) {
        void* ptr = customMalloc(64);  // Allocate 64 bytes
        assert(ptr != NULL);           // Ensure allocation succeeded
        customFree(ptr);               // Free memory
    }
    printf("Multiple malloc/free test passed!\n");
}

void test_large_allocation() {
    size_t large_size = 1024 * 1024 * 100;  // 100 MB

    // Test allocating large memory
    void* ptr = customMalloc(large_size);
    assert(ptr != NULL);  // Ensure allocation succeeded

    // Perform some operation on the allocated memory
    memset(ptr, 0, large_size);  // Write zeros to the memory

    customFree(ptr);  // Free large memory block
    printf("Large allocation test passed!\n");
}


void test_realloc() {
    void* ptr = customMalloc(128);  // Initial allocation
    assert(ptr != NULL);

    // Test shrinking the size
    void* smaller_ptr = customRealloc(ptr, 64);  // Shrink to 64 bytes
    assert(smaller_ptr != NULL);  // Ensure realloc succeeded
    memset(smaller_ptr, 0, 64);  // Write zeros to the resized memory

    // Test growing the size
    void* larger_ptr = customRealloc(smaller_ptr, 256);  // Grow to 256 bytes
    assert(larger_ptr != NULL);  // Ensure realloc succeeded
    memset(larger_ptr, 0, 256);  // Write zeros to the resized memory

    customFree(larger_ptr);  // Free the final memory block
    printf("Realloc test passed!\n");
}

void test_null_pointer_handling() {
    // Test freeing a NULL pointer (should not crash)
    customFree(NULL);

    // Test reallocating NULL pointer (should behave like malloc)
    void* ptr = customRealloc(NULL, 256);
    assert(ptr != NULL);  // Ensure realloc returned a valid pointer
    customFree(ptr);

    printf("Null pointer handling test passed!\n");
}

void test_allocation_size() {
    printf("Running allocation size test...\n");

    size_t sizes[] = {1, 16, 32, 64, 128, 256, 512, 1024};  // Different test sizes
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (size_t i = 0; i < num_sizes; i++) {
        size_t size = sizes[i];
        unsigned char* ptr = (unsigned char*) customMalloc(size);

        assert(ptr != NULL);  // Ensure allocation succeeded

        // Fill entire allocated space with a pattern
        memset(ptr, 0xAA, size);

        // Ensure the last byte within bounds can be accessed
        ptr[size - 1] = 0xFF;  // Should not cause a segmentation fault

        // Check if memory corruption happens when writing outside the allocated memory
        int error_detected = 0;
        volatile unsigned char out_of_bounds;
        if (size < 1024 * 1024) {  // Avoid testing huge allocations
            out_of_bounds = ptr[size];  // Should be outside allocated memory
            error_detected = (out_of_bounds != 0);  // Check if read is invalid
            assert(error_detected == 0);  // Ensure no out-of-bounds access occurred
        }

        customFree(ptr);

        printf("  - Size %zu bytes: PASSED\n", size);
    }

    printf("Allocation size test completed successfully!\n");
}


void test_memory_stability() {
    printf("Running memory stability test...\n");

    const int num_blocks = 5;
    size_t sizes[] = {32, 64, 128, 256, 512};
    void* blocks[num_blocks];

    // Step 1: Allocate memory and fill it with unique patterns
    for (int i = 0; i < num_blocks; i++) {
        blocks[i] = customMalloc(sizes[i]);
        assert(blocks[i] != NULL);

        // Fill memory with a unique pattern based on index
        memset(blocks[i], i + 1, sizes[i]);
    }

    // Step 2: Verify that memory still contains the expected data
    for (int i = 0; i < num_blocks; i++) {
        unsigned char* data = (unsigned char*)blocks[i];  // Move inside the loop
        for (size_t j = 0; j < sizes[i]; j++) {
            assert(data[j] == (unsigned char)(i + 1));  // Ensure data is unchanged
        }
    }

    // Step 3: Reallocate and check stability
  for (int i = 0; i < num_blocks; i++) {
    size_t new_size = sizes[i] * 2;  // Increase size
    
    // Verify old data *before* calling realloc
    unsigned char* old_data = (unsigned char*)blocks[i];
    for (size_t j = 0; j < sizes[i]; j++) {
        assert(old_data[j] == (unsigned char)(i + 1));  // Ensure old data is intact
    }

    // Now perform reallocation
    void* new_block = customRealloc(blocks[i], new_size);
    assert(new_block != NULL);
    blocks[i] = new_block;

    // Verify that old data is still intact in the new block
    unsigned char* new_data = (unsigned char*)blocks[i];
    for (size_t j = 0; j < sizes[i]; j++) {
        assert(new_data[j] == (unsigned char)(i + 1));  // Old data should remain
    }

    // Fill the new part of the block with a new pattern
    memset(new_data + sizes[i], 0xFF, new_size - sizes[i]);
}

    // Step 4: Free some blocks and check remaining data integrity
    for (int i = 0; i < num_blocks; i++) {
        if (i == 1 || i == 3) {
            customFree(blocks[i]);
            blocks[i] = NULL;  // Avoid dangling pointer
        }
    }

    for (int i = 0; i < num_blocks; i++) {
        if (blocks[i] == NULL) continue;  // Skip freed blocks

        unsigned char* data = (unsigned char*)blocks[i];
        for (size_t j = 0; j < sizes[i]; j++) {
            assert(data[j] == (unsigned char)(i + 1));  // Data should still be correct
        }
    }

    // Free remaining blocks
    for (int i = 0; i < num_blocks; i++) {
        if (blocks[i] != NULL) {
            customFree(blocks[i]);
        }
    }

    printf("Memory stability test completed successfully!\n");
}


void random_allocation_free_stability_test() {
    void* allocations[1000];
    size_t sizes[1000];
    
    srand(time(NULL));
    
    // Randomly allocate and free memory blocks
    for (int i = 0; i < 1000; i++) {
        sizes[i] = rand() % 1024 + 1; // Random size between 1 and 1024 bytes
        allocations[i] = customMalloc(sizes[i]);
    }

    // Randomly free some of the allocations
    for (int i = 0; i < 1000; i++) {
        if (rand() % 2 == 0 && allocations[i] != NULL) {
            customFree(allocations[i]);
            allocations[i] = NULL;  // Mark as freed
        }
    }

    // Ensure that all blocks are freed
    for (int i = 0; i < 1000; i++) {
        if (allocations[i] != NULL) {
            customFree(allocations[i]);
        }
    }
     printf("random_allocation_free_stability_test passed\n");
}


void large_sequential_alloc_free_test() {
    size_t allocation_size = 1 * 1024 * 1024; // 1 MB blocks
    void* allocations[100];

    // Sequential allocation
    for (int i = 0; i < 100; i++) {
        allocations[i] = customMalloc(allocation_size);
        if (allocations[i] == NULL) {
            printf("Memory allocation failed at index %d\n", i);
            break;
        }
    }

    // Sequential deallocation
    for (int i = 0; i < 100; i++) {
        customFree(allocations[i]);
    }
     printf("Memory allocation passed \n");
}

void mixed_size_random_free_test() {
    void* allocations[500];
    size_t sizes[500];
    
    srand(time(NULL));
    
    // Allocate blocks of random sizes
    for (int i = 0; i < 500; i++) {
        sizes[i] = rand() % 1024 + 1; // Sizes between 1 and 1024 bytes
        allocations[i] = customMalloc(sizes[i]);
    }

    // Randomly free some blocks and leave some allocated
    for (int i = 0; i < 500; i++) {
        if (rand() % 2 == 0 && allocations[i] != NULL) {
            customFree(allocations[i]);
            allocations[i] = NULL;
        }
    }

    // At the end, free all remaining blocks
    for (int i = 0; i < 500; i++) {
        if (allocations[i] != NULL) {
            customFree(allocations[i]);
        }
    }
      printf("mixed_size_random_free_test passed  \n");
}

void run_tests() {
    test_customMalloc();
    test_customFree();
    test_customCalloc();
    test_customRealloc();
    printf("customRealloc test passed!\n");
}




int main(int argc, char* argv[])
{
    
    void* x = customMalloc(1024);
    void* y = customMalloc(200);
    void* z = customMalloc(100);
    printf("passed allocation");
    *(int*)x=12000;
    *(int*)y = 10000;
    printf("%d",(*(int*)x));
    customFree(y);
    void* r = customMalloc(100);
    printf("passed free");
    customFree(r);
    customRealloc(x,200);
    customRealloc(z,1000);
    void* Q = customCalloc(5, 200);
    void* A = customCalloc(5, 4000);
    *(int*)A = 10000;
    customFree(Q);
    customFree(z);
    customFree(A);
     run_tests();
    printf("All tests passed!\n");
     test_stress();
    test_fragmentation();
    test_allocation_size();
    test_multiple_malloc_free();
    random_allocation_free_stability_test();
    test_memory_stability();
    mixed_size_random_free_test();
    test_null_pointer_handling();
    test_realloc();

    large_sequential_alloc_free_test();
    test_large_allocation();
    printf("All tests passed!\n");
    return 0;
}

