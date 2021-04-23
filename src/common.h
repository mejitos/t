#ifndef t_common_h
#define t_common_h

#include <stdbool.h>


char* read_file(const char* path);
char* shift(int* argc, char*** argv);
bool str_equals(char* string1, char* string2);
bool str_starts_with(char* string, char* start);
char* str_copy(char* what);


#endif
