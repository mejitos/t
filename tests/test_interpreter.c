#include "tests.h"
#include "../src/t.h"
#include "../src/common.h"


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


/*
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


void test_evaluate_variable_declaration()
{
    printf("\tEvaluate variable declaration...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* tests[] =
    {
        "foo: int = 42;",
        "_bar: bool = true;",
    };
    
    const char* identifiers[] = { "foo", "_bar" };
    int values[] = { 42, 1 };

    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        // AST_Declaration* declaration = parse_declaration(&parser);
        AST_Statement* statement = parse_statement(&parser);
        
        assert(statement->kind == STATEMENT_DECLARATION);
        // assert(declaration->kind == DECLARATION_VARIABLE);
    
        resolver_init(&resolver);
        // NOTE(timo): We cant test these direct declaration resolving
        // functions since with them we don't have the symbols in the scope
        // resolve_declaration(&resolver, declaration);
        resolve_statement(&resolver, statement);
        
        interpreter_init(&interpreter);
        evaluate_declaration(&interpreter, statement->declaration);
        
        assert(interpreter.global->symbols->count == 1);
        
        Symbol* symbol = scope_lookup(resolver.global, identifiers[i]);

        assert(strcmp(symbol->identifier, identifiers[i]) == 0);

        Value value = symbol->value;

        if (value.type == VALUE_INTEGER)
            assert_value_integer(values[i], value);
        else if (value.type == VALUE_BOOLEAN)
            assert_value_boolean(values[i], value);
        
        // TODO(timo): teardown
        resolver_free(&resolver);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_evaluate_variable_expression()
{
    printf("\tEvaluate variable expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* source = "{\n"
                         "    foo: int = 42;\n"
                         "    foo;\n"
                         "}";
    
    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    AST_Statement* statement = parse_statement(&parser);
    
    assert(statement->kind == STATEMENT_BLOCK);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    interpreter_init(&interpreter);
    
    array* statements = statement->block.statements;
    evaluate_statement(&interpreter, statements->items[0]);
    
    assert(interpreter.global->symbols->count == 1);

    statement = statements->items[1];
    assert(statement->kind == STATEMENT_EXPRESSION);
    
    Value value = evaluate_expression(&interpreter, statement->expression);

    assert_value_integer(42, value);
    
    // TODO(timo): teardown
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_evaluate_assignment_expression()
{
    printf("\tEvaluate assignment expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* source = "{\n"
                         "    foo: int = 42;\n"
                         "    foo := 6;\n"
                         "    foo;\n"
                         "}";
    
    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    AST_Statement* statement = parse_statement(&parser);
    
    assert(statement->kind == STATEMENT_BLOCK);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    interpreter_init(&interpreter);
    
    array* statements = statement->block.statements;
    evaluate_statement(&interpreter, statements->items[0]);
    
    assert(interpreter.global->symbols->count == 1);

    statement = statements->items[1];
    assert(statement->kind == STATEMENT_EXPRESSION);
    assert(statement->expression->kind == EXPRESSION_ASSIGNMENT);
    assert(statement->expression->assignment.value->kind == EXPRESSION_LITERAL);
    Value value = evaluate_expression(&interpreter, statement->expression);
    assert_value_integer(6, value);
    
    statement = statements->items[2];
    assert(statement->kind == STATEMENT_EXPRESSION);
    assert(statement->expression->kind == EXPRESSION_VARIABLE);
    value = evaluate_expression(&interpreter, statement->expression);
    assert_value_integer(6, value);

    // TODO(timo): teardown
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}
*/


static void test_example_files(Test_Runner* runner)
{
    const char* files[] =
    {
        "./examples/first.t",
        "./examples/trivial_add.t",
        "./examples/trivial_subtract.t",
        "./examples/trivial_multiply.t",
        "./examples/trivial_divide.t",
        "./examples/trivial_arithmetics.t"
    };

    int results[] =
    {
        0, 2, 0, 6, 5, 7
    };
    
    for (int i = 0; i < sizeof (files) / sizeof (*files); i++)
    {
        Value return_value = interpret(read_file(files[i]));

        assert(return_value.integer == results[i]);
    }

    if (runner->error) runner->failed++;
    else runner->passed++;
}


Test_Set* interpreter_test_set()
{
    Test_Set* set = test_set("Interpreter");

    array_push(set->tests, test_case("Example files", test_example_files));

    set->length = set->tests->length;

    return set;
}


void test_interpreter()
{

    /*
    test_evaluate_literal_expression();
    test_evaluate_unary_expression();
    test_evaluate_binary_expression();
    test_evaluate_variable_declaration();
    test_evaluate_variable_expression();
    test_evaluate_assignment_expression();
    */
}
