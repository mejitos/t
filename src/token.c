#include "t.h"


// NOTE(timo): This memory allocation for each lexeme is pretty
// wasteful of resources and speed, but for now it makes things
// easier to understand and do in the later stages so it stays
// like this at least for now.
// TODO(timo): Therefore later we should just use pointer
// arithmetics with the pointer to the starting character
Token* token(Token_Kind kind, const char* lexeme, int lexeme_length, Position position)
{
    Token* token = xcalloc(1, sizeof (Token));
    token->kind = kind;
    token->lexeme = xcalloc(lexeme_length + 1, sizeof (char));
    memcpy(token->lexeme, lexeme, lexeme_length);
    token->lexeme_length = lexeme_length;
    token->position = position;

    return token;
}
