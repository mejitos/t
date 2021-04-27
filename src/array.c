//
// TODO(timo): Filedocstring
//

#include "array.h"
#include "memory.h"     // for x-allocators
#include <stdlib.h>     // for free


array* array_init(size_t item_size)
{
    array* arr = xcalloc(1, sizeof (array));
    arr->item_size = item_size;

    return arr;
}


void array_free(array* arr)
{
    if (arr->items != NULL)
    {
        free(arr->items);
        arr->items = NULL;
    }

    free(arr);
    arr = NULL;
}


void array_push(array* arr, void* item)
{
    // If there is no items in the array, memory will be allocated for the
    // array items.
    // TODO(timo): This should be done at the array_init() so the user can
    // define the initial size of the array
    if (! arr->items)
    {
        arr->items = xcalloc(64, arr->item_size);
        arr->capacity = 64;
        arr->length = 0;
    }

    // If the array length has reached the maximum capacity, more memory will
    // be allocated for the array and the capacity will be updated accordingly.
    if (arr->capacity == arr->length)
    {
        arr->items = xrealloc(arr->items, arr->item_size * arr->capacity * 2);
        arr->capacity *= 2;

        for (int i = arr->length; i < arr->capacity; i++) 
            arr->items[i] = NULL;
    }

    // Add the new item and update the array length
    *(arr->items + arr->length) = item;
    arr->length++;
}
