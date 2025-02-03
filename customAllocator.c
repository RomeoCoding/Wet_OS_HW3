#include "customAllocator.h"

static BlockList blockList = {NULL, NULL};

void* customMalloc(size_t size)
{
    if(size <= 0)
    {
        perror("<malloc error>: passed nonpositive size");
        return NULL;
    }
    size_t real_size = ALIGN_TO_MULT_OF_4(size);
    struct Block* new_block = Find_And_Allocate(real_size);
    if(new_block == NULL)
    {
        new_block = Allocate_Extenend_Heap(real_size);
    }
   // printf("memory %d \n",(int)real_size);
       return new_block ? (void*)new_block->user_data : NULL;
}



void customFree(void* ptr)
{
    if(ptr == NULL)
    {
        perror("<free error>: passed null pointer");
        return;
    }
    struct Block* block = Find_Block_ptr(ptr);
    if(block == NULL)
    {
        perror("<free error>: cant find the pointer");
        return;
    }

    if(block->free == true)
    {
        return;
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
        size_t release_size = sizeof(struct Block) + block->size;
        void* temp = sbrk(-release_size);
          if(temp  == (void*)-1)
            {
                if (errno == ENOMEM)
                {
                    perror("<sbrk/brk error>: out of memory");
                    Free_All_Memory();
                    exit(1);
                }
            }
    }

 }



void* customCalloc(size_t nmeb, size_t size)
{

    if( size<=0 || nmeb <=0 )
    {
        perror("<calloc error>: passed nonpositive value");
    }
    size_t total_size = nmeb*size;
    void *ptr = customMalloc(total_size);
    if(ptr == NULL)
    {
        return NULL;
    }
    struct Block* block=Find_Block_ptr(ptr);
    memset(block->user_data,0,total_size);
    return block->user_data;
}




void* customRealloc(void* ptr, size_t size)
{

    if(ptr == NULL)
    {
        return customMalloc(size);
    }
    if(size == 0)
    {
        customFree(ptr);
        return NULL;
    }
    size_t blocksize = sizeof(struct Block);
    struct Block* block = Find_Block_ptr(ptr);

    if(block == NULL)
    {
        return NULL; //block doesnt exist
    }

    size = ALIGN_TO_MULT_OF_4(size);
    size_t old_size = block->size;
    //block = Merge_Block(block);
    //size_t new_size = block->size;
    if(size == old_size)
    {
        return ptr;
    }
    
    block = Merge_Block(block);
    size_t new_size  = block->size;
    block->free = false;
   
    if(block->size == size) //incase the size are equal
    {
        memmove(block->user_data,ptr,old_size);
        block->free = false;
        return (void*)(block->user_data);
    }
    if(block->user_data != ptr)
        memmove(block->user_data,ptr,old_size);

    if(block->size > size)   //incase old size > size
    {
        block->size = size;
        block->free = false;
        if(block == blockList.head)  //if the block is the final one (the head) then only decrease the progtam break
        {  
            void* temp = sbrk( size - block->size );
            if(temp  == (void*)-1)
            {
                if (errno == ENOMEM)
                {
                    perror("<sbrk/brk error>: out of memory");
                    Free_All_Memory();
                    exit(1);
                }
            }
            return (void*)(block->user_data);
        }else{

            if(block->size > size + blocksize + 4)  //if possible split to two blocks
            {
                struct Block* new_block = (struct Block*)(block->user_data + size);
                new_block->free = true;
                new_block->size = new_size - size - blocksize;
                new_block->next = block->next;
                if(block->next)
                {
                    block->next->prev = new_block;
                }
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
            void* temp = sbrk( size - block->size);
            if(temp  == (void*)-1)
            {
                if (errno == ENOMEM)
                {
                    perror("<sbrk/brk error>: out of memory");
                    Free_All_Memory();
                    exit(1);
                }
            }
           
           // total_sbrk += size-block->size;
            block->free = false;
            block->size = size;
            return (void*)(block->user_data);
        }else{
            //move to new block
            void* new_block_ptr=customMalloc(size);
            Block* new_block = Find_Block_ptr(new_block_ptr);
            memmove(new_block->user_data,block->user_data,old_size);
            block->free = true;
            Merge_Block(block);
            return (void*)(new_block->user_data);

            }
        
    }


   
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
    if( blockList.head == NULL ||  blockList.tail == NULL)
    {
        return NULL;
    }
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
    if(next_block->next != NULL)
    {
        next_block->next->prev = block;
    }
    block->size+=sizeof(struct Block) + next_block->size;
    return true;
}




struct Block* Allocate_Extenend_Heap(size_t size)
{
       
    void* ptr = sbrk(sizeof(Block)+size);
    if(ptr == (void*)-1)    
    {   
        if (errno == ENOMEM) {
            perror("<sbrk/brk error>: out of memory");
            Free_All_Memory();
            exit(1);
        }else{
            return NULL;
        }
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
        blockList.tail -> prev = NULL;
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






void Free_All_Memory()
{
    Block* block = blockList.head;
    size_t size;
    while(block->prev != NULL)
    {
        size=block->size;
        block = block->prev;
        sbrk(-sizeof(Block) + size);
       
    }
   
    sbrk(-sizeof(Block) + block->size);
}