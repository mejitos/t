#include "tests.h"
#include "../src/t.h"
#include "../src/stringbuilder.h"


static bool not_error = true;


void test_parser()
{
    printf("Running parser tests...");

    Lexer lexer;
    Parser parser;
    stringbuilder* sb = sb_init();

    lexer_init(&lexer, "return 0;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    AST_Statement* s = parse_statement(&parser);

    assert(s->kind == STATEMENT_RETURN);
    assert(s->_return.value->literal->integer_value == 0);
        
    if (not_error) printf("OK\n");
    else printf("\n");
}
