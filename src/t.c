#include "t.h"


void compile(const char* source)
{
    // Lexing part
    Lexer* lexer = lexer_init(source);
    lex(lexer);

    assert(lexer->tokens_length == 1);

    // Print the tokens
}
