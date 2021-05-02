//
// TODO(timo): Filedocstring
//

#include "asserts.h"
#include "tests.h"
#include "../src/t.h"


static void test_resolve_literal_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "42",
        "true",
        "false",
    };

    int results[3][2] =
    {
        { TYPE_INTEGER, 42 },
        { TYPE_BOOLEAN, 1 },
        { TYPE_BOOLEAN, 0 },
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        expression = parse_expression(&parser);

        assert_expression(runner, expression->kind, EXPRESSION_LITERAL);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);

        Type* type = resolve_expression(&resolver, expression);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_type(runner, type->kind, results[i][0]);

        if (type->kind == TYPE_INTEGER)
            assert_base(runner, expression->value.integer == results[i][1],
                "Invalid integer value %d, expected, %d", expression->value.integer, results[i][1]);
        else if (type->kind == TYPE_BOOLEAN)
            assert_base(runner, expression->value.boolean == results[i][1],
                "Invalid boolean value '%s', expected '%s'", expression->value.boolean ? "false" : "true", results[i][1] ? "false" : "true");

        expression_free(expression);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_diagnose_integer_overflow_literal(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;

    lexer_init(&lexer, "2147483648");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - OverflowError: Integer overflow in integer literal. Maximum integer value is abs(2147483647)";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_unary_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "+42",
        "-42",
        "not true",
    };

    Type_Kind results[] =
    {
        TYPE_INTEGER,
        TYPE_INTEGER,
        TYPE_BOOLEAN,
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        expression = parse_expression(&parser);

        assert_expression(runner, expression->kind, EXPRESSION_UNARY);
        assert_expression(runner, expression->unary.operand->kind, EXPRESSION_LITERAL);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        Type* type = resolve_expression(&resolver, expression);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_type(runner, type->kind, results[i]);

        expression_free(expression);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_diagnose_invalid_operand_types_unary_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "+true",
        "-false",
        "not 42",
    };

    const char* results[] =
    {
        ":RESOLVER - TypeError: Unsupported operand type 'bool' for unary '+'",
        ":RESOLVER - TypeError: Unsupported operand type 'bool' for unary '-'",
        ":RESOLVER - TypeError: Unsupported operand type 'int' for unary 'not'",
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;
    Diagnostic* diagnostic;

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        expression = parse_expression(&parser);
    
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);

        assert_base(runner, resolver.diagnostics->length == 1,
            "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

        diagnostic = resolver.diagnostics->items[0];

        assert_base(runner, strcmp(diagnostic->message, results[i]) == 0,
            "Invalid diagnostic '%s', expected '%s'", diagnostic->message, results[i]);

        expression_free(expression);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_resolve_binary_expression(Test_Runner* runner)
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

    Type_Kind results[] =
    {
        TYPE_INTEGER,
        TYPE_INTEGER,
        TYPE_INTEGER,
        TYPE_INTEGER,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
        TYPE_BOOLEAN,
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);

        assert_expression(runner, expression->kind, EXPRESSION_BINARY);
        assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
        assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        Type* type = resolve_expression(&resolver, expression);
        
        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_type(runner, type->kind, results[i]);

        expression_free(expression);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_diagnose_invalid_operand_types_binary_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "1 + true",
        "false - 42",
        "true * false",
        "false / false",
        "42 == false",
        "7 != true",
        "true < false",
        "true <= 4",
        "0 > false",
        "true >= false",
    };

    const char* results[] =
    {
        ":RESOLVER - TypeError: Unsupported operand types 'int' and 'bool' for binary '+'",
        ":RESOLVER - TypeError: Unsupported operand types 'bool' and 'int' for binary '-'",
        ":RESOLVER - TypeError: Unsupported operand types 'bool' and 'bool' for binary '*'",
        ":RESOLVER - TypeError: Unsupported operand types 'bool' and 'bool' for binary '/'",
        ":RESOLVER - TypeError: Unsupported operand types 'int' and 'bool' for binary '=='",
        ":RESOLVER - TypeError: Unsupported operand types 'int' and 'bool' for binary '!='",
        ":RESOLVER - TypeError: Unsupported operand types 'bool' and 'bool' for binary '<'",
        ":RESOLVER - TypeError: Unsupported operand types 'bool' and 'int' for binary '<='",
        ":RESOLVER - TypeError: Unsupported operand types 'int' and 'bool' for binary '>'",
        ":RESOLVER - TypeError: Unsupported operand types 'bool' and 'bool' for binary '>='",
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;
    Diagnostic* diagnostic;

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        expression = parse_expression(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);

        assert_base(runner, resolver.diagnostics->length == 1,
            "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

        diagnostic = resolver.diagnostics->items[0];

        assert_base(runner, strcmp(diagnostic->message, results[i]) == 0,
            "Invalid diagnostic '%s', expected '%s'", diagnostic->message, results[i]);

        expression_free(expression);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_resolve_logical_expression(Test_Runner* runner)
{
    const char* tests[] =
    {
        "true and true",
        "true and false",
        "false and true",
        "false and false",
        "true or true",
        "true or false",
        "false or true",
        "false or false"
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        AST_Expression* expression = parse_expression(&parser);

        assert_expression(runner, expression->kind, EXPRESSION_BINARY);
        assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
        assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        Type* type = resolve_expression(&resolver, expression);
        
        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_type(runner, type->kind, TYPE_BOOLEAN);

        expression_free(expression);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_resolve_variable_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Statement* statement;

    const char* source = "{\n    foo: int = 42;\n    foo;\n}";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    array* statements = statement->block.statements;

    assert_statement(runner, statement->kind, STATEMENT_BLOCK);
    assert_base(runner, statements->length == 2,
        "Invalid number of statements in block: %d, expected %2", statements->length);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statements->items[0]);
    
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in symbol table: %d, expected 1", resolver.global->symbols->count);
    
    Type* type = resolve_expression(&resolver, ((AST_Statement*)(statements->items[1]))->expression);
    
    assert_type(runner, type->kind, TYPE_INTEGER);
    
    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_referencing_identifier_before_declaring_variable(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;

    lexer_init(&lexer, "foo");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);
    
    message = ":RESOLVER - SyntaxError: Referencing identifier 'foo' before declaring it";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_assignment_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Statement* statement;
    AST_Statement* statement_block;
    Symbol* symbol;

    const char* source = "{\n    foo: int = 42;\n    foo := 7;\n}";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement_block = parse_statement(&parser);
    array* statements = statement_block->block.statements;

    assert_statement(runner, statement_block->kind, STATEMENT_BLOCK);
    assert_base(runner, statements->length == 2,
        "Invalid number of statements in block: %d, expected %2", statements->length);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);

    statement = statements->items[0];
    resolve_statement(&resolver, statement);

    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in symbol table: %d, expected 1", resolver.global->symbols->count);

    statement = statements->items[1];

    assert_statement(runner, statement->kind, STATEMENT_EXPRESSION);
    assert_expression(runner, statement->expression->kind, EXPRESSION_ASSIGNMENT);

    Type* type = resolve_expression(&resolver, statement->expression);

    assert_type(runner, type->kind, TYPE_INTEGER);
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    
    statement_free(statement_block);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_referencing_identifier_before_declaring_assignment(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // base case
    source = "foo := 1";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    assert_base(runner, resolver.diagnostics->length == 2,
        "Invalid number of resolver diagnostics: %d, expected 2", resolver.diagnostics->length);
    
    message = ":RESOLVER - SyntaxError: Referencing identifier 'foo' before declaring it";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    message = ":RESOLVER - TypeError: Conflicting types in assignment expression. Assigning to 'none' from 'int'";
    diagnostic = resolver.diagnostics->items[1];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_conflicting_types_assignment_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Statement* statement;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;

    const char* source = "{\n"
                         "    foo: int = 42;\n"
                         "    foo := true;"
                         "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: Conflicting types in assignment expression. Assigning to 'int' from 'bool'";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_index_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Expression* expression;
    AST_Declaration* declaration;

    // TODO(timo): In our case the array subscription needs more context to
    // be used, so therefore the testing needs all that context. We need to
    // handle the resolving of the function expression first
    const char* source = "main: int = (argc: int, argv: [int]) => {\n"
                         "    return argv[0];\n"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    
    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_referencing_identifier_before_declaring_index(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // base case
    source = "foo[777]";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    assert_base(runner, resolver.diagnostics->length == 2,
        "Invalid number of resolver diagnostics: %d, expected 2", resolver.diagnostics->length);
    
    message = ":RESOLVER - SyntaxError: Referencing identifier 'foo' before declaring it";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    message = ":RESOLVER - TypeError: 'none' is not subscriptable.";
    diagnostic = resolver.diagnostics->items[1];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_type_is_not_subscriptable_variable(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // integer
    source = "main: int = (argc: int, argv: [int]) => {\n"
             "    foo: int = 42;\n"
             "    return foo[2];\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: 'int' is not subscriptable.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // bool
    source = "main: int = (argc: int, argv: [int]) => {\n"
             "    foo: bool = true;\n"
             "    return foo[2];\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: 'bool' is not subscriptable.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_type_is_not_subscriptable_function(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    source = "func: bool = () => { return true; };\n\n"
             "main: int = (argc: int, argv: [int]) => {\n"
             "    return func[2];\n"
             "};";
    
    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: 'function' is not subscriptable.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_invalid_array_subscript(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    source = "main: int = (argc: int, argv: [int]) => {\n"
             "    return argv[false];\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: Array subscripts must be integers.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_invalid_array_subscript_boundaries(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    source = "main: int = (argc: int, argv: [int]) => {\n"
             "    return argv[-1];\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - IndexError: Array subscript less than zero.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_array_subscript_not_argv(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    source = "main: int = (argc: int, foo: [int]) => {\n"
             "    return foo[2];\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - Error: You cannot use index expressions with other values than argv of the main function";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_array_subscript_not_main(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    source = "func: int = (argc: int, argv: [int]) => {\n"
             "    return argv[2];\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - Error: You cannot use index expressions with other values than argv of the main function";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_function_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Expression* expression;
    Type* type;
    char* source;

    // with parameters
    source = "(argc: int, argv: [int]) => {\n"
             "    return 0;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_FUNCTION);
    assert_base(runner, expression->function.arity == 2,
        "Function has %d parameters, expected 2", expression->function.arity);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    // This has to be set to get some context to the function
    resolver.context.current_function = "main";
    type = resolve_expression(&resolver, expression);
    
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_type(runner, type->kind, TYPE_FUNCTION);
    assert_base(runner, type->function.return_type->kind == TYPE_INTEGER,
        "Unexpected function return type '%s', expected 'int'", type_as_string(type->function.return_type->kind));
    assert_base(runner, type->function.scope->symbols->count == 2,
        "Invalid number of symbols in functions scope: %d, expected 2", type->function.scope->symbols->count);
    
    type_free(type);
    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // without parameters 
    source = "() => {\n"
             "    foo: bool = true;\n"
             "    return 0;"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_FUNCTION);
    assert_base(runner, expression->function.arity == 0,
        "Function has %d parameters, expected 0", expression->function.arity);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    // NOTE(timo): The function has to be in named context to get the scoping work
    resolver.context.current_function = "anonymous";
    type = resolve_expression(&resolver, expression);

    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_type(runner, type->kind, TYPE_FUNCTION);
    assert_base(runner, type->function.return_type->kind == TYPE_INTEGER,
        "Unexpected function return type '%s', expected 'int'", type_as_string(type->function.return_type->kind));
    assert_base(runner, type->function.scope->symbols->count == 1,
        "Invalid number of symbols in functions scope: %d, expected 1", type->function.scope->symbols->count);
    
    type_free(type);
    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_redeclaration_of_identifier_function_parameter(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;
    char* source;
    
    // parameters the same name
    source = "foo: int = (bar: int, bar: bool) => { return 0; };\n";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = "RESOLVER - NameError: Redeclaration of identifier 'bar' in 'foo'";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): parameter name same as function name? they can't be same.
}


static void test_resolve_call_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    char* source;

    // with arguments 
    source = "greater: bool = (x: int, y: int) => {\n"
             "    return x > y;\n"
             "};"
             "\n"
             "main: int = (argc: int, argv: [int]) => {\n"
             "   greater(1, 0);\n"
             "   return 0;"
             "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): no arguments 

    // TODO(timo): assigning a value to variable from call expression
}


static void test_constant_folding_binary_arithmetics(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    char* source;

    // global variable
    const char* tests[] =
    {
        "foo: int = 1 + 2;",
        "foo: int = 3 - 4;",
        "foo: int = 5 * 6;",
        "foo: int = 7 / 8;",
    };

    const int expected[] = { 3, -1, 30, 0 };

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]); 
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        declaration = parse_declaration(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_declaration(&resolver, declaration);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_value(runner, declaration->initializer->value, VALUE_INTEGER, expected[i]);

        declaration_free(declaration);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    // TODO(timo): local variables
}


static void test_constant_folding_binary_equality(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    char* source;

    // global variable
    const char* tests[] =
    {
        "foo: bool = 1 == 2;",
        "foo: bool = 3 != 4;",
        "foo: bool = true == false;",
        "foo: bool = false != true;",
    };

    const bool expected[] = { false, true, false, true };

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]); 
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        declaration = parse_declaration(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_declaration(&resolver, declaration);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_value(runner, declaration->initializer->value, VALUE_BOOLEAN, expected[i]);

        declaration_free(declaration);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    // TODO(timo): local variables
}


static void test_constant_folding_binary_relation(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    char* source;

    // global variable
    const char* tests[] =
    {
        "foo: bool = 1 < 2;",
        "foo: bool = 3 <= 4;",
        "foo: bool = 5 > 6;",
        "foo: bool = 7 >= 8;",
    };

    const bool expected[] = { true, true, false, false };

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]); 
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        declaration = parse_declaration(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_declaration(&resolver, declaration);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_value(runner, declaration->initializer->value, VALUE_BOOLEAN, expected[i]);

        declaration_free(declaration);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    // TODO(timo): local variables
    // TODO(timo): condition expressions
}


static void test_constant_folding_binary_logical(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    char* source;

    // global variable
    const char* tests[] =
    {
        "foo: bool = true and false;",
        "foo: bool = true or false;",
        "foo: bool = not true;",
        "foo: bool = not false;",
    };

    const bool expected[] = { false, true, false, true };

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]); 
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        declaration = parse_declaration(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_declaration(&resolver, declaration);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_value(runner, declaration->initializer->value, VALUE_BOOLEAN, expected[i]);

        declaration_free(declaration);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    // TODO(timo): local variables
    // TODO(timo): condition expressions
}


static void test_constant_folding_complex_arithmetics(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    char* source;

    // global variable
    const char* tests[] =
    {
        "foo: int = 1 + 2 * 3;",
        "foo: int = (1 + 2) * 3;",
        "foo: int = (1 + 2) * (3 - 4);",
        "foo: int = (1 + 2) * (3 - 4) / -3;",
    };

    const int expected[] = { 7, 9, -3, 1 };

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]); 
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        declaration = parse_declaration(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_declaration(&resolver, declaration);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_value(runner, declaration->initializer->value, VALUE_INTEGER, expected[i]);

        declaration_free(declaration);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }

    // TODO(timo): local variables
}


static void test_diagnose_callee_is_not_callable(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;
    
    // Global variable
    const char* source = "greater: bool = true;\n"
                         "main: int = (argc: int, argv: [int]) => {\n"
                         "   greater(1, 0);\n"
                         "   return 0;"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: 'greater' is not callable.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): local variable

    // TODO(timo): literals

    // TODO(timo): array
}


static void test_diagnose_invalid_number_of_arguments(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;

    const char* source = "greater: bool = (x: int, y: int) => {\n"
                         "    return x > y;\n"
                         "};"
                         "\n"
                         "main: int = (argc: int, argv: [int]) => {\n"
                         "   greater(1);\n"
                         "   return 0;"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: Function 'greater' expected 2 arguments, but 1 was given\n", 
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_invalid_type_of_argument(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;

    const char* source = "greater: bool = (x: int, y: int) => {\n"
                         "    return x > y;\n"
                         "};"
                         "\n"
                         "main: int = (argc: int, argv: [int]) => {\n"
                         "   greater(1, true);\n"
                         "   return 0;"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - TypeError: Parameter 'y' is of type 'int', but argument of type 'bool' was given.", 
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_declaration_statement_variable(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Symbol* symbol;
    AST_Statement* statement;
    AST_Declaration* declaration;

    // ---- int
    lexer_init(&lexer, "foo: int = 42;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    declaration = statement->declaration;

    assert_declaration(runner, declaration->kind, DECLARATION_VARIABLE);
    assert_type_specifier(runner, declaration->specifier, TYPE_SPECIFIER_INT);
    assert_type(runner, declaration->initializer->literal->kind, TOKEN_INTEGER_LITERAL);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);

    assert_base(runner, declaration->initializer->value.integer == 42,
        "Invalid value '%d', expected 42", declaration->initializer->value.integer);
    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in the symbol table: %d, expected 1", resolver.global->symbols->count);

    symbol = scope_lookup(resolver.global, "foo");

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "foo") == 0,
        "Invalid symbol identifier '%s', expected 'foo'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_INTEGER);

    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // ---- bool
    lexer_init(&lexer, "_bar: bool = false;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    declaration = statement->declaration;

    assert_declaration(runner, declaration->kind, DECLARATION_VARIABLE);
    assert_type_specifier(runner, declaration->specifier, TYPE_SPECIFIER_BOOL);
    assert_type(runner, declaration->initializer->literal->kind, TOKEN_BOOLEAN_LITERAL);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    assert_base(runner, declaration->initializer->value.boolean == false,
        "Invalid value '%s', expected false", declaration->initializer->value.boolean ? "true" : "false");
    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in the symbol table: %d, expected 1", resolver.global->symbols->count);

    symbol = scope_lookup(resolver.global, "_bar");

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "_bar") == 0,
        "Invalid symbol identifier '%s', expected '_bar'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_BOOLEAN);

    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}

    
static void test_resolve_if_statement(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Statement* statement;
    
    // If with then
    lexer_init(&lexer, "if 0 < 1 then { }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert_statement(runner, statement->kind, STATEMENT_IF);
    assert_expression(runner, statement->_if.condition->kind, EXPRESSION_BINARY);
    assert_statement(runner, statement->_if.then->kind, STATEMENT_BLOCK);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);

    Type* condition_type = resolve_expression(&resolver, statement->_if.condition);

    assert_type(runner, condition_type->kind, TYPE_BOOLEAN);

    resolve_statement(&resolver, statement);
    
    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): If with then and else
    // TODO(timo): If with else ifs
    // TODO(timo): multiple ifs with one else at end
}


static void test_resolve_while_statement(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Statement* statement;
    
    lexer_init(&lexer, "while 0 < 1 do { }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert_statement(runner, statement->kind, STATEMENT_WHILE);
    assert_expression(runner, statement->_while.condition->kind, EXPRESSION_BINARY);
    assert_statement(runner, statement->_while.body->kind, STATEMENT_BLOCK);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);

    Type* condition_type = resolve_expression(&resolver, statement->_while.condition);

    assert_type(runner, condition_type->kind, TYPE_BOOLEAN);

    resolve_statement(&resolver, statement);
    
    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_return_statement(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Expression* return_value;

    const char* source = "return 0;";

    // integer literal
    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    AST_Statement* statement = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);

    Type* return_type = resolve_expression(&resolver, statement->_return.value);

    assert_type(runner, return_type->kind, TYPE_INTEGER);
    
    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // TODO(timo): variable

    // TODO(timo): value from array subscript

    // TODO(timo): value from function call
}


static void test_diagnose_no_return_statement_outside_functions(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Expression* return_value;
    Diagnostic* diagnostic;

    // return statement at top level
    const char* source = "return 0;";
    const char* result = ":RESOLVER - SyntaxError: Can't return outside of functions";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    AST_Statement* statement = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, result) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, result);
    
    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): return statements somewhere else in wrong places?
}


static void test_diagnose_function_has_to_return_value(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;

    // return statement at top level
    const char* source = "main: int (argc: int, argv: [int]) => {\n"
                         "    1 + 1;\n"
                         "};";
    const char* result = ":RESOLVER - SyntaxError: Function has to return a value.";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, result) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, result);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_return_statement_must_return_value(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;

    const char* source = "main: int (argc: int, argv: [int]) => {\n"
                         "    return;\n"
                         "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 2,
        "Invalid number of resolver diagnostics: %d, expected 2", resolver.diagnostics->length);

    message = ":RESOLVER - SyntaxError: Return statement must return a value.";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
   
    // NOTE(timo): This is actually a wrong message because in this situation
    // the function doesnt actually even have a return type, it is NULL.
    // message = ":RESOLVER - TypeError: Conflicting types in function declaration. Declaring type 'none' to 'int'";
    message = ":RESOLVER - SyntaxError: Function has to return a value.";
    diagnostic = resolver.diagnostics->items[1];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_yoro(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // same type of return values
    source = "main: int (argc: int, argv: [int]) => {\n"
             "    return 0;\n"
             "    return 0;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - SyntaxError: You can only return once from function";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // different type of return values
    source = "main: int (argc: int, argv: [int]) => {\n"
             "    return true;\n"
             "    return 0;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = ":RESOLVER - SyntaxError: You can only return once from function";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // only the "last" return will be taken into account when resolving types
    source = "main: int (argc: int, argv: [int]) => {\n"
             "    return 0;\n"
             "    return true;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 2,
        "Invalid number of resolver diagnostics: %d, expected 2", resolver.diagnostics->length);

    message = ":RESOLVER - SyntaxError: You can only return once from function";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    message = ":RESOLVER - TypeError: Conflicting types in function declaration. Declaring type 'bool' to 'int'";
    diagnostic = resolver.diagnostics->items[1];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);
    
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_break_statement(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Statement* statement;

    const char* source = "while 1 > 0 do { break; }";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);

    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);

    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_no_break_statement_outside_loops(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    
    const char* tests[] =
    {
        "{ break; }",
        "if 1 < 0 then { break; }",
    };

    const char* result = ":RESOLVER - SyntaxError: Cant't break outside of loops.";

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);
        
        parser_init(&parser, lexer.tokens);
        statement = parse_statement(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_statement(&resolver, statement);

        assert_base(runner, resolver.diagnostics->length == 1,
            "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

        diagnostic = resolver.diagnostics->items[0];

        assert_base(runner, strcmp(diagnostic->message, result) == 0,
            "Invalid diagnostic '%s', expected '%s'", diagnostic->message, result);
        
        statement_free(statement);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_resolve_continue_statement(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Statement* statement;

    const char* source = "while 1 > 0 do { continue; }";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);

    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);

    statement_free(statement);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_no_continue_statement_outside_loops(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    
    const char* tests[] =
    {
        "{ continue; }",
        "if 1 < 0 then { continue; }",
    };

    const char* result = ":RESOLVER - SyntaxError: Cant't continue outside of loops.";

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);
        
        parser_init(&parser, lexer.tokens);
        statement = parse_statement(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_statement(&resolver, statement);

        assert_base(runner, resolver.diagnostics->length == 1,
            "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

        diagnostic = resolver.diagnostics->items[0];

        assert_base(runner, strcmp(diagnostic->message, result) == 0,
            "Invalid diagnostic '%s', expected '%s'", diagnostic->message, result);
        
        statement_free(statement);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_resolve_variable_declaration(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Symbol* symbol;
    AST_Declaration* declaration;

    // ---- int
    lexer_init(&lexer, "foo: int = 42;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration(runner, declaration->kind, DECLARATION_VARIABLE);
    assert_type_specifier(runner, declaration->specifier, TYPE_SPECIFIER_INT);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);
    
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_type(runner, declaration->initializer->literal->kind, TOKEN_INTEGER_LITERAL);
    assert_base(runner, declaration->initializer->value.integer == 42,
        "Invalid value '%d', expected 42", declaration->initializer->value.integer);
    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in the symbol table: %d, expected 1", resolver.global->symbols->count);

    symbol = scope_lookup(resolver.global, "foo");

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "foo") == 0,
        "Invalid symbol identifier '%s', expected 'foo'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_INTEGER);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // ---- bool
    lexer_init(&lexer, "_bar: bool = false;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration(runner, declaration->kind, DECLARATION_VARIABLE);
    assert_type_specifier(runner, declaration->specifier, TYPE_SPECIFIER_BOOL);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);

    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_type(runner, declaration->initializer->literal->kind, TOKEN_BOOLEAN_LITERAL);
    assert_base(runner, declaration->initializer->value.boolean == false,
        "Invalid value '%s', expected 'false'", declaration->initializer->value.boolean ? "true" : "false");
    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in the symbol table: %d, expected 1", resolver.global->symbols->count);

    symbol = scope_lookup(resolver.global, "_bar");

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "_bar") == 0,
        "Invalid symbol identifier '%s', expected '_bar'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_BOOLEAN);

    declaration_free(declaration);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_multiple_global_variable_declarations(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Declaration* declaration;
    Symbol* symbol;

    const char* source = "foo: int = 42;"
                         "_bar: bool = false;"
                         "FOOBAR: int = 0;";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    array* declarations = parser.declarations;

    assert_base(runner, declarations->length == 3,
        "Invalid number of declarations: %d, expected 3", declarations->length);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, declarations);
    
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_base(runner, resolver.global->symbols->count == 3,
        "Invalid number of symbols in the symbol table: %d, expected 3", resolver.global->symbols->count);

    // ---- symbol 1
    declaration = declarations->items[0];
    symbol = scope_lookup(resolver.global, "foo");

    assert_type(runner, declaration->initializer->literal->kind, TOKEN_INTEGER_LITERAL);
    assert_base(runner, declaration->initializer->value.integer == 42,
        "Invalid value '%d', expected '42'", declaration->initializer->value.integer);

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "foo") == 0,
        "Invalid symbol identifier '%s', expected 'foo'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_INTEGER);

    // ---- symbol 2
    declaration = declarations->items[1];
    symbol = scope_lookup(resolver.global, "_bar");

    assert_type(runner, declaration->initializer->literal->kind, TOKEN_BOOLEAN_LITERAL);
    assert_base(runner, declaration->initializer->value.boolean == false,
        "Invalid value '%s', expected 'false'", declaration->initializer->value.boolean ? "true" : "false");

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "_bar") == 0,
        "Invalid symbol identifier '%s', expected '_bar'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_BOOLEAN);

    // ---- symbol 3
    declaration = declarations->items[2];
    symbol = scope_lookup(resolver.global, "FOOBAR");

    assert_type(runner, declaration->initializer->literal->kind, TOKEN_INTEGER_LITERAL);
    assert_base(runner, declaration->initializer->value.integer == 0,
        "Invalid value '%d', expected '0'", declaration->initializer->value.integer);

    assert_base(runner, symbol->kind == SYMBOL_VARIABLE,
        "Invalid symbol kind");
    assert_base(runner, strcmp(symbol->identifier, "FOOBAR") == 0,
        "Invalid symbol identifier '%s', expected 'FOOBAR'", symbol->identifier);
    assert_type(runner, symbol->type->kind, TYPE_INTEGER);
    
    // ---- teardown
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_variable_declaration_const_literal_only(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    char* source = "foo: int = 42;\n"
                   "bar: int = foo + 1;\n";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    char* message = ":RESOLVER - Error: Global variable initializer has to be compile-time constant.";
    Diagnostic* diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_redeclaration_of_identifier_variable_declaration(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // global variables
    source = "foo: int = 42;\n"
             "foo: int = 7;\n";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = "RESOLVER - NameError: Redeclaration of identifier 'foo' in 'global'";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // local variables
    source = "main: int () => {\n"
             "    foo: int = 42;\n"
             "    foo: bool = true;\n"
             "\n"
             "    return 0;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = "RESOLVER - NameError: Redeclaration of identifier 'foo' in 'main'";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_function_declaration(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    AST_Expression* return_value;

    const char* source = "main: int = (argc: int, argv: int) => {\n"
                         "    return 0;\n"
                         "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);

    assert_base(runner, parser.declarations->length == 1,
        "Invalid number of declarations: %d, expected 1", parser.declarations->length);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 0,
        "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
    assert_base(runner, resolver.global->symbols->count == 1,
        "Invalid number of symbols in the symbol table: %d, expected 1", resolver.global->symbols->count);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_diagnose_redeclaration_of_identifier_function_declaration(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // global variables
    source = "foo: int = () => { return 0; };\n\n"
             "foo: int = () => { return 1; };\n";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert_base(runner, resolver.diagnostics->length == 1,
        "Invalid number of resolver diagnostics: %d, expected 1", resolver.diagnostics->length);

    message = "RESOLVER - NameError: Redeclaration of identifier 'foo' in 'global'";
    diagnostic = resolver.diagnostics->items[0];

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
        "Invalid diagnostic '%s', expected '%s'", diagnostic->message, message);

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_resolve_type_specifier(Test_Runner* runner)
{
    const char* tests[] =
    {
        "int",
        "bool",
        "[int]",
    };

    int expected[3][2] =
    {
        { TYPE_SPECIFIER_INT, TYPE_INTEGER },
        { TYPE_SPECIFIER_BOOL, TYPE_BOOLEAN },
        { TYPE_SPECIFIER_ARRAY_INT, TYPE_ARRAY },
    };

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    hashtable* type_table;
    Type_Specifier specifier;

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        specifier = parse_type_specifier(&parser);

        assert_type_specifier(runner, specifier, expected[i][0]);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);

        Type* type = resolve_type_specifier(&resolver, specifier);

        assert_base(runner, resolver.diagnostics->length == 0,
            "Invalid number of resolver diagnostics: %d, expected 0", resolver.diagnostics->length);
        assert_type(runner, type->kind, expected[i][1]);

        if (type->kind == TYPE_ARRAY)
            type_free(type);

        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


Test_Set* resolver_test_set()
{
    Test_Set* set = test_set("Resolver");

    // Literals
    array_push(set->tests, test_case("Literal expression", test_resolve_literal_expression));
    array_push(set->tests, test_case("Diagnose integer overlow in integer literal", test_diagnose_integer_overflow_literal));

    // Unary
    array_push(set->tests, test_case("Unary expression", test_resolve_unary_expression));
    array_push(set->tests, test_case("Diagnose invalid unary operands", test_diagnose_invalid_operand_types_unary_expression));
    // TODO(timo): Integer overflow checks in unary (positive and negative)

    // Binary
    array_push(set->tests, test_case("Binary expression", test_resolve_binary_expression));
    array_push(set->tests, test_case("Diagnose invalid binary operands", test_diagnose_invalid_operand_types_binary_expression));
    // TODO(timo): Integer overflow checks in binary (positive and negative)

    // Logical expressions
    array_push(set->tests, test_case("Logical expression", test_resolve_logical_expression));

    // Variable
    array_push(set->tests, test_case("Variable expression", test_resolve_variable_expression));
    array_push(set->tests, test_case("Diagnose referencing identifier before declaring it (variable)", test_diagnose_referencing_identifier_before_declaring_variable));

    // Assignment
    array_push(set->tests, test_case("Assignment expression", test_resolve_assignment_expression));
    array_push(set->tests, test_case("Diagnose referencing identifier before declaring it (assignment)", test_diagnose_referencing_identifier_before_declaring_assignment));
    array_push(set->tests, test_case("Diagnose conflicting types in assignment", test_diagnose_conflicting_types_assignment_expression));

    // Index expression / subscript
    array_push(set->tests, test_case("Index expression", test_resolve_index_expression));
    array_push(set->tests, test_case("Diagnose referencing identifier before declaring it (index)", test_diagnose_referencing_identifier_before_declaring_index));
    // NOTE(timo): Literal cannot be used since parser should disregard everything except variable expressions
    // array_push(set->tests, test_case("Diagnose type is not subscriptable (literal)", test_diagnose_type_is_not_subscriptable_literal));
    array_push(set->tests, test_case("Diagnose type is not subscriptable (variable)", test_diagnose_type_is_not_subscriptable_variable));
    array_push(set->tests, test_case("Diagnose type is not subscriptable (function)", test_diagnose_type_is_not_subscriptable_function));
    array_push(set->tests, test_case("Diagnose invalid array subscript type", test_diagnose_invalid_array_subscript));
    array_push(set->tests, test_case("Diagnose invalid array subscript boundaries", test_diagnose_invalid_array_subscript_boundaries));
    // TODO(timo): These two can be removed after the language has arrays
    array_push(set->tests, test_case("Diagnose subcript target is not argv", test_diagnose_array_subscript_not_argv));
    array_push(set->tests, test_case("Diagnose subscript context is not main program", test_diagnose_array_subscript_not_main));

    // Function expression
    array_push(set->tests, test_case("Function expression", test_resolve_function_expression));
    array_push(set->tests, test_case("Diagnose redeclaration of identifier (function parameter)", test_diagnose_redeclaration_of_identifier_function_parameter));
    // TODO(timo): Function cannot be declared inside a function

    // Call expression
    array_push(set->tests, test_case("Call expression", test_resolve_call_expression));
    // TODO(timo): Diagnose referencing variable before declaring it
    array_push(set->tests, test_case("Diagnose callee is not callable", test_diagnose_callee_is_not_callable));
    array_push(set->tests, test_case("Diagnose invalid number of arguments", test_diagnose_invalid_number_of_arguments));
    array_push(set->tests, test_case("Diagnose invalid type of argument", test_diagnose_invalid_type_of_argument));

    // Constant folding
    array_push(set->tests, test_case("Constant folding (binary arithmetics)", test_constant_folding_binary_arithmetics));
    array_push(set->tests, test_case("Constant folding (binary equality)", test_constant_folding_binary_equality));
    array_push(set->tests, test_case("Constant folding (binary relation)", test_constant_folding_binary_relation));
    array_push(set->tests, test_case("Constant folding (binary logical)", test_constant_folding_binary_logical));
    array_push(set->tests, test_case("Constant folding (complex arithmetics)", test_constant_folding_complex_arithmetics));

    // Expressions MISC
    // TODO(timo): You can only use declarations at the top level so no
    // statements or expressions are allowed at top level.

    // Statements
    array_push(set->tests, test_case("Declaration statement (variable)", test_resolve_declaration_statement_variable));
    array_push(set->tests, test_case("If statement", test_resolve_if_statement));
    array_push(set->tests, test_case("While statement", test_resolve_while_statement));

    // Return statement
    array_push(set->tests, test_case("Return statement", test_resolve_return_statement));
    array_push(set->tests, test_case("Diagnose no return outside functions", test_diagnose_no_return_statement_outside_functions));
    array_push(set->tests, test_case("Diagnose function has to return value", test_diagnose_function_has_to_return_value));
    array_push(set->tests, test_case("Diagnose return statement has to return value", test_diagnose_return_statement_must_return_value));
    array_push(set->tests, test_case("Diagnose you can only return once", test_diagnose_yoro));

    // Break statement
    array_push(set->tests, test_case("Break statement", test_resolve_break_statement));
    array_push(set->tests, test_case("Diagnose no break outside loops", test_diagnose_no_break_statement_outside_loops));

    // Continue statement
    array_push(set->tests, test_case("Continue statement", test_resolve_continue_statement));
    array_push(set->tests, test_case("Diagnose no continue outside loops", test_diagnose_no_continue_statement_outside_loops));

    // Statements MISC
    // TODO(timo): You can only use declarations at the top level so no
    // statements or expressions are allowed at top level.

    // Variable declarations
    array_push(set->tests, test_case("Global variable declaration", test_resolve_variable_declaration));
    array_push(set->tests, test_case("Multiple global variable declarations", test_resolve_multiple_global_variable_declarations));
    array_push(set->tests, test_case("Diagnose global variable declaration has to be literal constant", test_diagnose_variable_declaration_const_literal_only));
    array_push(set->tests, test_case("Diagnose redeclaration of identifier (variable declaration)", test_diagnose_redeclaration_of_identifier_variable_declaration));
    // TODO(timo): Create global variable foo and local variable foo
    
    // Function declarations
    array_push(set->tests, test_case("Function declaration", test_resolve_function_declaration));
    array_push(set->tests, test_case("Diagnose redeclaration of identifier (function declaration)", test_diagnose_redeclaration_of_identifier_function_declaration));
    // TODO(timo): Diagnose invalid type of the return value.
    // TODO(timo): Function cannot be declared inside a function

    // Type specifiers
    array_push(set->tests, test_case("Type specifier", test_resolve_type_specifier));
    
    // MISC / TODO

    // ---
    

    // ----

    // Scoping
    // TODO(timo): test_resolve_local_scopes();
    
    set->length = set->tests->length;

    return set;
}
