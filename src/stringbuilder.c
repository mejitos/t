#include "stringbuilder.h"
#include <string.h>     // for memmove


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

    if (sb->capacity < sb->length + length + 1)
    {
        sb->capacity *= 2;
        sb->string = xrealloc(sb->string, sb->capacity);
    }

    memmove(sb->string + sb->length, string, length);
    sb->length += length;
}


char* sb_to_string(stringbuilder* sb)
{
    return sb->string;
}
