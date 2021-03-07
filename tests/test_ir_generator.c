#include "../src/t.h"
#include "tests.h"


static bool not_error = true;


static void test_generate_literal_expression()
{
    printf("\tLiteral expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;

    // integer literal
    lexer_init(&lexer, "42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    AST_Expression* expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    char* result = ir_generate_expression(&generator, expression);
    
    // assert(strcmp(result, "42") == 0);
    //dump_instructions(generator.instructions);

    // TODO(timo): boolean literals

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
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
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    lexer_init(&lexer, "-42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // ----
    lexer_init(&lexer, "not false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);
    
    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
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
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    // binary arithmetic +
    lexer_init(&lexer, "1 + 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);
    
    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // binary arithmetic -
    lexer_init(&lexer, "3 - 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // binary arithmetic *
    lexer_init(&lexer, "5 * 6");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    expression_free(expression);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // binary arithmetic /
    lexer_init(&lexer, "7 / 8");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
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
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n    foo: int = 0;\n    foo;\n}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);

    ir_generator_init(&generator);
    
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    char* result = ir_generate_expression(&generator, expression);
    // assert(strcmp(result, "foo") == 0);
    //dump_instructions(generator.instructions);
    
    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
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
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n    foo: int = 0;\n    foo := 1 * -2 + 3 * -4;\n}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator);
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("----------\n");

    // ----
    source = "{\n    a: int = 0;\n    b: int = 0;\n    c: int = 0;\n    a := b * -c + b * -c;\n}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator);
    expression = ((AST_Statement*)statement->block.statements->items[3])->expression;
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_index_expression()
{
    printf("\tIndex expression...");
    not_error = true;

    printf("\n\t\tNOT IMPLEMENTED");
    not_error = false;

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_function_expression()
{
    printf("\tFunction expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    lexer_init(&lexer, "(a: int, b: int) => { b := a + b; return b; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);
    
    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_call_expression()
{
    printf("\tCall expression...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Declaration* declaration;
    char* source;
    
    source = "cube: int = (x: int) => { return x * x * x; };\n"
             "main: int = (argc: int, argv: [int]) => { cube(42); };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    array* declarations = parser.declarations;
    declaration = (AST_Declaration*)declarations->items[1];

    array* statements = declaration->initializer->function.body->block.statements;
    expression = (AST_Expression*)((AST_Statement*)(statements->items[0]))->expression;

    assert(strcmp(expression->call.variable->identifier->lexeme, "cube") == 0);
    
    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("----------\n");

    source = "max: int = (x: int, y: int) => { if x > y then { return x; } else { return y; } };\n"
             "main: int = (argc: int, argv: [int]) => { max(0, 1); };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    declarations = parser.declarations;
    declaration = (AST_Declaration*)declarations->items[1];

    statements = declaration->initializer->function.body->block.statements;
    expression = (AST_Expression*)((AST_Statement*)(statements->items[0]))->expression;

    assert(strcmp(expression->call.variable->identifier->lexeme, "max") == 0);
    
    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);
    
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_arithmetics()
{
    printf("\tArithmetics...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    lexer_init(&lexer, "1 + 2 * 3");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);
    
    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // ----
    lexer_init(&lexer, "1 * 2 + 3 * 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("--------\n");

    // ----
    lexer_init(&lexer, "1 * -2 + 3 * -4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator);
    ir_generate_expression(&generator, expression);

    //dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_if_statement()
{
    printf("\tIf statement...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
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

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator);
    ir_generate_statement(&generator, statement);

    //dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("----------\n");

    // if-then-else
    source = "";
    lexer_init(&lexer, "if 0 < 1 then { 1 + 2; 3 - 4; } else {5 * 6; 7 / 8; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator);
    ir_generate_statement(&generator, statement);

    //dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // if-then-else if-then-else

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_while_statement()
{
    printf("\tWhile statement...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Statement* statement;

    lexer_init(&lexer, "while 0 < 1 do { 1 + 2; 3 - 4; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator);
    ir_generate_statement(&generator, statement);

    //dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_return_statement()
{
    printf("\tReturn statement...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Statement* statement;

    lexer_init(&lexer, "return 0;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    // NOTE(timo): This is needed to tell the resolver that 
    // our context is correct to actually run this test
    resolver.context.not_in_function = false;
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator);
    ir_generate_statement(&generator, statement);

    //dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_function_declaration()
{
    printf("\tFunction declaration...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Declaration* declaration;
    char* source;
    
    source = "max: int = (x: int, y: int) => { if x > y then { return x; } else { return y; } };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    ir_generator_init(&generator);
    ir_generate_declaration(&generator, declaration);
    
    //dump_instructions(generator.instructions);

    declaration_free(declaration);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_generate_small_program()
{
    printf("\tSmall program...");
    printf("\n");
    not_error = true;

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    
    const char* source = "max: int = (x: int, y: int) => { if x > y then { return x; } else { return y; } };\n"
                         "main: int = (argc: int, argv: [int]) => { return max(0, 1); };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    ir_generator_init(&generator);
    ir_generate(&generator, parser.declarations);
    
    //dump_instructions(generator.instructions);

    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
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
    test_generate_return_statement();
    
    test_generate_function_declaration();

    test_generate_small_program();
}
