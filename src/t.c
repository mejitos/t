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


    // Teardown
    parser_free(&parser);
    lexer_free(&lexer);
}
