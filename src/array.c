#include "array.h"


// item size if the size of the items pointer
// since this array will store pointers of the items
array* array_init(size_t item_size)
{
    array* arr = xcalloc(1, sizeof (array));
    arr->item_size = item_size;

    return arr;
}


void array_free(array* arr)
{
    if (arr->items)
    {
        // NOTE(timo): The user of the array is responsible for freeing the 
        // elements they use. Since the rest of the elements are being initalized 
        // as NULL and not used, we don't have to explicitly free them.
        free(arr->items);
        arr->items = NULL;
    }

    free(arr);
    arr = NULL;
}


void array_push(array* arr, void* item)
{
    if (! arr->items)
    {
        arr->items = xcalloc(8, arr->item_size);
        arr->capacity = 8;
        arr->length = 0;
    }

    if (arr->capacity == arr->length)
    {
        arr->items = xrealloc(arr->items, arr->item_size * arr->capacity * 2);
        arr->capacity *= 2;
        for (int i = arr->length; i < arr->capacity; i++) arr->items[i] = NULL;
    }

    *(arr->items + arr->length) = item;
    arr->length++;
}
