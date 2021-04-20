#include "asserts.h"
#include "tests.h"
#include "../src/t.h"


static void test_generate_literal_expression(Test_Runner* runner)
{
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

    ir_generator_init(&generator, resolver.global);
    char* result = ir_generate_expression(&generator, expression);
    
    assert_base(runner, generator.instructions->length == 1,
        "Invalid number of instructions: %d, expected 1", generator.instructions->length);
    assert_base(runner, strcmp(result, "_t0") == 0,
        "Invalid result '%s', expected '_t0'", result);

    Instruction* instruction = generator.instructions->items[0];

    assert_instruction(runner, instruction, OP_COPY);

    // dump_instructions(generator.instructions);

    // TODO(timo): boolean literals

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_unary_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    // minus
    lexer_init(&lexer, "-42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 2,
        "Invalid number of instructions: %d, expected 2", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_MINUS);

    dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // not
    lexer_init(&lexer, "not false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 2,
        "Invalid number of instructions: %d, expected 2", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_NOT);

    dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_binary_expression_arithmetic(Test_Runner* runner)
{
    const char* tests[] =
    {
        "1 + 2",
        "3 - 4",
        "5 * 6",
        "7 / 8"
    };

    Operation results[][3] = 
    {
        {OP_COPY, OP_COPY, OP_ADD},
        {OP_COPY, OP_COPY, OP_SUB},
        {OP_COPY, OP_COPY, OP_MUL},
        {OP_COPY, OP_COPY, OP_DIV}
    };

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        expression = parse_expression(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);
        
        ir_generator_init(&generator, resolver.global);
        ir_generate_expression(&generator, expression);

        assert_base(runner, generator.instructions->length == 3,
            "Invalid number of instructions: %d, expected 3", generator.instructions->length);

        for (int j = 0; j < generator.instructions->length; j++)
            assert_instruction(runner, generator.instructions->items[j], results[i][j]);

        // dump_instructions(generator.instructions);
        
        expression_free(expression);
        ir_generator_free(&generator);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_generate_binary_expression_comparison(Test_Runner* runner)
{
    const char* tests[] =
    {
        "1 == 0",
        "1 != 0",
        "1 < 0",
        "1 <= 0",
        "1 > 0",
        "1 >= 0"
    };

    Operation results[][3] = 
    {
        {OP_COPY, OP_COPY, OP_EQ},
        {OP_COPY, OP_COPY, OP_NEQ},
        {OP_COPY, OP_COPY, OP_LT},
        {OP_COPY, OP_COPY, OP_LTE},
        {OP_COPY, OP_COPY, OP_GT},
        {OP_COPY, OP_COPY, OP_GTE}
    };

    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    for (int i = 0; i < sizeof (tests) / sizeof (*tests); i++)
    {
        lexer_init(&lexer, tests[i]);
        lex(&lexer);

        parser_init(&parser, lexer.tokens);
        expression = parse_expression(&parser);
        
        type_table = type_table_init();
        resolver_init(&resolver, type_table);
        resolve_expression(&resolver, expression);
        
        ir_generator_init(&generator, resolver.global);
        ir_generate_expression(&generator, expression);

        assert_base(runner, generator.instructions->length == 3,
            "Invalid number of instructions: %d, expected 3", generator.instructions->length);

        for (int j = 0; j < generator.instructions->length; j++)
            assert_instruction(runner, generator.instructions->items[j], results[i][j]);

        // dump_instructions(generator.instructions);
        
        expression_free(expression);
        ir_generator_free(&generator);
        resolver_free(&resolver);
        type_table_free(type_table);
        parser_free(&parser);
        lexer_free(&lexer);
    }
}


static void test_generate_variable_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n"
             "    foo: int = 0;\n"
             "    foo;\n"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);

    // NOTE(timo): We cannot be at global scope
    resolver.global->name = "local";

    ir_generator_init(&generator, resolver.global);
    
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 1,
        "Invalid number of instructions: %d, expected 1", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);

    // dump_instructions(generator.instructions);
    
    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_assignment_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n    "
             "    foo: int = 0;\n"
             "    foo := 42;\n"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    // NOTE(timo): We cannot be at global scope
    resolver.global->name = "local";
    
    ir_generator_init(&generator, resolver.global);
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 2,
        "Invalid number of instructions: %d, expected 2", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);

    // dump_instructions(generator.instructions);
    
    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_assignment_expression_complex(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Statement* statement;
    char* source;
    
    source = "{\n    "
             "    foo: int = 0;\n"
             "    foo := 1 * -2 + 3 * -4;\n"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    // NOTE(timo): We cannot be at global scope
    resolver.global->name = "local";

    ir_generator_init(&generator, resolver.global);
    expression = ((AST_Statement*)statement->block.statements->items[1])->expression;
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 10,
        "Invalid number of instructions: %d, expected 10", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_MINUS);
    assert_instruction(runner, generator.instructions->items[3], OP_MUL);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_MINUS);
    assert_instruction(runner, generator.instructions->items[7], OP_MUL);
    assert_instruction(runner, generator.instructions->items[8], OP_ADD);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);

    // dump_instructions(generator.instructions);
    
    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    source = "{\n"
             "    a: int = 0;\n"
             "    b: int = 0;\n"
             "    c: int = 0;\n"
             "    a := b * -c + b * -c;\n"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    // NOTE(timo): We cannot be at global scope
    resolver.global->name = "local";

    ir_generator_init(&generator, resolver.global);
    expression = ((AST_Statement*)statement->block.statements->items[3])->expression;
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 10,
        "Invalid number of instructions: %d, expected 10", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_MINUS);
    assert_instruction(runner, generator.instructions->items[3], OP_MUL);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_MINUS);
    assert_instruction(runner, generator.instructions->items[7], OP_MUL);
    assert_instruction(runner, generator.instructions->items[8], OP_ADD);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);

    // dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_index_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    char* source;

    source = "main: int = (argc: int, argv: [int]) => { return argv[0]; };";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    assert(parser.declarations->length == 1);
    AST_Declaration* declaration = parser.declarations->items[0];
    AST_Statement* body = declaration->initializer->function.body;
    AST_Statement* _return = body->block.statements->items[0];

    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, _return->_return.value);

    // dump_instructions(generator.instructions);

    assert_base(runner, generator.instructions->length == 5,
        "Invalid number of instructions: %d, expected 5", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_MUL);
    assert_instruction(runner, generator.instructions->items[3], OP_ADD);
    assert_instruction(runner, generator.instructions->items[4], OP_DEREFERENCE);
    
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_function_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;

    // With parameters
    lexer_init(&lexer, "(a: int, b: int) => { b := a + b; return b; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    
    // Resolver will enter a scope, and since we don't have a name for the
    // function expression, we will have to set it here manually.
    resolver.context.current_function = "some_func";
    // We need to take this type and free it, since it won't be freed otherwise
    Type* type = resolve_expression(&resolver, expression);
    
    ir_generator_init(&generator, resolver.global);

    // We will also have to set the scope to the function scope manually
    generator.local = type->function.scope;

    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 8,
        "Invalid number of instructions: %d, expected 8", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_FUNCTION_BEGIN);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_ADD);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_RETURN);
    assert_instruction(runner, generator.instructions->items[7], OP_FUNCTION_END);

    // dump_instructions(generator.instructions);
    
    type_free(type);
    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): Without parameters
}


static void test_generate_call_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Expression* expression;
    AST_Declaration* declaration;
    char* source;

    // TODO(timo): zero arguments
    
    // single argument
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

    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 4,
        "Invalid number of instructions: %d, expected 4", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_PARAM_PUSH);
    assert_instruction(runner, generator.instructions->items[2], OP_CALL);
    assert_instruction(runner, generator.instructions->items[3], OP_PARAM_POP);

    // dump_instructions(generator.instructions);
    
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // two arguments
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

    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 7,
        "Invalid number of instructions: %d, expected 7", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_PARAM_PUSH);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_PARAM_PUSH);
    assert_instruction(runner, generator.instructions->items[4], OP_CALL);
    assert_instruction(runner, generator.instructions->items[5], OP_PARAM_POP);
    assert_instruction(runner, generator.instructions->items[6], OP_PARAM_POP);

    // dump_instructions(generator.instructions);
    
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_if_statement_1(Test_Runner* runner)
{
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
             "    if a < b then {\n"
             "        a := c;\n"
             "    }\n"
             "    c := b;\n"
             "}";
    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    // This scope is needed to have some kind of local scope
    // Scope* local = scope_init(resolver.global, "local");
    // resolver.local = local;
    resolver.global->name = "local";
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_statement(&generator, statement);

    assert_base(runner, generator.instructions->length == 15,
        "Invalid number of instructions: %d, expected 15", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_COPY);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_COPY);
    assert_instruction(runner, generator.instructions->items[7], OP_COPY);
    assert_instruction(runner, generator.instructions->items[8], OP_LT);
    assert_instruction(runner, generator.instructions->items[9], OP_GOTO_IF_FALSE);
    assert_instruction(runner, generator.instructions->items[10], OP_COPY);
    assert_instruction(runner, generator.instructions->items[11], OP_COPY);
    assert_instruction(runner, generator.instructions->items[12], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[13], OP_COPY);
    assert_instruction(runner, generator.instructions->items[14], OP_COPY);

    // dump_instructions(generator.instructions);

    // scope_free(local);
    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_if_statement_2(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Statement* statement;
    char* source;
    
    // if-then-else
    source = "if 0 < 1 then { 1 + 2; } else { 5 * 6; }";
    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_statement(&generator, statement);

    assert_base(runner, generator.instructions->length == 13,
        "Invalid number of instructions: %d, expected 13", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_LT);
    assert_instruction(runner, generator.instructions->items[3], OP_GOTO_IF_FALSE);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_ADD);
    assert_instruction(runner, generator.instructions->items[7], OP_GOTO);
    assert_instruction(runner, generator.instructions->items[8], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);
    assert_instruction(runner, generator.instructions->items[10], OP_COPY);
    assert_instruction(runner, generator.instructions->items[11], OP_MUL);
    assert_instruction(runner, generator.instructions->items[12], OP_LABEL);

    // dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_if_statement_3(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Statement* statement;
    char* source;
    
    // if-then-else if-then-else
    source = "if 1 < 0 then {\n"
             "    1 + 2;\n"
             "} else if 1 == 0 then {\n"
             "    3 - 4;\n"
             "} else {\n"
             "    5 * 6;\n"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_statement(&generator, statement);

    // TODO(timo): asserts
    runner->error = true;

    // dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_if_statement_4(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Statement* statement;
    char* source;
    
    // if-then-else if-then-else
    source = "if 1 < 0 then {\n"
             "    1 + 2;\n"
             "} else if 1 == 0 then {\n"
             "    3 - 4;\n"
             "} else if 1 != 0 then {\n"
             "    5 * 6;\n"
             "} else {\n"
             "    7 / 8;\n"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_statement(&resolver, statement);
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_statement(&generator, statement);

    // TODO(timo): asserts
    runner->error = true;

    // dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_while_statement(Test_Runner* runner)
{
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
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_statement(&generator, statement);

    assert_base(runner, generator.instructions->length == 13,
        "Invalid number of instructions: %d, expected 13", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_LT);
    assert_instruction(runner, generator.instructions->items[4], OP_GOTO_IF_FALSE);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_COPY);
    assert_instruction(runner, generator.instructions->items[7], OP_ADD);
    assert_instruction(runner, generator.instructions->items[8], OP_COPY);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);
    assert_instruction(runner, generator.instructions->items[10], OP_SUB);
    assert_instruction(runner, generator.instructions->items[11], OP_GOTO);
    assert_instruction(runner, generator.instructions->items[12], OP_LABEL);

    // dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_return_statement(Test_Runner* runner)
{
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
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_statement(&generator, statement);

    assert_base(runner, generator.instructions->length == 2,
        "Invalid number of instructions: %d, expected 2", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_RETURN);

    // dump_instructions(generator.instructions);

    statement_free(statement);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_variable_declaration_global(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Declaration* declaration;
    char* source;
    
    source = "foo: int = 42;\n\n"
             "bar: int = 7;\n\n"
             "max: int = () => {\n"
             "    result: int = 0;\n"
             "    if foo > bar then {\n"
             "        result := foo;\n"
             "    } else {\n"
             "        result := bar;\n"
             "    }\n"
             "    return result;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    ir_generator_init(&generator, resolver.global);
    ir_generate(&generator, parser.declarations);

    assert_base(runner, generator.instructions->length == 18,
        "Invalid number of instructions: %d, expected 18", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[1], OP_FUNCTION_BEGIN);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_COPY);
    assert_instruction(runner, generator.instructions->items[4], OP_LOAD_GLOBAL);
    assert_instruction(runner, generator.instructions->items[5], OP_LOAD_GLOBAL);
    assert_instruction(runner, generator.instructions->items[6], OP_GT);
    assert_instruction(runner, generator.instructions->items[7], OP_GOTO_IF_FALSE);
    assert_instruction(runner, generator.instructions->items[8], OP_LOAD_GLOBAL);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);
    assert_instruction(runner, generator.instructions->items[10], OP_GOTO);
    assert_instruction(runner, generator.instructions->items[11], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[12], OP_LOAD_GLOBAL);
    assert_instruction(runner, generator.instructions->items[13], OP_COPY);
    assert_instruction(runner, generator.instructions->items[14], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[15], OP_COPY);
    assert_instruction(runner, generator.instructions->items[16], OP_RETURN);
    assert_instruction(runner, generator.instructions->items[17], OP_FUNCTION_END);
    
    // dump_instructions(generator.instructions);

    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_function_declaration(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    AST_Declaration* declaration;
    char* source;
    
    source = "max: int = (x: int, y: int) => {\n"
             "    result: int = 0;\n"
             "    if x > y then {\n"
             "        result := x;\n"
             "    } else {\n"
             "        result := y;\n"
             "    }\n"
             "    return result;\n"
             "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_declaration(&resolver, declaration);

    ir_generator_init(&generator, resolver.global);
    ir_generate_declaration(&generator, declaration);

    assert_base(runner, generator.instructions->length == 18,
        "Invalid number of instructions: %d, expected 18", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[1], OP_FUNCTION_BEGIN);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_COPY);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_GT);
    assert_instruction(runner, generator.instructions->items[7], OP_GOTO_IF_FALSE);
    assert_instruction(runner, generator.instructions->items[8], OP_COPY);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);
    assert_instruction(runner, generator.instructions->items[10], OP_GOTO);
    assert_instruction(runner, generator.instructions->items[11], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[12], OP_COPY);
    assert_instruction(runner, generator.instructions->items[13], OP_COPY);
    assert_instruction(runner, generator.instructions->items[14], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[15], OP_COPY);
    assert_instruction(runner, generator.instructions->items[16], OP_RETURN);
    assert_instruction(runner, generator.instructions->items[17], OP_FUNCTION_END);
    
    // dump_instructions(generator.instructions);

    declaration_free(declaration);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_arithmetics(Test_Runner* runner)
{
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
    
    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 5,
        "Invalid number of instructions: %d, expected 5", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_MUL);
    assert_instruction(runner, generator.instructions->items[4], OP_ADD);

    // dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 * 2 + 3 * 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 7,
        "Invalid number of instructions: %d, expected 7", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_MUL);
    assert_instruction(runner, generator.instructions->items[3], OP_COPY);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_MUL);
    assert_instruction(runner, generator.instructions->items[6], OP_ADD);

    // dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 * -2 + 3 * -4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve_expression(&resolver, expression);

    ir_generator_init(&generator, resolver.global);
    ir_generate_expression(&generator, expression);

    assert_base(runner, generator.instructions->length == 9,
        "Invalid number of instructions: %d, expected 9", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_COPY);
    assert_instruction(runner, generator.instructions->items[1], OP_COPY);
    assert_instruction(runner, generator.instructions->items[2], OP_MINUS);
    assert_instruction(runner, generator.instructions->items[3], OP_MUL);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_MINUS);
    assert_instruction(runner, generator.instructions->items[7], OP_MUL);
    assert_instruction(runner, generator.instructions->items[8], OP_ADD);

    // dump_instructions(generator.instructions);

    expression_free(expression);
    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


static void test_generate_small_program(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator generator;
    
    const char* source = "max: int = (x: int, y: int) => {\n"
                         "result: int = 0;\n"
                         "    if x > y then {\n"
                         "        result := x;\n"
                         "    } else {\n"
                         "        result := y;\n"
                         "    }\n"
                         "    return result;\n"
                         "};\n"
                         "\n"
                         "main: int = (argc: int, argv: [int]) => {\n"
                         "    return max(0, 1);\n"
                         "};";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    ir_generator_init(&generator, resolver.global);
    ir_generate(&generator, parser.declarations);

    assert_base(runner, generator.instructions->length == 29,
        "Invalid number of instructions: %d, expected 29", generator.instructions->length);
    assert_instruction(runner, generator.instructions->items[0], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[1], OP_FUNCTION_BEGIN);
    assert_instruction(runner, generator.instructions->items[2], OP_COPY);
    assert_instruction(runner, generator.instructions->items[3], OP_COPY);
    assert_instruction(runner, generator.instructions->items[4], OP_COPY);
    assert_instruction(runner, generator.instructions->items[5], OP_COPY);
    assert_instruction(runner, generator.instructions->items[6], OP_GT);
    assert_instruction(runner, generator.instructions->items[7], OP_GOTO_IF_FALSE);
    assert_instruction(runner, generator.instructions->items[8], OP_COPY);
    assert_instruction(runner, generator.instructions->items[9], OP_COPY);
    assert_instruction(runner, generator.instructions->items[10], OP_GOTO);
    assert_instruction(runner, generator.instructions->items[11], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[12], OP_COPY);
    assert_instruction(runner, generator.instructions->items[13], OP_COPY);
    assert_instruction(runner, generator.instructions->items[14], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[15], OP_COPY);
    assert_instruction(runner, generator.instructions->items[16], OP_RETURN);
    assert_instruction(runner, generator.instructions->items[17], OP_FUNCTION_END);
    assert_instruction(runner, generator.instructions->items[18], OP_LABEL);
    assert_instruction(runner, generator.instructions->items[19], OP_FUNCTION_BEGIN);
    assert_instruction(runner, generator.instructions->items[20], OP_COPY);
    assert_instruction(runner, generator.instructions->items[21], OP_PARAM_PUSH);
    assert_instruction(runner, generator.instructions->items[22], OP_COPY);
    assert_instruction(runner, generator.instructions->items[23], OP_PARAM_PUSH);
    assert_instruction(runner, generator.instructions->items[24], OP_CALL);
    assert_instruction(runner, generator.instructions->items[25], OP_PARAM_POP);
    assert_instruction(runner, generator.instructions->items[26], OP_PARAM_POP);
    assert_instruction(runner, generator.instructions->items[27], OP_RETURN);
    assert_instruction(runner, generator.instructions->items[28], OP_FUNCTION_END);
    
    // dump_instructions(generator.instructions);

    ir_generator_free(&generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
}


Test_Set* ir_generator_test_set()
{
    Test_Set* set = test_set("IR Generator");

    // Expressions
    array_push(set->tests, test_case("Literal expression", test_generate_literal_expression));
    array_push(set->tests, test_case("Unary expression", test_generate_unary_expression));
    array_push(set->tests, test_case("Binary expression (arithmetic)", test_generate_binary_expression_arithmetic));
    array_push(set->tests, test_case("Binary expression (comparison)", test_generate_binary_expression_comparison));
    array_push(set->tests, test_case("Variable expression", test_generate_variable_expression));
    array_push(set->tests, test_case("Assignment expression", test_generate_assignment_expression));
    array_push(set->tests, test_case("Assignment expression (complex)", test_generate_assignment_expression_complex));
    array_push(set->tests, test_case("Function expression", test_generate_function_expression));
    array_push(set->tests, test_case("Call expression", test_generate_call_expression));
    array_push(set->tests, test_case("Index expression", test_generate_index_expression));

    // Statements
    array_push(set->tests, test_case("If statement (if then)", test_generate_if_statement_1));
    array_push(set->tests, test_case("If statement (if then - else)", test_generate_if_statement_2));
    array_push(set->tests, test_case("If statement (if then - else if then - else)", test_generate_if_statement_3));
    array_push(set->tests, test_case("If statement (arbitrary number of else if's)", test_generate_if_statement_4));
    array_push(set->tests, test_case("While statement", test_generate_while_statement));
    // TODO(timo): array_push(set->tests, test_case("While statement with a break", test_generate_while_statement_with_break));
    // TODO(timo): array_push(set->tests, test_case("Nested while statement", test_generate_nested_while_statement));
    // TODO(timo): array_push(set->tests, test_case("Nested while statement witch break(s)", test_generate_nested_while_statement_with_break));
    array_push(set->tests, test_case("Return statement", test_generate_return_statement));
    // TODO(timo): array_push(set->tests, test_case("Break statement", test_generate_break_statement));

    // Declarations
    array_push(set->tests, test_case("Variable declaration (global)", test_generate_variable_declaration_global));
    array_push(set->tests, test_case("Function declaration", test_generate_function_declaration));

    // MISC
    array_push(set->tests, test_case("Generate MISC arithmetics", test_generate_arithmetics));
    array_push(set->tests, test_case("Small program", test_generate_small_program));

    set->length = set->tests->length;

    return set;
}
