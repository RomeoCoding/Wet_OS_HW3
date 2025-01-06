#include "customAllocator.h"
#include <cstddef>  
#include <cstring>  
#include <iostream> 
#include <unistd.h> 

Block* blockList = nullptr;

/*
========================
==== Utility Functions ==
========================
*/

// Align size to the nearest multiple of 4
inline size_t alignSize(size_t size) {
    return ALIGN_TO_MULT_OF_4(size);
}

// Find the best fit block for the requested size
Block* findBestFitBlock(size_t size) {
    Block* best = nullptr;
    for (Block* current = blockList; current; current = current->next) {
        if (current->free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
            }
        }
    }
    return best;
}

// Extend the heap and create a new block
Block* extendHeapForBlock(size_t size) {
    void* mem = sbrk(size + sizeof(Block));
    if (mem == SBRK_FAIL) {
        std::cerr << "<sbrk error>: out of memory\n";
        return nullptr;
    }

    auto* newBlock = static_cast<Block*>(mem);
    newBlock->size = size;
    newBlock->free = false;
    newBlock->next = nullptr;
    return newBlock;
}

// Split a block if it's larger than the requested size
void splitBlock(Block* block, size_t size) {
    if (block->size > size + sizeof(Block)) {
        auto* newBlock = reinterpret_cast<Block*>(
            reinterpret_cast<char*>(block) + sizeof(Block) + size);
        newBlock->size = block->size - size - sizeof(Block);
        newBlock->free = true;
        newBlock->next = block->next;

        block->size = size;
        block->next = newBlock;
    }
}

// Coalesce adjacent free blocks
void coalesceFreeBlocks() {
    for (Block* current = blockList; current && current->next;) {
        if (current->free && current->next->free) {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Shrink the heap if the last block is free
void shrinkHeap() {
    if (!blockList) return;

    Block* current = blockList;
    Block* prev = nullptr;

    while (current->next) {
        prev = current;
        current = current->next;
    }

    if (current->free) {
        if (prev) {
            prev->next = nullptr;
        } else {
            blockList = nullptr;
        }

        if (sbrk(-(current->size + sizeof(Block))) == SBRK_FAIL) {
            std::cerr << "<sbrk error>: failed to shrink heap\n";
        }
    }
}

/*
========================
=== Required Functions ==
========================
*/

void* customMalloc(size_t size) {
    if (size == 0) {
        std::cerr << "<malloc error>: passed nonpositive size\n";
        return nullptr;
    }

    size = alignSize(size);

    Block* block = findBestFitBlock(size);
    if (block) {
        block->free = false;
        splitBlock(block, size);
    } else {
        block = extendHeapForBlock(size);
        if (!block) return nullptr;

        if (!blockList) {
            blockList = block;
        } else {
            Block* current = blockList;
            while (current->next) {
                current = current->next;
            }
            current->next = block;
        }
    }

    return reinterpret_cast<void*>(reinterpret_cast<char*>(block) + sizeof(Block));
}

void customFree(void* ptr) {
    if (!ptr) {
        std::cerr << "<free error>: passed null pointer\n";
        return;
    }

    auto* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(ptr) - sizeof(Block));
    block->free = true;

    coalesceFreeBlocks();
    shrinkHeap();
}

void* customCalloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) {
        std::cerr << "<calloc error>: passed nonpositive value\n";
        return nullptr;
    }

    size_t totalSize = nmemb * size;
    void* ptr = customMalloc(totalSize);
    if (ptr) {
        std::memset(ptr, 0, totalSize);
    }
    return ptr;
}

void* customRealloc(void* ptr, size_t size) {
    if (!ptr) return customMalloc(size);
    if (size == 0) {
        customFree(ptr);
        return nullptr;
    }

    auto* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(ptr) - sizeof(Block));
    if (block->size >= size) {
        splitBlock(block, alignSize(size));
        return ptr;
    }

    void* newPtr = customMalloc(size);
    if (newPtr) {
        std::memcpy(newPtr, ptr, block->size);
        customFree(ptr);
    }
    return newPtr;
}

