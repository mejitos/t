#include "../src/t.h"
#include "tests.h"


static bool not_error = true;


static void test_generate_literal_expression()
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

    char* result = generate_expression(&generator, expression);
    
    // assert(strcmp(result, "42") == 0);

    // TODO(timo): boolean literals

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_unary_expression()
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

    generate_expression(&generator, expression);

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

    generate_expression(&generator, expression);

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}

static void test_generate_binary_expression()
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

    generate_expression(&generator, expression);
    
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

    generate_expression(&generator, expression);
    
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

    generate_expression(&generator, expression);
    
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

    generate_expression(&generator, expression);
    
    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_variable_expression()
{
    printf("\tVariable expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n    foo: int = 0;\n    foo;\n}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);

    statement = parse_statement(&parser);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    char* result = generate_expression(&generator, expression);

    assert(strcmp(result, "foo") == 0);
    
    // This expression will be free'd with the statement
    // expression_free(expression);
    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_assignment_expression()
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
    generate_expression(&generator, expression);
    
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
    generate_expression(&generator, expression);

    // This expression will be free'd with the statement
    // expression_free(expression);
    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_index_expression()
{
    printf("\tIndex expression...");
    not_error = true;

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_function_expression()
{
    printf("\tFunction expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;

    lexer_init(&lexer, "(a: int, b: int) => { b := a + b; return b; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    resolver_init(&resolver);
    resolve_expression(&resolver, expression);
    
    printf("\n");
    generate_expression(&generator, expression);
    
    // expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_call_expression()
{
    printf("\tCall expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;
    AST_Declaration* declaration;
    char* source;
    
    source = "cube: int = (x: int) => { return x * x * x; };\n"
             "main: int = (argc: int, argv: [int]) => { cube(42); };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    resolver_init(&resolver);
    resolve(&resolver, parser.declarations);
    
    array* declarations = parser.declarations;
    declaration = (AST_Declaration*)declarations->items[1];

    array* statements = declaration->initializer->function.body->block.statements;
    expression = (AST_Expression*)((AST_Statement*)(statements->items[0]))->expression;

    assert(strcmp(expression->call.variable->identifier->lexeme, "cube") == 0);
    
    printf("\n");
    generate_expression(&generator, expression);
    
    // declaration_free(declaration);
    // expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("----------\n");

    generator = (IR_Generator) { .label = 0, .temp = 0 };
    
    source = "max: int = (x: int, y: int) => { if x > y then { return x; } else { return y; } };\n"
             "main: int = (argc: int, argv: [int]) => { max(0, 1); };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    resolver_init(&resolver);
    resolve(&resolver, parser.declarations);
    
    declarations = parser.declarations;
    declaration = (AST_Declaration*)declarations->items[1];

    statements = declaration->initializer->function.body->block.statements;
    expression = (AST_Expression*)((AST_Statement*)(statements->items[0]))->expression;

    assert(strcmp(expression->call.variable->identifier->lexeme, "max") == 0);
    
    printf("\n");
    generate_expression(&generator, expression);
    
    // declaration_free(declaration);
    // expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_arithmetics()
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

    generate_expression(&generator, expression);

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

    generate_expression(&generator, expression);

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

    generate_expression(&generator, expression);

    expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_if_statement()
{
    printf("\tIf statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Statement* statement;
    char* source;
    
    // if-then
    source = "{\n"
             "    a: int = 0;\n"
             "    b: int = 0;\n"
             "    c: int = 0;\n"
             "\n"
             "    if a < b + c then {\n"
             "        a := a - c;\n"
             "    }\n"
             "    c := b * c;\n"
             "}";
    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    printf("\n");
    generate_statement(&generator, statement);

    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("----------\n");

    // if-then-else
    source = "";
    lexer_init(&lexer, "if 0 < 1 then { 1 + 2; 3 - 4; } else {5 * 6; 7 / 8; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    printf("\n");
    generate_statement(&generator, statement);

    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    // if-then-else if-then-else

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_while_statement()
{
    printf("\tWhile statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Statement* statement;

    lexer_init(&lexer, "while 0 < 1 do { 1 + 2; 3 - 4; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    resolver_init(&resolver);
    resolve_statement(&resolver, statement);
    
    printf("\n");
    generate_statement(&generator, statement);

    statement_free(statement);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_small_program()
{
    printf("\tSmall program...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Resolver resolver;
    IR_Generator generator = (IR_Generator) { .label = 0, .temp = 0 };
    AST_Expression* expression;
    AST_Declaration* declaration;
    char* source;
    
    /*
    source = "max: int = (x: int, y: int) => { if x > y then { return x; } else { return y; } };\n"
             "main: int = (argc: int, argv: [int]) => { result: int = max(0, 1); return result; };";
    */
    
    // TODO(timo): For some reason this segfaults
    source = "max: int = (x: int, y: int) => { if x > y then { return x; } else { return y; } };\n"
             "main: int = (argc: int, argv: [int]) => { return max(0, 1); };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    resolver_init(&resolver);
    resolve(&resolver, parser.declarations);
    
    printf("\n");
    for (int i = 0; i < parser.declarations->length; i++)
        generate_declaration(&generator, parser.declarations->items[i]);

    //generate(&generator, expression);
    
    // declaration_free(declaration);
    // expression_free(expression);
    resolver_free(&resolver);
    parser_free(&parser);
    lexer_free(&lexer);


    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_ir_generator()
{
    printf("Running IR generator tests...\n");

    test_generate_literal_expression();
    test_generate_unary_expression();
    test_generate_binary_expression();
    test_generate_variable_expression();
    test_generate_assignment_expression();
    test_generate_index_expression();
    test_generate_function_expression();
    test_generate_call_expression();

    test_generate_arithmetics();

    test_generate_if_statement();
    test_generate_while_statement();
    // test_generate_return_statement();
    
    // test_generate_function_declaration();

    test_generate_small_program();
}
