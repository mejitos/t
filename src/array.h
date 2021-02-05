#ifndef t_array_h
#define t_array_h


#include <stddef.h>     // for size_t
#include "memory.h"     // for x-allocators

/*
 *  General use dynamic array
 *
 *  File: array.c
 */
typedef struct array
{
    void** items;
    int length;
    int capacity;
    size_t item_size;
} array;

array* array_init(size_t item_size);
void array_free(array* arr);
void array_push(array* arr, void* item);

#endif
