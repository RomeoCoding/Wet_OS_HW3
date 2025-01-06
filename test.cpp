#include "customAllocator.h"
#include <iostream>
#include <cassert>
#include <cstring>

void testCustomMalloc() {
    std::cout << "Testing customMalloc..." << std::endl;

    void* ptr1 = customMalloc(10);
    assert(ptr1 != nullptr);

    void* ptr2 = customMalloc(20);
    assert(ptr2 != nullptr);

    std::cout << "customMalloc test passed!" << std::endl;
}

void testCustomFree() {
    std::cout << "Testing customFree..." << std::endl;

    void* ptr = customMalloc(50);
    assert(ptr != nullptr);

    customFree(ptr);
    std::cout << "customFree test passed!" << std::endl;
}

void testCustomCalloc() {
    std::cout << "Testing customCalloc..." << std::endl;

    int* ptr = static_cast<int*>(customCalloc(5, sizeof(int)));
    assert(ptr != nullptr);

    for (int i = 0; i < 5; ++i) {
        assert(ptr[i] == 0); // Check if memory is zero-initialized
    }

    customFree(ptr);
    std::cout << "customCalloc test passed!" << std::endl;
}

void testCustomRealloc() {
    std::cout << "Testing customRealloc..." << std::endl;

    char* ptr = static_cast<char*>(customMalloc(10));
    assert(ptr != nullptr);

    std::strncpy(ptr, "Test", 10);
    ptr = static_cast<char*>(customRealloc(ptr, 20));
    assert(ptr != nullptr);
    assert(std::strcmp(ptr, "Test") == 0); // Ensure content is preserved

    customFree(ptr);
    std::cout << "customRealloc test passed!" << std::endl;
}

int main() {
    testCustomMalloc();
    testCustomFree();
    testCustomCalloc();
    testCustomRealloc();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
