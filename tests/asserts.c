#include "asserts.h"


void assert_base(Test_Runner* runner, bool assertion, const char* message, ...)
{
    if (! assertion)
    {
        char* buffer;
        size_t buffer_length;
        FILE* out = open_memstream(&buffer, &buffer_length);

        va_list args;
        va_start(args, message);

        vfprintf(out, message, args);
        va_end(args);
        fclose(out);

        array_push(runner->errors, test_error(runner->current_set->description,
                                              runner->current_case->description,
                                              buffer));
        runner->error = true;
    }
}


void assert_position(Test_Runner* runner, Position position, int line_start, int column_start, int line_end, int column_end)
{
    assert_base(runner, (line_start != 0 && position.line_start == line_start),
        "Invalid starting line '%d', expected '%d'", position.line_start, line_start);
    assert_base(runner, (column_start != 0 && position.column_start == column_start),
        "Invalid starting column '%d', expected '%d'", position.column_start, column_start);
    assert_base(runner, (line_end != 0 && position.line_end == line_end), 
        "Invalid ending line '%d', expected '%d'", position.line_end, line_end);
    assert_base(runner, (column_end != 0 && position.column_end == column_end), 
        "Invalid ending column '%d', expected '%d'", position.column_end, column_end);
}


void assert_token(Test_Runner* runner, Token* token, Token_Kind kind, const char* lexeme)
{
    assert_base(runner, token->kind == kind,
        "Invalid token kind '%d', expected '%d'", token->kind, kind);
    assert_base(runner, strcmp(token->lexeme, lexeme) == 0,
        "Invalid lexeme '%s', expected '%s'", token->lexeme, lexeme);
}
