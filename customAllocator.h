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

#define _BSD_SOURCE
#define _XOPEN_SOURCE 550
#define _XOPEN_SOURCE_EXTENDED
#define _SVID_SOURCE


/*==========================================================
More includes
==========================================================
*/
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h> 
#include <errno.h>
/*=============================================================================
* Block
=============================================================================*/
//suggestion for block usage - feel free to change this

typedef struct Block
{
    size_t size;
    struct Block* next;
    struct Block* prev;
    bool free;
    char user_data[];
} Block;

typedef struct BlockList
{
    struct Block* head;
    struct Block* tail;
} BlockList;

//extern struct BlockList blocklist;

// addtional functions 

struct Block* Find_And_Allocate(size_t size);
struct Block* Find_Block_ptr(void* ptr);
struct Block* Merge_Block(struct Block* block);
bool Combine_with(struct Block* block);
struct Block* Allocate_Extenend_Heap(size_t size);
struct Block* Find_BestFit(size_t size, struct Block* head);
void customFree(void* ptr);
void Free_All_Memory();
#endif // CUSTOM_ALLOCATOR
