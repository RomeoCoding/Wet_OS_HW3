#include "customAllocator.h"




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
    customFree(Q);
    customFree(z);
    return 0;
}

