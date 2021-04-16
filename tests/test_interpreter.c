#include "asserts.h"
#include "tests.h"
#include "../src/t.h"
#include "../src/common.h"


static void test_evaluate_literal_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "42",
        "true",
        "false",
    };

    int results[3][2] = 
    {
        {VALUE_INTEGER, 42},
        {VALUE_BOOLEAN, 1},
        {VALUE_BOOLEAN, 0}
    };

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Interpreter interpreter;
    
    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);
        
        interpreter_init(&interpreter, resolver.global);
        Value value = evaluate_expression(&interpreter, expression);

        assert_value(runner, value, results[i][0], results[i][1]);
        
        expression_free(expression);
        interpreter_free(&interpreter);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_evaluate_unary_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "+42",
        "-42",
        "not true",
        "not false",
    };

    int results[4][2] = 
    {
        {VALUE_INTEGER, 42},
        {VALUE_INTEGER, -42},
        {VALUE_BOOLEAN, 0},
        {VALUE_BOOLEAN, 1}
    };

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Interpreter interpreter;
    
    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);
        
        interpreter_init(&interpreter, resolver.global);
        Value value = evaluate_expression(&interpreter, expression);

        assert_value(runner, value, results[i][0], results[i][1]);
        
        expression_free(expression);
        interpreter_free(&interpreter);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_evaluate_binary_expression(Test_Runner* runner)
{
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

    int results[12][2] = 
    { 
        {VALUE_INTEGER, 3},
        {VALUE_INTEGER, -1},
        {VALUE_INTEGER, 20},
        {VALUE_INTEGER, 0},
        {VALUE_BOOLEAN, 0},
        {VALUE_BOOLEAN, 0},
        {VALUE_BOOLEAN, 1},
        {VALUE_BOOLEAN, 1},
        {VALUE_BOOLEAN, 0},
        {VALUE_BOOLEAN, 0},
        {VALUE_BOOLEAN, 1},
        {VALUE_BOOLEAN, 1}
    };

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Interpreter interpreter;
    
    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);
        
        interpreter_init(&interpreter, resolver.global);
        Value value = evaluate_expression(&interpreter, expression);

        assert_value(runner, value, results[i][0], results[i][1]);

        expression_free(expression);
        interpreter_free(&interpreter);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_evaluate_variable_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Interpreter interpreter;
    
    const char* source = "{\n"
                         "    foo: int = 42;\n"
                         "    foo;\n"
                         "}";
    
    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    AST_Statement* statement_block = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement_block);
    
    interpreter_init(&interpreter, resolver.global);
    
    array* statements = statement_block->block.statements;
    evaluate_statement(&interpreter, statements->items[0]);

    AST_Statement* statement = statements->items[1];
    Value value = evaluate_expression(&interpreter, statement->expression);

    assert_value(runner, value, VALUE_INTEGER, 42);
    
    statement_free(statement_block);
    interpreter_free(&interpreter);
    type_table_free(type_table);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_evaluate_assignment_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
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
    AST_Statement* statement_block = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement_block);
    
    interpreter_init(&interpreter, resolver.global);
    
    array* statements = statement_block->block.statements;
    evaluate_statement(&interpreter, statements->items[0]);
    
    // Check that the assignment operation returns the assigned value
    AST_Statement* statement = statements->items[1];
    Value value = evaluate_expression(&interpreter, statement->expression);

    assert_value(runner, value, VALUE_INTEGER, 6);
    
    // Check that the variable expression returns the newly assigned value
    statement = statements->items[2];
    value = evaluate_expression(&interpreter, statement->expression);

    assert_value(runner, value, VALUE_INTEGER, 6);

    statement_free(statement_block);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_evaluate_variable_declaration(Test_Runner* runner)
{
    const char* tests[] =
    {
        "foo: int = 42;",
        "_bar: bool = true;",
    };
    
    const char* identifiers[] = { "foo", "_bar" };
    int results[2][2] = { {VALUE_INTEGER, 42}, {VALUE_BOOLEAN, 1} };

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Interpreter interpreter;
    
    for (int i = 0; i < sizeof (tests) / sizeof(*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Declaration* declaration = parse_declaration(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_declaration(&resolver, declaration);
        
        interpreter_init(&interpreter, resolver.global);
        evaluate_declaration(&interpreter, declaration);
        
        Symbol* symbol = scope_lookup(interpreter.global, identifiers[i]);

        assert_base(runner, strcmp(symbol->identifier, identifiers[i]) == 0,
            "Invalid symbol identifier '%s', expected '%s'", symbol->identifier, identifiers[i]);

        Value value = symbol->value;

        assert_value(runner, value, results[i][0], results[i][1]);
        
        declaration_free(declaration);
        type_table_free(type_table);
        resolver_free(&resolver);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_first(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/first.t");
    assert_value(runner, return_value, VALUE_INTEGER, 0);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_trivial_add(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/trivial_add.t");
    assert_value(runner, return_value, VALUE_INTEGER, 2);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_trivial_subtract(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/trivial_subtract.t");
    assert_value(runner, return_value, VALUE_INTEGER, 0);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_trivial_multiply(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/trivial_multiply.t");
    assert_value(runner, return_value, VALUE_INTEGER, 6);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_trivial_divide(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/trivial_divide.t");
    assert_value(runner, return_value, VALUE_INTEGER, 5);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_trivial_arithmetics(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/trivial_arithmetics.t");
    assert_value(runner, return_value, VALUE_INTEGER, 7);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_if_1(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/if_1.t");
    assert_value(runner, return_value, VALUE_INTEGER, 0);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_if_7(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/if_7.t");
    assert_value(runner, return_value, VALUE_INTEGER, 1);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_while_1(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/while_loop_1.t");
    assert_value(runner, return_value, VALUE_INTEGER, 4950);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_example_while_2(Test_Runner* runner)
{
    Value return_value = interpret_from_file("./examples/while_loop_2.t");
    assert_value(runner, return_value, VALUE_INTEGER, 105);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


Test_Set* interpreter_test_set()
{
    Test_Set* set = test_set("Interpreter");

    // Expressions
    array_push(set->tests, test_case("Literal expression", test_evaluate_literal_expression));
    array_push(set->tests, test_case("Unary expression", test_evaluate_unary_expression));
    array_push(set->tests, test_case("Binary expression", test_evaluate_binary_expression));
    array_push(set->tests, test_case("Variable expression", test_evaluate_variable_expression));
    array_push(set->tests, test_case("Assignment expression", test_evaluate_assignment_expression));

    // Declarations
    array_push(set->tests, test_case("Variable declaration", test_evaluate_variable_declaration));

    // Example files
    array_push(set->tests, test_case("Example file: first.t", test_example_first));
    array_push(set->tests, test_case("Example file: trivial_add.t", test_example_trivial_add));
    array_push(set->tests, test_case("Example file: trivial_subtract.t", test_example_trivial_subtract));
    array_push(set->tests, test_case("Example file: trivial_multiply.t", test_example_trivial_multiply));
    array_push(set->tests, test_case("Example file: trivial_divide.t", test_example_trivial_divide));
    array_push(set->tests, test_case("Example file: trivial_arithmetics.t", test_example_trivial_arithmetics));

    array_push(set->tests, test_case("Example file: if_1.t", test_example_if_1));
    array_push(set->tests, test_case("Example file: if_7.t", test_example_if_7));

    array_push(set->tests, test_case("Example file: while_loop_1.t", test_example_while_1));
    array_push(set->tests, test_case("Example file: while_loop_2.t", test_example_while_2));

    set->length = set->tests->length;

    return set;
}
