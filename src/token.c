#include "t.h"


Token* token_base(Token_Kind kind)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = kind;

    return token;
}
