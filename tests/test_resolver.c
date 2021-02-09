#include "tests.h"
#include "../src/t.h"


static bool not_error = true;

void test_resolve_literal_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
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

    for (size_t i = 0; i < sizeof (tests) / sizeof (*tests); i++)
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
}


void test_resolve_unary_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
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
}


void test_resolve_binary_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
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
}


void test_resolve_variable_declaration(Lexer* lexer, Parser* parser, Resolver* resolver)
{
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
}


void test_resolve_declaration_statement_variable(Lexer* lexer, Parser* parser, Resolver* resolver)
{
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
}

    
void test_resolve_multiple_global_variable_declarations(Lexer* lexer, Parser* parser, Resolver* resolver)
{
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
    assert(symbol->state == STATE_RESOLVED);

    // ---- symbol 2
    declaration = declarations->items[1];
    symbol = resolver->global->symbols->items[1];

    assert(declaration->initializer->literal.literal->kind == TOKEN_BOOLEAN_LITERAL);
    assert(declaration->initializer->literal.value.boolean == false);

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "_bar") == 0);
    assert(symbol->type->kind == TYPE_BOOLEAN);
    assert(symbol->state == STATE_RESOLVED);

    // ---- symbol 2
    declaration = declarations->items[2];
    symbol = resolver->global->symbols->items[2];

    assert(declaration->initializer->literal.literal->kind == TOKEN_INTEGER_LITERAL);
    assert(declaration->initializer->literal.value.integer == 0);

    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(strcmp(symbol->identifier, "FOOBAR") == 0);
    assert(symbol->type->kind == TYPE_INTEGER);
    assert(symbol->state == STATE_RESOLVED);
    
    // ---- teardown
    /*
    declaration_free(declaration);
    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);
    */
}


void test_resolve_variable_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
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
}


void test_resolve_assignment_expression(Lexer* lexer, Parser* parser, Resolver* resolver)
{
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
}


void test_resolve_order_independent_global_variable_declarations(Lexer* lexer, Parser* parser, Resolver* resolver)
{
    const char* source = "foo: int = bar;"
                         "bar: int = 42;";

    lexer_init(lexer, source);
    lex(lexer);

    parser_init(parser, lexer->tokens);
    parse(parser);

    array* declarations = parser->declarations;

    assert(declarations->length == 2);

    resolver_init(resolver);

    resolve(resolver, declarations);

    resolver_free(resolver);
    parser_free(parser);
    lexer_free(lexer);
}


void test_resolver()
{
    printf("Running resolver tests...");

    Lexer lexer;
    Parser parser;
    Resolver resolver;

    test_resolve_literal_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving literal expressions

    test_resolve_unary_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving unary expressions

    test_resolve_binary_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving binary expressions

    // TODO(timo): You can only use declarations at the top level so no
    // statements or expressions are allowed at top level.
    
    // TODO(timo): We can't use the direct declaration resolving, unless we
    // create the undeclared symbols in the resolve declaration function itself
    // test_resolve_variable_declaration(&lexer, &parser, &resolver);
    test_resolve_declaration_statement_variable(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving variable declarations
    
    test_resolve_multiple_global_variable_declarations(&lexer, &parser, &resolver);

    test_resolve_variable_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving variable expression

    // TODO(timo): Variable assignment
    test_resolve_assignment_expression(&lexer, &parser, &resolver);

    // TODO(timo): Diagnose errors while resolving assignment expressions

    // TODO(timo): Resolve order independent global variable declarations
    test_resolve_order_independent_global_variable_declarations(&lexer, &parser, &resolver);

    // TODO(timo); Diagnose errors while resolving order independent global variable declarations

    if (not_error) printf("OK\n");
    else printf("\n");
}
