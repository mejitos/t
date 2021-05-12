// Definitions for dynamic array.
//
// TODO(timo): Create a single header file library of this
// TODO(timo): More functions for the interface:
//      - pop
//      - get (get_item)
//      - set (set_item)
//
// File(s): array.c
//
// Author: Timo Mehto
// Date: 2021/05/12

#ifndef t_array_h
#define t_array_h

#include <stddef.h>     // for size_t, NULL


// General use dynamic array
//
// Fields
//      items: Pointer to the start of the items in the array.
//      length: Length of the array.
//      capacity: Maximum capacity of the array.
//      item_size: Size of the item being saved in the array.
typedef struct array
{
    void** items;
    int length;
    int capacity;
    size_t item_size;
} array;


// Initializes new array.
//
// Arguments
//      item_size: Size of the pointer to the item which are being saved
//                 to the array.
// Returns
//      Pointer to the newly created array.
array* array_init(size_t item_size);


// Frees the memory allocated for an array structure.
//
// The user of the array is responsible for freeing the elements they use. 
// Since the rest of the elements are being initalized as NULL and not used, 
// we don't have to explicitly free them, so only the pointers themselves
// will be freed.
//
// Arguments
//      arr: Array structure to be freed.
void array_free(array* arr);


// Appends new item to the end of the array.
//
// Allocating and reallocting of the memory for the array will be handled 
// in here.
//
// Arguments
//      arr: Array which the item is being appended to.
//      item: Pointer to the item being appended to the end of the array.
void array_push(array* arr, void* item);


#endif
