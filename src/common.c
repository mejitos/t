#include "common.h"
#include <stdlib.h>
#include <stdio.h>


char* read_file(const char* path)
{
    FILE* file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Could not open file '%s'\n", path);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char* buffer = malloc(file_size * sizeof (char) + 1);
    
    if (buffer == NULL)
    {
        printf("Malloc failed. Not enough memory to read file '%s'\n", path);
        exit(1);
    }

    size_t bytes_read = fread(buffer, sizeof (char), file_size, file);

    if (bytes_read < file_size)
    {
        printf("Could not read file '%s'\n", path);
        exit(1);
    }

    buffer[bytes_read] = 0;
    fclose(file);
    
    return buffer;
}
