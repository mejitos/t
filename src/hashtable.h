// Implementation for simple hashtable with key-value pairs. All keys are
// strings and values can be arbitrary pointers.
//
// Author: Timo Mehto
// Date: 2021/05/12

#ifndef T_HASHTABLE_H 
#define T_HASHTABLE_H 

#include "memory.h"     // for x-allocators
#include <string.h>     // for memory comparisons and strlen
#include <stdlib.h>     // for memory allocation
#include <assert.h>     // for assertions
#include <stdint.h>     // for int32_t
#include <stdbool.h>    // for bool type

#define LOAD_FACTOR 0.75
#define GROWTH_FACTOR 2
#define INITIAL_CAPACITY 128


// Structure for hashtable entry which is represents key-value pairs saved
// into the hash table.
//
// Fields
//      key: Pointer to the key for the value.
//      value: Pointer to the saved value.
typedef struct hashtable_entry {
    char* key;
    void* value;
} hashtable_entry;


// Hash table/map to save key-value pairs. 
//
// Keys are strings but the values can be any type of pointer.
//
// Fields
//      capacity: Maximum capacity of the hash table.
//      count: Current number of the symbols in the hash table.
//      threshold: Count after which the table will be resized.
//      entries: Pointer to the start of the symbol table entries.
typedef struct hashtable
{
    int capacity;
    int count;
    int threshold;
    
    hashtable_entry* entries;
} hashtable;


// Initializes new hashtable with initial capacity.
//
// If the passed initial capacity is less than INITIAL_CAPACITY, the initial
// capacity will be INITIAL_CAPACITY.
//
// Arguments
//      capacity: The initial capacity.
// Returns
//      Pointer to the new hash table
hashtable* hashtable_init(int capacity);


// Frees all the memory allocated for a hash table.
//
// Arguments
//      table: Hash table to be freed.
void hashtable_free(hashtable* table);


// Get the value accompanied with the key in the hash table.
//
// Arguments
//      table: Hash table where the value is being queried from.
//      key: Key accompanied with the value.
void* hashtable_get(const hashtable* table, const char* key);


// Creates a new entry and puts it to the hash table.
//
// Arguments
//      table: Hash table where the new entry is being created.
//      key: Key accompanied with the value.
//      value: Pointer to the value accomapnied with the key.
void hashtable_put(hashtable* table, const char* key, void* value);


// Checks if the given key is found from the hash table.
//
// Arguments
//      table: Hash table where the key is being searched from.
//      key: Key being searched from the hash table.
// Returns
//      Value true if the key is found from the hash table, otherwise false.
const bool hashtable_contains(const hashtable* table, const char* key);


#ifdef T_HASHTABLE_IMPLEMENTATION


hashtable* hashtable_init(int capacity)
{
    hashtable* table = xmalloc(sizeof (hashtable));

    if (capacity < INITIAL_CAPACITY)
        capacity = INITIAL_CAPACITY;
    
    table->capacity = capacity;
    table->threshold = capacity * LOAD_FACTOR;
    table->count = 0;

    table->entries = xmalloc(capacity * sizeof (hashtable_entry));
    memset(table->entries, 0, capacity * sizeof (hashtable_entry));

    return table;
}


void hashtable_free(hashtable* table)
{
    for (int i = 0; i < table->capacity; i++)
    {
        hashtable_entry entry = table->entries[i];

        if (entry.key) 
        {
            free(entry.key);
            entry.key = NULL;
        }
    }

    free(table->entries);
    table->entries = NULL;

    free(table);
    table = NULL;
}


// Fowler-Noll-Vo-1a hash function 
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function                                        
static uint32_t fnv1a_hash(const char* key)
{
    uint32_t hash = 0x811c9dc5;

    for (int i = 0; key[i] != 0; i++)
    {
        hash ^= (uint8_t)key[i];
        hash *= 0x01000193;
    }

    return hash;
}


static void hashtable_resize(hashtable* table)
{
    int old_capacity = table->capacity;
    hashtable_entry* old_entries = table->entries;
    
    table->capacity *= GROWTH_FACTOR;
    table->threshold = table->capacity * LOAD_FACTOR;
    table->count = 0;
    table->entries = xmalloc(table->capacity * sizeof (hashtable_entry));
    memset(table->entries, 0, table->capacity * sizeof (hashtable_entry));
    
    for (int i = 0; i < old_capacity; i++)
    {
        hashtable_entry* entry = &old_entries[i];

        if (entry->key)
        {
            hashtable_put(table, entry->key, entry->value);
            free(entry->key);
        }
    }

    free(old_entries);
}


void* hashtable_get(const hashtable* table, const char* key)            
{
    size_t key_length = strlen(key);
    uint32_t hash = fnv1a_hash(key);

    for (int i = 0; i < table->capacity; i++)
    {
        hashtable_entry* entry = &table->entries[(hash + i) % table->capacity];
        
        if (entry->key && memcmp(entry->key, key, key_length) == 0)
            return entry->value;
        if (entry->key == NULL) 
            return NULL;
    }

    return NULL;
}


void hashtable_put(hashtable* table, const char* key, void* value)
{
    if (table->count + 1 >= table->threshold) 
        hashtable_resize(table);

    size_t key_length = strlen(key);
    uint32_t hash = fnv1a_hash(key);

    for (int i = 0; i < table->capacity; i++)
    {
        hashtable_entry* entry = &table->entries[(hash + i) % table->capacity];
    
        if (entry->key && memcmp(entry->key, key, key_length) == 0)
        {
            entry->value = value;
            return;
        }
        if (entry->key == NULL)
        {
            entry->key = xmalloc(key_length * sizeof (char) + 1);
            memcpy(entry->key, key, key_length); 
            entry->key[key_length] = 0;
            entry->value = value;

            table->count++;

            return;
        }
    }
}


const bool hashtable_contains(const hashtable* table, const char* key)
{
    size_t key_length = strlen(key);
    uint32_t hash = fnv1a_hash(key);

    for (int i = 0; i < table->capacity; i++)
    {
        hashtable_entry* entry = &table->entries[(hash + i) % table->capacity];
        
        if (entry->key == NULL) 
            return false;
        if (entry->key && memcmp(entry->key, key, key_length) == 0)
            return true;
    }

    return false;
}


#endif // T HASHTABLE IMPLEMENTATION

#endif // T HASHTABLE H
