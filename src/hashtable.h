#ifndef T_HASHTABLE_H 
#define T_HASHTABLE_H 

#include <string.h> // for memory comparisons and strlen
#include <stdlib.h> // for memory allocation
#include <assert.h> // for assertions
#include <stdint.h> // for int32_t
#include <stdbool.h>

#define LOAD_FACTOR 0.75
#define GROWTH_FACTOR 2
#define INITIAL_CAPACITY 1024


typedef struct hashtable_entry {
    char* key;
    void* value;
} hashtable_entry;


typedef struct hashtable
{
    int capacity;
    int count;
    int threshold;
    
    hashtable_entry* entries;
} hashtable;


hashtable* hashtable_init(int capacity);
void hashtable_free(hashtable* table);
void* hashtable_get(hashtable* table, const char* key);
void hashtable_put(hashtable* table, const char* key, void* value);
bool hashtable_contains(hashtable* table, const char* key);


#ifdef T_HASHTABLE_IMPLEMENTATION


hashtable* hashtable_init(int capacity)
{
    // TODO(timo): Check for alloc failure or use xmalloc
    hashtable* table = malloc(sizeof (hashtable));

    if (capacity < INITIAL_CAPACITY)
        capacity = INITIAL_CAPACITY;
    
    table->capacity = capacity;
    table->threshold = capacity * LOAD_FACTOR;
    table->count = 0;

    // TODO(timo): Check for alloc failure or use xmalloc
    table->entries = malloc(capacity * sizeof (hashtable_entry));
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
    table->entries = malloc(table->capacity * sizeof (hashtable_entry));
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


void* hashtable_get(hashtable* table, const char* key)            
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
            // TODO(timo): Check for failed allocation
            entry->key = malloc(key_length * sizeof (char) + 1);
            memcpy(entry->key, key, key_length); 
            entry->key[key_length] = 0;
            entry->value = value;

            table->count++;

            return;
        }
    }
}


bool hashtable_contains(hashtable* table, const char* key)
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
