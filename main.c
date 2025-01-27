#include "customAllocator.h"




int main(int argc, char* argv[])
{
    
    void* x = customMalloc(8);
    printf("passed allocation");
    *(int*)x=12000;
    printf("%d",(*(int*)x));
    return 0;
}

