#include "array.h"


array* array_init(size_t item_size)
{
    array* arr = xcalloc(1, sizeof (array));
    arr->item_size = item_size;

    return arr;
}


void array_init2(array* arr, size_t item_size)
{
    *arr = xcalloc(1, sizeof (array));
    arr->item_size = item_size;
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
