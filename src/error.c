#include "t.h"


void error(Position position, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[line %d, column %d] ", position.line_start, position.column_start);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
