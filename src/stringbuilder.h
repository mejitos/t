#ifndef t_stringbuilder_h
#define t_stringbuilder_h

#include "memory.h"


typedef struct stringbuilder
{
    char* string;
    int length;
    int capacity;
} stringbuilder;


stringbuilder* sb_init();
void sb_append(stringbuilder* sb, const char* string);
void sb_free(stringbuilder* sb);

#endif
