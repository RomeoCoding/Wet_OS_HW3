#ifndef __CUSTOM_ALLOCATOR__
#define __CUSTOM_ALLOCATOR__


/*=============================================================================
* do no edit lines below!
=============================================================================*/
#include <stddef.h> //for size_t

void* customMalloc(size_t size);
void customFree(void* ptr);
void* customCalloc(size_t nmemb, size_t size);
void* customRealloc(void* ptr, size_t size);
/*=============================================================================
* do no edit lines above!
=============================================================================*/

//suggestion for block usage - feel free to change this
typedef struct Block
{
    size_t size;
    struct Block* next;
    bool free;
} Block;
extern Block* blockList;


/*
========================
==== Util functions ====
========================
*/

//inline size_t alignSize(size_t size);
/*static Block* findBestFitBlock(size_t size);
static Block* extendHeapForBlock(size_t size);
static void splitBlock(Block* block, size_t size);
static void coalesceFreeBlocks();
static void shrinkHeap();*/

/*=============================================================================
* Utility Functions
=============================================================================*/
inline size_t alignSize(size_t size);
Block* findBestFitBlock(size_t size);
Block* extendHeapForBlock(size_t size);
void splitBlock(Block* block, size_t size);
void coalesceFreeBlocks();
void shrinkHeap();


/*=============================================================================
* if writing bonus - uncomment lines below
=============================================================================*/
// #ifndef __BONUS__
// #define __BONUS__
// #endif
// void* customMTMalloc(size_t size);
// void customMTFree(void* ptr);
// void* customMTCalloc(size_t nmemb, size_t size);
// void* customMTRealloc(void* ptr, size_t size);

// void heapCreate();
// void heapKill();

/*=============================================================================
* defines
=============================================================================*/
#define SBRK_FAIL (void*)(-1)
#define ALIGN_TO_MULT_OF_4(x) (((((x) - 1) >> 2) << 2) + 4)

/*=============================================================================
* Block
=============================================================================*/

#endif // CUSTOM_ALLOCATOR
