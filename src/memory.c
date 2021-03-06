// Implementations for the x-allocators.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "memory.h"
#include <stdlib.h>     // for allocs, exit
#include <stdio.h>      // for printing


void* xmalloc(size_t bytes)
{
    void* pointer = malloc(bytes);

    if (pointer == NULL)
    {
        printf("Error: xmalloc failed.\n");
        exit(1);
    }

    return pointer;
}


void* xrealloc(void* pointer, size_t bytes)
{
    pointer = realloc(pointer, bytes);

    if (pointer == NULL)
    {
        printf("Error: xrealloc failed.\n");
        exit(1);
    }

    return pointer;
}


void* xcalloc(size_t length, size_t size)
{
    void* pointer = calloc(length, size);

    if (pointer == NULL)
    {
        printf("Error: xcalloc failed.\n");
        exit(1);
    }

    return pointer;
}
