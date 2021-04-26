//
// TODO(timo): File docstring
//

#include "common.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char* read_file(const char* path)
{
    // Try to open the file and read it's contents. If no file could not be
    // opened, the `file` will be NULL, error will be printed and the program 
    // will be exited.
    FILE* file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Could not open file '%s'\n", path);
        exit(1);
    }

    // Find the end of the file and save the size of the file based on that.
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);

    // Rewind the file back to beginning and allocate memory for the buffer
    // to save the contents of the file. If the allocation fails, error will
    // be printed and the program will be exited.
    rewind(file);
    char* buffer = malloc(file_size * sizeof (char) + 1);
    
    if (buffer == NULL)
    {
        printf("Malloc failed. Not enough memory to read file '%s'\n", path);
        exit(1);
    }

    // Try to read the contents of the file and save them to the allocated
    // buffer. If the reading fails, error will be printed and the program
    // will be exited.
    size_t bytes_read = fread(buffer, sizeof (char), file_size, file);

    if (bytes_read < file_size)
    {
        printf("Could not read file '%s'\n", path);
        exit(1);
    }

    // Set the last character to 0 and close the file handle before returning
    // the buffer.
    buffer[bytes_read] = 0;
    fclose(file);
    
    return buffer;
}


const char* shift(int* argc, char*** argv)
{
    const char* arg = **argv;
    // TODO(timo): For some reason ++ and -- does not work here, find out why.
    *argv += 1;
    *argc -= 1;

    return arg;
}


const bool str_equals(const char* string1, const char* string2)
{
    return strcmp(string1, string2) == 0;
}


const bool str_starts_with(const char* string, const char* start)
{
    if (string == NULL || start == NULL) return false;

    for (int i = 0; i < strlen(start); i++)
        if (start[i] != string[i]) return false;
    
    return true;
}


char* str_copy(const char* what)
{
    size_t length = strlen(what);
    char* buffer = xmalloc(sizeof (char) * (length + 1));
    memcpy(buffer, what, length);
    buffer[length] = 0;
    
    return buffer;
}
