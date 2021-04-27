//
// TODO(timo): File docstring
//

#ifndef t_common_h
#define t_common_h

#include <stdbool.h>        // for bool type


// Reads file from given path and returns the contents of the file.
//
// If error is detected anywhere in the process of reading the file, error will
// be printed and the program will exit right away.
//
// Arguments
//      path: Path pointing to the file wanted to be read.
// Returns
//      Pointer to the start of the buffer containing the contents of the file.
const char* read_file(const char* path);


// Shifts the pointer of the argument vector to the right by one and decreases
// the argument count by one.
//
// Arguments
//      argc: Pointer to argument count.
//      argv: Pointer to argument vector.
// Returns
//      Pointer to the start of the current argument in the argument vector
//      before shifting everything to right.
const char* shift(int* argc, char*** argv);


// Compares two strings together and checks if they are equal.
//
// Arguments
//      string1: First string to be compared.
//      string2: Second string to be compared.
// Returns
//      Value true if the strings are equal, otherwise false.
const bool str_equals(const char* string1, const char* string2);


// Checks if start of the string matches with other string.
//  
// Arguments
//      string: String to be inspected
//      start: String which is being compared agains the 
// Returns
//      Value true if the start of the string matches with the second string,
//      otherwise false.
const bool str_starts_with(const char* string, const char* start);


// Creates copy of the string passed as argument.
//
// Copy is created by allocating new memory of size of the passed string and 
// copying the string to the newly allocated memory.
//
// Arguments
//      what: String to be copied.
// Returns
//      Pointer to the start of the copied string.
char* str_copy(const char* what);


#endif
