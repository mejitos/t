#include "../src/t.h"
#include "tests.h"


static bool not_error = true;

static void test_emit_literal_expression()
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


static void test_emit_unary_expression()
{
    printf("\tUnary expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;

    lexer_init(&lexer, "-42");
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
    lexer_init(&lexer, "not false");
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

static void test_emit_binary_expression()
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


static void test_emit_variable_expression()
{
    //
}


static void test_emit_assignment_expression()
{
    printf("\tAssignment expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n    foo: int = 0;\n    foo := 1 * -2 + 3 * -4;\n}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    statement = parse_statement(&parser);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    emit_expression(&generator, expression);
    
    // This expression will be free'd with the statement
    // expression_free(expression);
    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("----------\n");

    // ----
    source = "{\n    a: int = 0;\n    b: int = 0;\n    c: int = 0;\n    a := b * -c + b * -c;\n}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    statement = parse_statement(&parser);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    expression = ((AST_Statement*)statement->block.statements->items[3])->expression;
    emit_expression(&generator, expression);

    // This expression will be free'd with the statement
    // expression_free(expression);
    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_emit_arithmetics()
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

    printf("--------\n");

    // ----
    lexer_init(&lexer, "1 * -2 + 3 * -4");
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
    test_emit_unary_expression();
    test_emit_binary_expression();
    test_emit_variable_expression();
    test_emit_assignment_expression();

    test_emit_arithmetics();
}
