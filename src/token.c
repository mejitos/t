#include "t.h"


Token* token_base(Token_Kind kind)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = kind;

    return token;
}


Token* token_integer(int value)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = TOKEN_INTEGER_LITERAL;
    token->integer_value = value;

    return token;
}


Token* token_boolean(bool value)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = TOKEN_BOOLEAN_LITERAL;
    token->boolean_value = value;

    return token;
}


Token* token_identifier(Token_Kind kind, const char* identifier)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = kind;
    token->identifier = identifier;

    return token;
}
