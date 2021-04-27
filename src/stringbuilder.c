//
// TODO(timo): Filedocstring
//

#include "stringbuilder.h"
#include "memory.h"     // for x-allocators
#include <string.h>     // for memmove, strlen
#include <stdlib.h>     // for free


stringbuilder* sb_init()
{
    stringbuilder* sb = xcalloc(1, sizeof (stringbuilder));
    sb->string = xcalloc(32, sizeof (char));
    sb->capacity = 32;

    return sb;
}


void sb_free(stringbuilder* sb)
{
    free(sb->string);
    free(sb);
}


void sb_append(stringbuilder* sb, const char* string)
{
    int length = strlen(string);

    // If the length of the new string would exceed the maximum length, new
    // memory will be allocated for the new bigger string.
    if (sb->capacity < sb->length + length + 1)
    {
        sb->capacity *= 2;
        sb->string = xrealloc(sb->string, sb->capacity);
    }

    // Add the new piece of string to the end of the current string.
    // memmove is considered more safe than memcpy, so it is used. Safety
    // comes from taking into account overlapping memory.
    memmove(sb->string + sb->length, string, length);
    sb->length += length;
}


char* sb_to_string(stringbuilder* sb)
{
    return sb->string;
}
