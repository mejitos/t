#include "t.h"


void compile(const char* source)
{
    // Lexing part
    Lexer lexer;

    lexer_init(&lexer, source);
    lex(&lexer);

    // Parsing part
    Parser parser;

    parser_init(&parser, lexer.tokens);
    parse(&parser);


    // Free the lexers allocated data
    // lexer_free(&lexer);
}
