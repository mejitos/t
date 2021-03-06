#include "asserts.h"


void assert_position(Position position, int line_start, int column_start, int line_end, int column_end)
{
    if (line_start != 0 && position.line_start != line_start)
        printf("\n\t\tInvalid starting line '%d', expected '%d'", position.line_start, line_start);
    if (column_start != 0 && position.column_start != column_start)
        printf("\n\t\tInvalid starting column '%d', expected '%d'", position.column_start, column_start);
    if (line_end != 0 && position.line_end != line_end)
        printf("\n\t\tInvalid ending line '%d', expected '%d'", position.line_end, line_end);
    if (column_end != 0 && position.column_end != column_end)
        printf("\n\t\tInvalid ending column '%d', expected '%d'", position.column_end, column_end);
}


void assert_token(Token* token, Token_Kind kind, const char* lexeme)
{
    if (token->kind != kind)
        printf("\n\t\tInvalid token kind '%d', expected '%d'", token->kind, kind);
    if (strcmp(token->lexeme, lexeme) != 0)
        printf("\n\t\tInvalid lexeme '%s', expected '%s'", token->lexeme, lexeme);
}
