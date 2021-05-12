// Implementations for creating Diagnostic items and printing them.
// 
// TODO(timo): Add the name of the file to be part of the message
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


Diagnostic* diagnostic(const Diagnostic_Kind kind, const Position position, const char* message, ...)
{
    char* buffer;
    size_t buffer_length;
    // TODO(timo): apparently open_memstream is not part of C99
    FILE* out = open_memstream(&buffer, &buffer_length);

    va_list args;
    va_start(args, message);

    vfprintf(out, message, args);
    va_end(args);
    fclose(out);

    Diagnostic* diagnostic = xmalloc(sizeof (Diagnostic));
    diagnostic->kind = kind;
    diagnostic->position = position;
    diagnostic->message = buffer;

    return diagnostic;
}


void print_diagnostic(const Diagnostic* diagnostic)
{
    printf(":%d:%d%s\n", diagnostic->position.line_start, diagnostic->position.column_start, diagnostic->message);
}


void print_diagnostics(const array* diagnostics)
{
    for (int i = 0; i < diagnostics->length; i++)
        print_diagnostic(diagnostics->items[i]);
}
