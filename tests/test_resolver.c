#include "tests.h"
#include "../src/t.h"


static bool not_error = true;

void test_resolve_literal_expression(Lexer* lexer, Parser* parser)
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

        assert(expression->kind == EXPRESSION_LITERAL);

        resolve_expression(expression);

        assert(expression->type->kind == results[i][0]);

        if (expression->type->kind == TYPE_INTEGER)
            assert(expression->literal.value.integer == results[i][1]);
        else if (expression->type->kind == TYPE_BOOLEAN)
            assert(expression->literal.value.boolean == results[i][1]);

        expression_free(expression);
        parser_free(parser);
        lexer_free(lexer);
    }
}


void test_resolve_unary_expression(Lexer* lexer, Parser* parser)
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

        assert(expression->kind == EXPRESSION_UNARY);
        assert(expression->unary.operand->kind == EXPRESSION_LITERAL);

        resolve_expression(expression);

        assert(expression->type->kind == results[i]);

        expression_free(expression);
        parser_free(parser);
        lexer_free(lexer);
    }
}


void test_resolve_binary_expression(Lexer* lexer, Parser* parser)
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

        assert(expression->kind == EXPRESSION_BINARY);
        assert(expression->binary.left->kind == EXPRESSION_LITERAL);
        assert(expression->binary.right->kind == EXPRESSION_LITERAL);

        resolve_expression(expression);

        assert(expression->type->kind == results[i]);

        expression_free(expression);
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


void test_resolver()
{
    printf("Running resolver tests...");

    Lexer lexer;
    Parser parser;
    Resolver resolver;

    test_resolve_literal_expression(&lexer, &parser);

    // TODO(timo): Diagnose errors while resolving literal expressions

    test_resolve_unary_expression(&lexer, &parser);

    // TODO(timo): Diagnose errors while resolving unary expressions

    test_resolve_binary_expression(&lexer, &parser);

    // TODO(timo): Diagnose errors while resolving binary expressions
    
    test_resolve_variable_declaration(&lexer, &parser, &resolver);

    // TODO(timo): Variable assignment

    if (not_error) printf("OK\n");
    else printf("\n");
}
