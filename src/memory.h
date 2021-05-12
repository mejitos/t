//  Memory allocators à la K & R. Therefore code is borrowed from the book 
//  The C Programming Language by Brian Kernighan and Dennis Ritchie. General 
//  opinion though is that this kind of allocation of memory is pretty bad 
//  practice in prodcution.
//
//  The memory is being alllocated like it normally would with these C functions, 
//  but the success of this allocation will be checked and if it fails, the 
//  program will exit.
//
//  This is bad practice because in case of error, there is no actual error
//  recovery and everything just dies. This doesn't really matter in our use
//  case (=compiler) because the program is so short lived and all the allocated
//  memory will be freed back to the operating system anyway.
//
//  Author: Timo Mehto
//  Date: 2021/05/12

#ifndef t_memory_h
#define t_memory_h

#include <stddef.h>     // for size_t


void* xmalloc(size_t bytes);
void* xcalloc(size_t length, size_t size);
void* xrealloc(void* pointer, size_t bytes);


#endif
