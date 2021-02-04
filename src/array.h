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
void array_init2(array* arr, size_t item_size);
void array_push(array* arr, void* item);
