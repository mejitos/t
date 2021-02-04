#ifndef t_memory_h
#define t_memory_h


#include <stdlib.h>     // for allocs, exit
#include <stdio.h>      // for printing


/*
 *  Memory allocators à la K & R. Therefore code is borrowed from the book The C Programming
 *  Language by Brian Kernighan and Dennis Ritchie. General opinion though is that this kind
 *  of allocation of memory is pretty bad practice in prodcution, but luckily we ain't in 
 *  production...at least yet.
 *
 *  The memory is being alllocated like it normally would with these C functions, but the
 *  success of this allocation will be checked and if it failed -> program will exit.
 *
 *  File: memory.c
 */
void* xmalloc(size_t bytes);
void* xcalloc(size_t length, size_t size);
void* xrealloc(void* pointer, size_t bytes);


#endif
