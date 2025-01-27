#include "customAllocator.h"

static BlockList blockList = {NULL, NULL};

void* customMalloc(size_t size)
{
    if(size <= 0)
    {
        perror("<malloc error>: passed nonpositive size");
    }
    size_t real_size = ALIGN_TO_MULT_OF_4(size);
    void* ptr = Find_And_Allocate(real_size);
    if(ptr == (void*)-1)
    {
        //error
        printf("creating new block\n");
        fflush(stdout);
         ptr = sbrk(sizeof(Block)+size);
         if(ptr == (void*)-1)
         {
            perror("sbrk failed");
         }
         blockList.tail = (struct Block*)ptr;
    }

    struct Block* new_block = (struct Block*)ptr;
    if(blockList.head == NULL)
    {
        blockList.head=new_block;
    }
    return (void*)new_block -> user_data;
}



void customFree(void* ptr)
{

    if(ptr == NULL)
    {
        perror("<free error>: passed null pointer");
    }
    struct Block* block = Find_Block_ptr(ptr);
    if(block == NULL)
    {
        perror("<free error>: cant find the pointer");
    //ptr doesnt exist
    //return error
    }
    
    block->free = true;
    block = Merge_Block(block);
    if(block->next == NULL)
    {
        sbrk(-(sizeof(struct Block) + block->size));
        if( block->prev != NULL )
        {
            block->prev->next = NULL;
        }
    }
//change program    heap to less`:w

 }



void* customCalloc(size_t nmeb, size_t size)
{

    if( size<=0 || nmeb <=0 )
    {
        //error
    }
    void *ptr = customMalloc(nmeb*size);
    if(ptr == NULL)
    {
        //error
    }
    struct Block* block=(struct Block*)ptr;
    memset(block->user_data,0,nmeb*size);
    return block->user_data;
}




void* customRealloc(void* ptr, size_t size)
{

    if(ptr == NULL)
    {
        return customMalloc(size);
    }
    struct Block* block = Find_Block_ptr(ptr);
    if(block == NULL)
    {
    //ptr doesnt exist
    //return error
    }
    if(size == block->size)
    {
        return (void*)(block->user_data);
    }
    size_t block_size = sizeof(struct Block);
    size_t current_size = block->size;
    size_t old_size = block->size;

    if( block->prev->free)
        current_size += block->prev->size+ block_size;
    if( block->next->free)
        current_size += block->next->size + block_size;
    
    
    if(current_size - size > block_size + 4)
    {
       block = Merge_Block(block);
       if((void*)(block->user_data) != ptr)
        {   
            memcpy(block,ptr,old_size);
        }
        struct Block* new_block  = (struct Block*)(block + size);
        new_block -> next = block->next;
        new_block -> prev = block;
        new_block -> free = true;
        new_block -> size = current_size - size - sizeof(struct Block);
        block -> next = new_block;
        block -> size = size;
        return (void*)(block->user_data);
    }else{
        void* ptr = customMalloc(size);
        memcpy(ptr,block->user_data,block->size);
        block->free = true;
        Merge_Block(block);
        return ptr;
    }

}











void* Find_And_Allocate(size_t size)
{
//find best fit
    struct Block* block = blockList.head;
    struct Block* tail = blockList.tail;
    if(block == NULL)
    {
        return NULL;
    }
    struct Block* min_block = NULL;
    size_t min_size = 0;
   
    while( 1 )
    {
        if( block->free == true && block->size >= size )
        {
            if( min_size == 0 )
            {
                min_size = block->size;
                min_block = block;
            }else{
                if( min_size > block->size )
                {
                    min_size = block->size;
                    min_block = block;
                }
            }
        }
        if( block == tail )
        {
            break;
        }
        block = block->next;
        
    }

    if( size == 0 )
    {
        return NULL;
    }
//end of find best fit


//resize the best fit to prevent wasting space

    if( min_size - size >= sizeof(struct Block)  + 4)
    {
        struct Block* new_block  = (struct Block*)(min_block + size);
        new_block -> next = min_block->next;
        new_block -> prev = min_block;
        new_block -> free = true;
        new_block -> size = min_size - size - sizeof(struct Block);

        min_block -> size = size;
        min_block -> next = new_block;
    }
    min_block->free = false;
    return (void*)min_block->user_data;
}





struct Block* Find_Block_ptr(void* ptr)
{
    struct Block* block = blockList.head;
    struct Block* tail = blockList.tail;
    while( 1 )
    {
        if( (void*)(block->user_data) == ptr ) 
        {
            return block;
        }

        if( block == tail )
        {
            return NULL;
        }

        block = block->next;
    }

}


struct Block* Merge_Block(struct Block* block)
{
    struct Block* prev_block = block->prev;
    if(prev_block->free)
    {
        if( Combine_with(prev_block))
            block = prev_block;
    }
    struct Block* next_block = block->next;
    if(next_block->free)
    {
       Combine_with(block);
    } 
    return block;
}



bool Combine_with(struct Block* block)
{
    if(block == NULL)
    {
        return false;
    }
    struct Block* next_block = block->next;
    if(next_block == NULL)
    {
        return false;
    }

    block->next=next_block->next;
    block->size+=sizeof(struct block*) + next_block->size;
    return true;
}
