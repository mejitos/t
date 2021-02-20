#include "tests.h"
#include "../src/t.h"


static bool not_error = true;


void assert_value_integer(int expected, Value actual)
{
    if (actual.type != TYPE_INTEGER)
    {
        printf("FAILED\n");
        printf("\t\tExpected integer type, got something else\n");
        not_error = false;
    }
    if (actual.integer != expected)
    {
        printf("FAILED\n");
        printf("\t\tExpected '%d', got '%d'\n", expected, actual.integer);
        not_error = false;
    }
}


void assert_value_boolean(bool expected, Value actual)
{
    if (actual.type != TYPE_BOOLEAN)
    {
        printf("FAILED\n");
        printf("\t\tExpected boolean type, got something else\n");
        not_error = false;
    }
    if (actual.boolean != expected)
    {
        printf("FAILED\n");
        printf("\t\tExpected '%d', got '%d'\n", expected, actual.boolean);
        not_error = false;
    }
}


void test_evaluate_literal_expression()
{
    printf("\tEvaluate literal expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* tests[] =
    {
        "42",
        "true",
        "false",
    };

    int results[] = { 42, 1, 0 };

    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);

        assert(expression->kind == EXPRESSION_LITERAL);
    
        resolver_init(&resolver);
        resolve_expression(&resolver, expression);
        
        interpreter_init(&interpreter);

        Value value = evaluate_expression(&interpreter, expression);

        if (value.type == VALUE_INTEGER)
            assert_value_integer(results[i], value);
        else if (value.type == VALUE_BOOLEAN)
            assert_value_boolean(results[i], value);
        
        // TODO(timo): teardown
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_evaluate_unary_expression()
{
    printf("\tEvaluate unary expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* tests[] =
    {
        "+42",
        "-42",
        "not true",
        "not false",
    };

    int results[] = { 42, -42, 0, 1 };

    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);

        assert(expression->kind == EXPRESSION_UNARY);
        assert(expression->unary.operand->kind == EXPRESSION_LITERAL);
    
        resolver_init(&resolver);
        resolve_expression(&resolver, expression);
        
        interpreter_init(&interpreter);

        Value value = evaluate_expression(&interpreter, expression);

        if (value.type == VALUE_INTEGER)
            assert_value_integer(results[i], value);
        else if (value.type == VALUE_BOOLEAN)
            assert_value_boolean(results[i], value);
        
        // TODO(timo): teardown
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_evaluate_binary_expression()
{
    printf("\tEvaluate binary expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* tests[] =
    {
        "1 + 2",
        "2 - 3",
        "4 * 5",
        "6 / 8",
        "1 == 0",
        "true == false",
        "0 != 1",
        "false != true",
        "1 < 0",
        "1 <= 0",
        "1 > 0",
        "1 >= 0",
    };

    int results[] = { 3, -1, 20, 0, 0, 0, 1, 1, 0, 0, 1, 1 };

    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);

        assert(expression->kind == EXPRESSION_BINARY);
        assert(expression->binary.left->kind == EXPRESSION_LITERAL);
        assert(expression->binary.right->kind == EXPRESSION_LITERAL);
    
        resolver_init(&resolver);
        resolve_expression(&resolver, expression);
        
        interpreter_init(&interpreter);

        Value value = evaluate_expression(&interpreter, expression);

        if (value.type == VALUE_INTEGER)
            assert_value_integer(results[i], value);
        else if (value.type == VALUE_BOOLEAN)
            assert_value_boolean(results[i], value);
        
        // TODO(timo): teardown
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_interpreter()
{
    printf("Running interpreter tests...\n");

    test_evaluate_literal_expression();
    test_evaluate_unary_expression();
    test_evaluate_binary_expression();
}
