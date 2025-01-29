#include "customAllocator.h"

static BlockList blockList = {NULL, NULL};

void* customMalloc(size_t size)
{
    if(size <= 0)
    {
        perror("<malloc error>: passed nonpositive size");
    }
    size_t real_size = ALIGN_TO_MULT_OF_4(size);
    struct Block* new_block = Find_And_Allocate(real_size);
    if(new_block == NULL)
    {
        new_block = Allocate_Extenend_Heap(real_size);
    }
    printf("memory %d \n",(int)real_size);
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
    }
    
    block->free = true;
    block = Merge_Block(block);
    if(block->next == NULL)
    {
        if( block->prev != NULL )
        {
            block->prev->next = NULL;
        }

        blockList.head = block->prev;
        if(blockList.tail == block)
        {
            blockList.tail = NULL;
        }
        sbrk(-(sizeof(struct Block) + block->size));
    }

 }



void* customCalloc(size_t nmeb, size_t size)
{

    if( size<=0 || nmeb <=0 )
    {
        //error
    }
    size_t total_size = nmeb*size;
    void *ptr = customMalloc(total_size);
    if(ptr == NULL)
    {
        //error
    }
    struct Block* block=Find_Block_ptr(ptr);
    memset(block->user_data,0,nmeb*size);
    return block->user_data;
}




void* customRealloc(void* ptr, size_t size)
{

    if(ptr == NULL)
    {
        return customMalloc(size);
    }
    size_t blocksize = sizeof(struct Block);
    struct Block* block = Find_Block_ptr(ptr);

    if(block == NULL)
    {
        return NULL; //block doesnt exist
    }

    size_t old_size = block->size;
    block = Merge_Block(block);
    size_t new_size = block->size;

    
    if(block->size == size) //incase the size are equal
    {
        memcpy(block->user_data,ptr,old_size);
        block->free = false;
        return (void*)(block->user_data);
    }
    if(block->user_data != ptr)
        memcpy(block->user_data,ptr,old_size);

    if(block-> size > size)   //incase old size > size
    {
        block->size = size;
        block->free = false;
        if(block == blockList.head)  //if the block is the final one (the head) then only decrease the progtam break
        {  
            sbrk( size - block->size );
            return (void*)(block->user_data);
        }else{

            if(block->size > size + blocksize + 4)  //if possible split to two blocks
            {
                struct Block* new_block = (struct Block*)(block->user_data + size);
                new_block->free = true;
                new_block->size = new_size - size - blocksize;
                new_block->next = block->next;
                new_block->prev = block;
                block->next = new_block;
            }else{
                block->size = new_size;
            }
            return (void*)(block->user_data);
        }
    }else{

        if(block == blockList.head) //if it is at the head just extend the program break
        {
            sbrk( size - block->size);
            block->free = false;
            return (void*)(block->user_data);
        }else{
            //move to new block
            struct Block* new_block = (struct Block*)(customMalloc(size));
            memcpy(new_block->user_data,block->user_data,old_size);
            new_block->size = size;
            new_block->free = false;
            new_block->prev = blockList.head;
            blockList.head->next = new_block; 
            new_block->next = NULL;
            blockList.head = new_block;
            block->free = true;
            return (void*)(new_block->user_data);

            }
        
    }
    /*

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

    if(block->prev != NULL)
    {
        if( block->prev->free == true)
            current_size += block->prev->size+ block_size;
    }
    if(block->next != NULL)
    {
        if( block->next->free == true)
            current_size += block->next->size + block_size;
    }

    if(current_size >= size + block_size + 4)
    {
        block = Merge_Block(block);
       if((void*)(block->user_data) != ptr)
        {   
            memcpy(block,ptr,old_size);
        }
        struct Block* new_block  = (struct Block*)(block->user_data + size);
        new_block -> next = block->next;
        new_block -> prev = block;
        new_block -> free = true;
        new_block -> size = current_size - size - sizeof(struct Block);
        block -> next = new_block;
        block -> size = size;
        block -> free = false;
        Merge_Block(new_block);
        return (void*)(block->user_data);
    }else{
        if(block->next == NULL)
        { 
            block = Merge_Block(block);
            sbrk(size + block_size + 4 - current_size);
            block->size+= size + block_size + 4 - current_size;
            memcpy(block,ptr,size);
            return ptr;
        }else {
        struct Block* new_block = (struct Block*)(customMalloc(size));
        memcpy(new_block->user_data,block->user_data,block->size);
        block->free = true;
        Merge_Block(block);
        return ptr;
        }
        
    }
*/
}











struct Block* Find_And_Allocate(size_t size)
{
//find best fit
    struct Block* block = Find_BestFit(size,blockList.head);
//resize the best fit to prevent wasting space
    if(block == NULL)
    {
        return NULL;
    }
    if( block-> size - size >= sizeof(struct Block)  + 4)
    {
        struct Block* new_block  = (struct Block*)(block->user_data + size);
        new_block -> next = block->next;
        new_block -> prev = block;
        new_block -> free = true;
        new_block -> size = block->size - size - sizeof(struct Block);

        block -> size = size;
        block -> next = new_block;
    }
    block->free = false;
    return block;
}





struct Block* Find_Block_ptr(void* ptr)
{
    struct Block* block = blockList.tail;
    struct Block* head = blockList.head;
    while( 1 )
    {
        if( (void*)(block->user_data) == ptr ) 
        {
            return block;
        }

        if( block == head )
        {
            return NULL;
        }

        block = block->next;
    }

}


struct Block* Merge_Block(struct Block* block)
{
    if(block->prev != NULL)
    {
         struct Block* prev_block = block->prev;
    
        if(prev_block->free)
        {
            if( Combine_with(prev_block))
                block = prev_block;
        }

    }

    if(block->next != NULL)
    {
        struct Block* next_block = block->next;

        if(next_block->free)
        {
            Combine_with(block);
        }
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
    block->size+=sizeof(struct Block) + next_block->size;
    return true;
}




struct Block* Allocate_Extenend_Heap(size_t size)
{
       
    void* ptr = sbrk(sizeof(Block)+size);
    if(ptr == (void*)-1)    
    {   
        perror("sbrk failed");
        return NULL;
    }
    struct Block* block = (struct Block*)ptr;
    block->size = size;
    block->free = false;

    block->next = NULL;
    block->prev = blockList.head;
    if(blockList.head != NULL)
    {
        blockList.head -> next = block;
    }
    blockList.head = block;

    
    if(blockList.tail == NULL)
    {
        blockList.tail = block;
    }
    return block;
}




struct Block* Find_BestFit(size_t size, struct Block* head)
{
    struct Block* block = blockList.tail;
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
        if( block == head )
        {
            break;
        }
        block = block->next;
        
    }

    if( size == 0 )
    {
        return NULL;
    }

    return min_block;
}