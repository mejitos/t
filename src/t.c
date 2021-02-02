#include "t.h"


void compile(const char* source)
{
    // Lexing part
    Lexer lexer;

    lexer_init(&lexer, source);
    lex(&lexer);

    // Print the tokens
    
    // Free the lexers allocated data
    lexer_free(&lexer);
}
