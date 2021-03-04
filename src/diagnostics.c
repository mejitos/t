#include "t.h"


Diagnostic* diagnostic(Diagnostic_Kind kind, Position position, const char* message, ...)
{
    char* buffer;
    size_t buffer_length;
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


void print_diagnostic(Diagnostic* diagnostic)
{
    printf("file.t:%d:%d%s", diagnostic->position.line_start, diagnostic->position.column_start, diagnostic->message);
}


void print_diagnostics(array* diagnostic)
{
}
