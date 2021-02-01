#include "t.h"


void compile(const char* source)
{
    // Lexing part
    Lexer lexer = { .stream = source };
    // lexer_init(lexer, source);
    lex(&lexer);

    assert(lexer.tokens_length == 1);

    // Print the tokens
    
    // Free the lexer data
    for (Token** it = lexer.tokens; it != lexer.tokens + lexer.tokens_length; it++)
    {
        free(*it);
    }

    free(lexer.tokens);
}
