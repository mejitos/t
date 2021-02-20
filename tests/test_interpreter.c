#include "tests.h"
#include "../src/t.h"


static bool not_error = true;


void assert_value_integer(int expected, Value actual)
{
    if (actual.type != TYPE_INTEGER)
    {
        printf("Expected integer type, got something else");
        not_error = false;
    }
    if (actual.integer != expected)
    {
        printf("Expected '%d', got '%d'", expected, actual.integer);
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

        if (value.type == TYPE_INTEGER)
            assert_value_integer(results[i], value);
        else if (value.type == TYPE_BOOLEAN)
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

    // test_evaluate_unary_expression();

    // test_evaluate_binary_expression();
}
