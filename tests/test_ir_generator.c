#include "../src/t.h"
#include "tests.h"


static bool not_error = true;

void test_emit_literal_expression()
{
    printf("\tLiteral expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };

    // integer literal
    lexer_init(&lexer, "42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    AST_Expression* expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);
    
    // TODO(timo): boolean literals

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_emit_binary_expression()
{
    printf("\tBinary expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;

    // binary arithmetic +
    lexer_init(&lexer, "1 + 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // binary arithmetic -
    lexer_init(&lexer, "3 - 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // binary arithmetic *
    lexer_init(&lexer, "5 * 6");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // binary arithmetic /
    lexer_init(&lexer, "7 / 8");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_emit_arithmetics()
{
    printf("\tArithmetics...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;

    lexer_init(&lexer, "1 + 2 * 3");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // ----
    lexer_init(&lexer, "1 * 2 + 3 * 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);

    emit_expression(&generator, expression);

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_ir_generator()
{
    printf("Running IR generator tests...\n");

    test_emit_literal_expression();
    test_emit_binary_expression();

    test_emit_arithmetics();
}
