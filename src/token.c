#include "t.h"


// TODO(timo): Do I actually have to allocate these, since these are added
// to the lexer, which lives in the top context? Are they being added to the
// stack if I don't explicitly allocate them since allocating will allocate 
// the memory into the heap?


Token* token_base(Token_Kind kind, Position position)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = kind;
    token->position = position;

    return token;
}


Token* token_integer(int value, Position position)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = TOKEN_INTEGER_LITERAL;
    token->integer_value = value;
    token->position = position;

    return token;
}


Token* token_boolean(bool value, Position position)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = TOKEN_BOOLEAN_LITERAL;
    token->boolean_value = value;
    token->position = position;

    return token;
}


Token* token_identifier(const char* identifier, Position position)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = TOKEN_IDENTIFIER;
    token->identifier = identifier;
    token->position = position;

    return token;
}
