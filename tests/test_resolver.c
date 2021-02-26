#include "tests.h"
#include "../src/t.h"


static bool not_error = true;


static void teardown(Resolver* resolver, Parser* parser, Lexer* lexer)
{
    if (resolver != NULL) resolver_free(resolver);
    if (parser != NULL) parser_free(parser);
    if (lexer != NULL) lexer_free(lexer);
}


void test_resolve_literal_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tLiteral expression...");
    not_error = true;

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

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(lexer, tests[i]);
        lex(lexer);

        parser_init(parser, lexer->tokens);
        AST_Expression* expression = parse_expression(parser);

        resolver_init(resolver);

        assert(expression->kind == EXPRESSION_LITERAL);

        resolve_expression(resolver, expression);

        assert(expression->type->kind == results[i][0]);

        if (expression->type->kind == TYPE_INTEGER)
            assert(expression->literal.value.integer == results[i][1]);
        else if (expression->type->kind == TYPE_BOOLEAN)
            assert(expression->literal.value.boolean == results[i][1]);

        expression_free(expression);
        resolver_free(resolver);
        parser_free(parser);
        lexer_free(lexer);
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolve_unary_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tUnary expression...");
    not_error = true;

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

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(lexer, tests[i]);
        lex(lexer);

        parser_init(parser, lexer->tokens);
        AST_Expression* expression = parse_expression(parser);

        resolver_init(resolver);

        assert(expression->kind == EXPRESSION_UNARY);
        assert(expression->unary.operand->kind == EXPRESSION_LITERAL);

        resolve_expression(resolver, expression);

        assert(expression->type->kind == results[i]);

        expression_free(expression);
        resolver_free(resolver);
        parser_free(parser);
        lexer_free(lexer);
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolve_binary_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tBinary expression...");
    not_error = true;

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

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(lexer, tests[i]);
        lex(lexer);

        parser_init(parser, lexer->tokens);
        AST_Expression* expression = parse_expression(parser);

        resolver_init(resolver);

        assert(expression->kind == EXPRESSION_BINARY);
        assert(expression->binary.left->kind == EXPRESSION_LITERAL);
        assert(expression->binary.right->kind == EXPRESSION_LITERAL);

        resolve_expression(resolver, expression);

        assert(expression->type->kind == results[i]);

        expression_free(expression);
        resolver_free(resolver);
        parser_free(parser);
        lexer_free(lexer);
    }

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolve_variable_declaration(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tVariable declaration...");
    not_error = true;

    Symbol* symbol;
    AST_Declaration* declaration;

    // ---- int
    lexer_init(lexer, "foo: int = 42;");
    lex(lexer);

    parser_init(parser, lexer->tokens);
    declaration = parse_declaration(parser);

    resolver_init(resolver);

    assert(declaration->kind == DECLARATION_VARIABLE);
    assert(declaration->specifier == TYPE_SPECIFIER_INT);

    resolve_declaration(resolver, declaration);

    assert(resolver->global->symbols->length == 1);

    assert(declaration->initializer->literal.literal->kind == TOKEN_INTEGER_LITERAL);
    assert(declaration->initializer->literal.value.integer == 42);

    symbol = resolver->global->symbols->items[0];

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "foo") == 0);
    assert(symbol->type->kind == TYPE_INTEGER);

    declaration_free(declaration);
    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);


    // ---- bool
    lexer_init(lexer, "_bar: bool = false;");
    lex(lexer);

    parser_init(parser, lexer->tokens);
    declaration = parse_declaration(parser);

    resolver_init(resolver);

    assert(declaration->kind == DECLARATION_VARIABLE);
    assert(declaration->specifier == TYPE_SPECIFIER_BOOL);

    resolve_declaration(resolver, declaration);

    assert(resolver->global->symbols->length == 1);

    assert(declaration->initializer->literal.literal->kind == TOKEN_BOOLEAN_LITERAL);
    assert(declaration->initializer->literal.value.boolean == false);

    symbol = resolver->global->symbols->items[0];

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "_bar") == 0);
    assert(symbol->type->kind == TYPE_BOOLEAN);

    declaration_free(declaration);
    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolve_declaration_statement_variable(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tVariable declaration statement...");
    not_error = true;

    Symbol* symbol;
    AST_Statement* statement;
    AST_Declaration* declaration;

    // ---- int
    lexer_init(lexer, "foo: int = 42;");
    lex(lexer);

    parser_init(parser, lexer->tokens);
    statement = parse_statement(parser);

    resolver_init(resolver);

    declaration = statement->declaration;
    assert(declaration->kind == DECLARATION_VARIABLE);
    assert(declaration->specifier == TYPE_SPECIFIER_INT);
    assert(declaration->initializer->literal.literal->kind == TOKEN_INTEGER_LITERAL);

    resolve_statement(resolver, statement);

    // TODO(timo): assert that the value is null before resolving
    // Even though for now it didn't seem to be possible
    assert(declaration->initializer->literal.value.integer == 42);
    assert(resolver->global->symbols->length == 1);

    symbol = resolver->global->symbols->items[0];

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "foo") == 0);
    assert(symbol->type->kind == TYPE_INTEGER);
    // TODO(timo): How about value? Do we add the value to the symbol itself?
    // I think that we don't really need the ACTUAL types of these things anywhere
    // except when doing constant folding and of course it helps interpreter. But
    // therefore I think we don't need the values for the symbols.

    declaration_free(declaration);
    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);


    // ---- bool
    lexer_init(lexer, "_bar: bool = false;");
    lex(lexer);

    parser_init(parser, lexer->tokens);
    statement = parse_statement(parser);

    resolver_init(resolver);
    
    declaration = statement->declaration;
    assert(declaration->kind == DECLARATION_VARIABLE);
    assert(declaration->specifier == TYPE_SPECIFIER_BOOL);
    assert(declaration->initializer->literal.literal->kind == TOKEN_BOOLEAN_LITERAL);

    resolve_statement(resolver, statement);
    
    // TODO(timo): assert that the value is null before resolving
    // Even though for now it didn't seem to be possible
    assert(declaration->initializer->literal.value.boolean == false);
    assert(resolver->global->symbols->length == 1);

    symbol = resolver->global->symbols->items[0];

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "_bar") == 0);
    assert(symbol->type->kind == TYPE_BOOLEAN);

    declaration_free(declaration);
    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}

    
void test_resolve_multiple_global_variable_declarations(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tMultiple global variable declarations...");
    not_error = true;

    const char* source = "foo: int = 42;"
                         "_bar: bool = false;"
                         "FOOBAR: int = 0;";

    AST_Declaration* declaration;
    Symbol* symbol;

    lexer_init(lexer, source); 
    lex(lexer);

    parser_init(parser, lexer->tokens);
    parse(parser);

    array* declarations = parser->declarations;

    assert(declarations->length == 3);

    resolver_init(resolver);
    resolve(resolver, declarations);

    assert(resolver->global->symbols->length == 3);

    // ---- symbol 1
    declaration = declarations->items[0];
    symbol = resolver->global->symbols->items[0];

    assert(declaration->initializer->literal.literal->kind == TOKEN_INTEGER_LITERAL);
    assert(declaration->initializer->literal.value.integer == 42);

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "foo") == 0);
    assert(symbol->type->kind == TYPE_INTEGER);

    // ---- symbol 2
    declaration = declarations->items[1];
    symbol = resolver->global->symbols->items[1];

    assert(declaration->initializer->literal.literal->kind == TOKEN_BOOLEAN_LITERAL);
    assert(declaration->initializer->literal.value.boolean == false);

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "_bar") == 0);
    assert(symbol->type->kind == TYPE_BOOLEAN);

    // ---- symbol 2
    declaration = declarations->items[2];
    symbol = resolver->global->symbols->items[2];

    assert(declaration->initializer->literal.literal->kind == TOKEN_INTEGER_LITERAL);
    assert(declaration->initializer->literal.value.integer == 0);

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "FOOBAR") == 0);
    assert(symbol->type->kind == TYPE_INTEGER);
    
    // ---- teardown
    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolve_variable_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tVariable expression...");
    not_error = true;

    const char* source = "{\n    foo: int = 42;\n    foo;\n}";

    AST_Statement* statement;
    // Symbol* symbol;

    lexer_init(lexer, source); 
    lex(lexer);

    parser_init(parser, lexer->tokens);
    statement = parse_statement(parser);
    array* statements = statement->block.statements;

    assert(statement->kind == STATEMENT_BLOCK);
    assert(statements->length == 2);
    
    resolver_init(resolver);

    resolve_statement(resolver, statements->items[0]);

    assert(resolver->global->symbols->length == 1);
    
    statement = statements->items[1];
    assert(statement->kind == STATEMENT_EXPRESSION);
    // Type* type = resolve_expression(resolver, (AST_Statement*)(statements->items[1])->expression);
    Type* type = resolve_expression(resolver, statement->expression);
    
    assert(type->kind == TYPE_INTEGER);

    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolve_assignment_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    printf("\tAssignment expression...");
    not_error = true;

    const char* source = "{\n    foo: int = 42;\n    foo := 7;\n}";

    AST_Statement* statement;
    Symbol* symbol;

    lexer_init(lexer, source); 
    lex(lexer);

    parser_init(parser, lexer->tokens);
    statement = parse_statement(parser);
    array* statements = statement->block.statements;

    assert(statement->kind == STATEMENT_BLOCK);
    assert(statements->length == 2);

    resolver_init(resolver);

    statement = statements->items[0];
    resolve_statement(resolver, statement);

    assert(resolver->global->symbols->length == 1);

    statement = statements->items[1];
    assert(statement->kind == STATEMENT_EXPRESSION);
    assert(statement->expression->kind == EXPRESSION_ASSIGNMENT);

    Type* type = resolve_expression(resolver, statement->expression);
    assert(type->kind == TYPE_INTEGER);

    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_resolve_index_expression()
{
    printf("\tIndex expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Expression* expression;
    AST_Declaration* declaration;
    
    // TODO(timo): In our case the array subscription needs more context to
    // be used, so therefore the testing needs all that context. We need to
    // handle the resolving of the function expression first
    const char* source = "main: int = (argc: int, argv: [int]) => {\n"
                         "    return 0;\n"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    resolver_init(&resolver);
    resolve_declaration(&resolver, declaration);
    
    lexer_free(&lexer);
    parser_free(&parser);

    lexer_init(&lexer, "argv[0]"); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    Type* type = resolve_expression(&resolver, expression);

    assert(type->kind == TYPE_INTEGER);
    
    declaration_free(declaration);
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_resolve_function_expression()
{
    printf("\tFunction expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Expression* expression;
    Type* type;
    char* source;

    // with parameters and return
    source = "(argc: int, argv: [int]) => {\n"
             "    return 0;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert(expression->kind == EXPRESSION_FUNCTION);
    assert(expression->function.arity == 2);

    resolver_init(&resolver);
    type = resolve_expression(&resolver, expression);
    
    assert(resolver.global->symbols->length == 2);
    assert(resolver.context.return_type->kind == TYPE_INTEGER);
    assert(type->kind == TYPE_FUNCTION);
    assert(type->function.return_type->kind == TYPE_INTEGER);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // without parameters and without return
    source = "() => {\n"
             "    foo: bool = true;\n"
             "    return 0;"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert(expression->kind == EXPRESSION_FUNCTION);
    assert(expression->function.arity == 0);

    resolver_init(&resolver);
    type = resolve_expression(&resolver, expression);
    
    assert(resolver.global->symbols->length == 1);
    assert(resolver.context.return_type->kind == TYPE_INTEGER);
    assert(type->kind == TYPE_FUNCTION);
    assert(type->function.return_type->kind == TYPE_INTEGER);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_resolve_call_expression()
{
    printf("\tCall expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Expression* expression;
    AST_Declaration* declaration;
    char* source;

    // with arguments 
    source = "greater: bool = (x: int, y: int) => {\n"
             "    return x > y;\n"
             "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    resolver_init(&resolver);
    resolve_declaration(&resolver, declaration);
    
    lexer_free(&lexer);
    parser_free(&parser);

    lexer_init(&lexer, "greater(1, 0)");
    lex(&lexer);
    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert(expression->kind == EXPRESSION_CALL);

    Type* type = resolve_expression(&resolver, expression);

    assert(type->kind == TYPE_BOOLEAN);

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): no arguments 
    /*
    lexer_init(&lexer, "foo()");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    */

    // TODO(timo): assigning a value from call expression

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


/*
void test_resolve_order_independent_global_variable_declarations(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    // TODO(timo): Now there is the problem that we dont know the value of the
    // assigned declaration e.g. in this case even though the declaration and
    // assignments are correct, we don't have the value 42 in the foo.
    // In simple case like this, this might be fine
    const char* source = "foo: int = bar;\n"
                         "bar: int = 42;\n"
                         "main: int = (argc: int, argv: int) => {\n"
                         "    foo;\n"
                         "\n"
                         "    return 0;\n"
                         "};";

    lexer_init(lexer, source);
    lex(lexer);

    parser_init(parser, lexer->tokens);
    parse(parser);

    array* declarations = parser->declarations;

    assert(declarations->length == 3);

    resolver_init(resolver);

    resolve(resolver, declarations);

    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);
}
*/


static void test_resolve_function_declaration()
{
    printf("\tFunction declaration...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Expression* return_value;

    const char* source = "main: int = (argc: int, argv: int) => {\n"
                         "    return 0;\n"
                         "};";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);
    parse(&parser);

    assert(parser.declarations->length == 1);

    resolver_init(&resolver);
    resolve(&resolver, parser.declarations);

    assert(resolver.context.return_type->kind == TYPE_INTEGER);

    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_resolve_if_statement()
{
    printf("\tIf statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Statement* statement;
    
    // If with then
    lexer_init(&lexer, "if 0 < 1 then { }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_IF);

    resolver_init(&resolver);

    Type* condition_type = resolve_expression(&resolver, statement->_if.condition);
    assert(condition_type->kind == TYPE_BOOLEAN);

    resolve_statement(&resolver, statement);
    
    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): If with then and else
    // TODO(timo): If with else ifs
    // TODO(timo): multiple ifs with one else at end

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_resolve_while_statement()
{
    printf("\tWhile statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Statement* statement;
    
    lexer_init(&lexer, "while 0 < 1 do { }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_WHILE);

    resolver_init(&resolver);

    Type* condition_type = resolve_expression(&resolver, statement->_while.condition);
    assert(condition_type->kind == TYPE_BOOLEAN);

    resolve_statement(&resolver, statement);
    
    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_resolve_return_statement()
{
    printf("\tReturn statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    AST_Expression* return_value;

    const char* source = "return 0;";

    lexer_init(&lexer, source);
    lex(&lexer);
    
    parser_init(&parser, lexer.tokens);

    AST_Statement* statement = parse_statement(&parser);

    resolver_init(&resolver);

    Type* return_type = resolve_expression(&resolver, statement->_return.value);

    assert(return_type->kind == TYPE_INTEGER);
    
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_resolver()
{
    printf("Running resolver tests...\n");

    Lexer lexer;
    Parser parser;
    Resolver resolver;

    test_resolve_literal_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving literal expressions

    test_resolve_unary_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving unary expressions

    test_resolve_binary_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving binary expressions

    test_resolve_variable_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving variable expression

    test_resolve_assignment_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving assignment expressions

    test_resolve_index_expression();

    // TODO(timo): Diagnose errors while resolving index expression
    //      - accessed variable is not array type (LATER)
    //      - accessed variable is not argv
    //      - type of the index itself is not integer
    //      - index value cannot be < 0 or array_length - 1 (argc)
    
    test_resolve_function_expression();
    
    // TODO(timo): Diagnose errors while resolving function expression
    // TODO(timo): Diagnose invalid type of the return value

    test_resolve_call_expression();

    // TODO(timo): Diagnose errors while resolving call expression
    //      - invalid number of arguments
    //      - invalid type(s) of the argument(s)

    // ----


    // test_resolve_type_specifier();


    // ----
    

    // TODO(timo): We can't use the direct declaration resolving, unless we
    // create the undeclared symbols in the resolve declaration function itself
    test_resolve_variable_declaration(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving variable declarations

    test_resolve_function_declaration();
    
    test_resolve_multiple_global_variable_declarations(&lexer, &parser, &resolver);

    // TODO(timo): Resolve order independent global variable declarations
    // TODO(timo): Lets just forget this functionality for now and maybe add it later.
    // It is more important to get forward at this point
    // test_resolve_order_independent_global_variable_declarations(&lexer, &parser, &resolver);

    // TODO(timo); Diagnose errors while resolving order independent global variable declarations
    

    // ---


    // test_resolve_expression_statement()

    test_resolve_declaration_statement_variable(&lexer, &parser, &resolver);

    test_resolve_if_statement();
    
    test_resolve_while_statement();

    test_resolve_return_statement();

    // TODO(timo): Diganose error while resolving return statement (return value is missing)
    
    
    // TODO(timo): Diagnose no break statement outside loops
    // TODO(timo): Diagnose no continue statement outside loops
    // TODO(timo): Diagnose no return statement outside functions
    // TODO(timo): You can only use declarations at the top level so no
    // statements or expressions are allowed at top level.
    // TODO(timo): YORO - you only return once
    // TODO(timo): Functions are allowed only at top level
}
