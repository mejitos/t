#include "asserts.h"
#include "tests.h"
#include "../src/t.h"
#include "../src/stringbuilder.h"


static bool not_error = true;


stringbuilder* expression_to_string(AST_Expression* expression, stringbuilder* sb)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            sb_append(sb, expression->literal->lexeme);
            break;
        case EXPRESSION_BINARY:
            sb_append(sb, "(");
            expression_to_string(expression->binary.left, sb);
            sb_append(sb, expression->binary._operator->lexeme);
            expression_to_string(expression->binary.right, sb);
            sb_append(sb, ")");
            break;
        case EXPRESSION_UNARY:
            sb_append(sb, "(");
            sb_append(sb, expression->unary._operator->lexeme);
            expression_to_string(expression->unary.operand, sb);
            sb_append(sb, ")");
            break;
        default:
            printf("Error in expression_to_string()\n");
            exit(1);
    }

    return sb;
}


static void test_literal_expression_integer(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "767");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->literal, TOKEN_INTEGER_LITERAL, "767");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}

   
static void test_literal_expression_boolean(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    // true
    lexer_init(&lexer, "true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->literal, TOKEN_BOOLEAN_LITERAL, "true");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // false
    lexer_init(&lexer, "false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->literal, TOKEN_BOOLEAN_LITERAL, "false");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_unary_expression_plus(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "+42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_UNARY);
    assert_token(runner, expression->unary._operator, TOKEN_PLUS, "+");
    assert_expression(runner, expression->unary.operand->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->unary.operand->literal, TOKEN_INTEGER_LITERAL, "42");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_unary_expression_minus(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "-42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_UNARY);
    assert_token(runner, expression->unary._operator, TOKEN_MINUS, "-");
    assert_expression(runner, expression->unary.operand->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->unary.operand->literal, TOKEN_INTEGER_LITERAL, "42");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_plus(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "1 + 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_PLUS, "+");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_minus(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "3 - 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
        
    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_MINUS, "-");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "3");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "4");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_multiply(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "5 * 6");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_MULTIPLY, "*");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "5");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "6");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_division(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "7 / 8");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_DIVIDE, "/");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "7");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "8");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_equal(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    lexer_init(&lexer, "1 == 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_IS_EQUAL, "==");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_not_equal(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    lexer_init(&lexer, "1 != 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_NOT_EQUAL, "!=");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_less_than(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    lexer_init(&lexer, "1 < 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_LESS_THAN, "<");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_less_than_equal(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    lexer_init(&lexer, "1 <= 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_greater_than(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    lexer_init(&lexer, "1 > 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_GREATER_THAN, ">");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_binary_expression_greater_than_equal(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    lexer_init(&lexer, "1 >= 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_GREATER_THAN_EQUAL, ">=");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_INTEGER_LITERAL, "1");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_INTEGER_LITERAL, "2");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_logical_expression_and(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "true and false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_AND, "and");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_BOOLEAN_LITERAL, "true");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_BOOLEAN_LITERAL, "false");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_logical_expression_or(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "true or false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_BINARY);
    assert_token(runner, expression->binary._operator, TOKEN_OR, "or");
    assert_expression(runner, expression->binary.left->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.left->literal, TOKEN_BOOLEAN_LITERAL, "true");
    assert_expression(runner, expression->binary.right->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->binary.right->literal, TOKEN_BOOLEAN_LITERAL, "false");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_logical_expression_not(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "not true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_UNARY);
    assert_token(runner, expression->unary._operator, TOKEN_NOT, "not");
    assert_expression(runner, expression->unary.operand->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->unary.operand->literal, TOKEN_BOOLEAN_LITERAL, "true");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_variable_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "foo");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_VARIABLE);
    assert_token(runner, expression->identifier, TOKEN_IDENTIFIER, "foo");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_assignment_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "foo := 345");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_ASSIGNMENT);
    assert_expression(runner, expression->assignment.variable->kind, EXPRESSION_VARIABLE);
    assert_token(runner, expression->assignment.variable->identifier, TOKEN_IDENTIFIER, "foo");
    assert_expression(runner, expression->assignment.value->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->assignment.value->literal, TOKEN_INTEGER_LITERAL, "345");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_index_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "foo[0]");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_INDEX);
    assert_expression(runner, expression->index.variable->kind, EXPRESSION_VARIABLE);
    assert_token(runner, expression->index.variable->identifier, TOKEN_IDENTIFIER, "foo");
    assert_expression(runner, expression->index.value->kind, EXPRESSION_LITERAL);
    assert_token(runner, expression->index.value->literal, TOKEN_INTEGER_LITERAL, "0");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_function_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    array* parameters;
    Parameter* parameter;

    // Function expression with parameters
    lexer_init(&lexer, "(foo: int, bar: bool) => { do_something; };");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_expression(runner, expression->kind, EXPRESSION_FUNCTION);
    assert_base(runner, expression->function.arity == 2,
        "Function got %d parameters, expected 2", expression->function.arity);

    parameters = expression->function.parameters;

    assert_base(runner, parameters->length == 2,
        "Invalid number of parameters %d, expected 2", parameters->length);
    assert_parameter(runner, parameters->items[0], "foo", TYPE_SPECIFIER_INT);
    assert_parameter(runner, parameters->items[1], "bar", TYPE_SPECIFIER_BOOL);
    assert_statement(runner, expression->function.body->kind, STATEMENT_BLOCK);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Function expression with no parameters
    lexer_init(&lexer, "() => { do_something; };");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_FUNCTION);
    assert_base(runner, expression->function.arity == 0,
        "Function got %d parameters, expected 0", expression->function.arity);

    parameters = expression->function.parameters;

    assert_base(runner, parameters == NULL,
        "There should be no parameters");
    assert_statement(runner, expression->function.body->kind, STATEMENT_BLOCK);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_call_expression(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    AST_Expression* argument;

    lexer_init(&lexer, "foo(0, bar)");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_expression(runner, expression->kind, EXPRESSION_CALL);
    assert_expression(runner, expression->call.variable->kind, EXPRESSION_VARIABLE);
    assert_token(runner, expression->call.variable->identifier, TOKEN_IDENTIFIER, "foo");
    
    array* arguments = expression->call.arguments;
    assert_base(runner, arguments->length == 2,
        "Invalid number of arguments %d, expected 2", arguments->length);
    
    argument = arguments->items[0];
    assert_expression(runner, argument->kind, EXPRESSION_LITERAL);
    assert_token(runner, argument->literal, TOKEN_INTEGER_LITERAL, "0");

    argument = arguments->items[1];
    assert_expression(runner, argument->kind, EXPRESSION_VARIABLE);
    assert_token(runner, argument->identifier, TOKEN_IDENTIFIER, "bar");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


/*
static void test_order_of_arithmetic_operations(Test_Runner* runner)
{
    printf("\tOrder of arithmetic operations...");
    not_error = true;
    
    Lexer lexer;
    Parser parser;
    stringbuilder* sb;
    AST_Expression* expression;

    sb = sb_init();

    lexer_init(&lexer, "1 + 2 * 3");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    expression_to_string(expression, sb);

    assert_expression_str(sb_to_string(sb), "(1+(2*3))");

    sb_free(sb);
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Test for correct order of operations with parenthesized expressions
    sb = sb_init();

    lexer_init(&lexer, "(1 + 2) * 3");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    expression_to_string(expression, sb);

    assert_expression_str(sb_to_string(sb), "((1+2)*3)");

    sb_free(sb);
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Sequencial unary operators
    sb = sb_init();

    lexer_init(&lexer, "----7");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    expression_to_string(expression, sb);

    assert_expression_str(sb_to_string(sb), "(-(-(-(-7))))");

    sb_free(sb);
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // 10 - -7
    sb = sb_init();

    lexer_init(&lexer, "10 - -7");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    expression_to_string(expression, sb);

    assert_expression_str(sb_to_string(sb), "(10-(-7))");

    sb_free(sb);
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}
*/


static void test_expression_statement(Test_Runner* runner)
{
    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    // ---- binary arithmetics
    lexer_init(&lexer, "1 + 1;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert_statement(runner, statement->kind, STATEMENT_EXPRESSION);
    // assert_binary_expression_integer(statement->expression, TOKEN_PLUS, "1", "1");

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (runner->error) runner->failed++;
    else runner->passed++;
}


/*
static void test_block_statement(Test_Runner* runner)
{
    printf("\tBlock statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    lexer_init(&lexer, "{ 1 + 2;\n 3 * 4;\nreturn 0; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_BLOCK);
    assert(statement->block.statements_length == 3);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_if_statement(Test_Runner* runner)
{
    printf("\tIf statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    lexer_init(&lexer, "if a == b then { do_something; } else { do_else; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_IF);
    assert(statement->_if.condition->kind == EXPRESSION_BINARY);
    assert(statement->_if.then->kind = STATEMENT_BLOCK);
    assert(statement->_if._else->kind = STATEMENT_BLOCK);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_while_statement(Test_Runner* runner)
{
    printf("\tWhile statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    lexer_init(&lexer, "while a < b do { do_something; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_WHILE);
    assert(statement->_while.condition->kind == EXPRESSION_BINARY);
    assert(statement->_while.body->kind = STATEMENT_BLOCK);
    
    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_break_statement(Test_Runner* runner)
{
    printf("\tBreak statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    lexer_init(&lexer, "break;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_BREAK);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_return_statement(Test_Runner* runner)
{
    printf("\tReturn statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    lexer_init(&lexer, "return 0;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_RETURN);
    assert_literal_expression_integer(statement->_return.value, "0");

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_declaration_statement(Test_Runner* runner)
{
    printf("\tDeclaration statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    lexer_init(&lexer, "BAR: int = 0;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_DECLARATION);
    assert_declaration_variable_integer(statement->declaration, "BAR", TYPE_SPECIFIER_INT, "0");

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_type_specifier(Test_Runner* runner)
{
    printf("\tType specifier...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Type_Specifier specifier;
    
    // int
    lexer_init(&lexer, "int");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    specifier = parse_type_specifier(&parser);

    assert(specifier == TYPE_SPECIFIER_INT);

    parser_free(&parser);
    lexer_free(&lexer);

    // bool
    lexer_init(&lexer, "bool");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    specifier = parse_type_specifier(&parser);

    assert(specifier == TYPE_SPECIFIER_BOOL);

    parser_free(&parser);
    lexer_free(&lexer);

    // [int]
    lexer_init(&lexer, "[int]");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    specifier = parse_type_specifier(&parser);

    assert(specifier == TYPE_SPECIFIER_ARRAY_INT);

    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_variable_declaration(Test_Runner* runner)
{
    printf("\tVariable declaration...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Declaration* declaration;

    lexer_init(&lexer, "foo: int = 42;"); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration_variable_integer(declaration, "foo", TYPE_SPECIFIER_INT, "42");

    declaration_free(declaration);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_function_declaration(Test_Runner* runner)
{
    printf("\tFunction declartion...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Declaration* declaration;
    const char* source = "main: int = (argc: int, argv: [int]) => {"
                         "    return 0;"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration_function(declaration, "main", TYPE_SPECIFIER_INT, 2);

    declaration_free(declaration);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_small_program(Test_Runner* runner)
{
    printf("\tSmall program...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    AST_Declaration* declaration;
    const char* source = "main: int = (argc: int, argv: [int]) => {\n"
                         "    foo: int = 0;\n"
                         "    foo := 453;\n"
                         "\n"
                         "    return foo;\n"
                         "};";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration_function(declaration, "main", TYPE_SPECIFIER_INT, 2);
    array* statements = declaration->initializer->function.body->block.statements;
    assert(statements->length == 3);

    statement = (AST_Statement*)*statements->items++;
    assert(statement->kind == STATEMENT_DECLARATION);
    assert_declaration_variable_integer(statement->declaration, "foo", TYPE_SPECIFIER_INT, "0");

    statement = (AST_Statement*)*statements->items++;
    assert(statement->kind == STATEMENT_EXPRESSION);
    assert_assignment_expression_integer(statement->expression, "foo", "453");

    statement = (AST_Statement*)*statements->items;
    assert(statement->kind == STATEMENT_RETURN);
    statements->items -= 2; // rewind the array
    
    declaration_free(declaration);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_invalid_type_specifier(Test_Runner* runner)
{
    printf("\tDiagnose invalid type specifier...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    Type_Specifier specifier;
    AST_Statement* statement;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    
    lexer_init(&lexer, "type");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    specifier = parse_type_specifier(&parser);
    message = ":PARSER - SyntaxError: Expected type specifier, got 'type'\n";

    assert(specifier == TYPE_SPECIFIER_NONE);
    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    
    assert(strcmp(diagnostic->message, message) == 0);

    parser_free(&parser);
    lexer_free(&lexer);

    // in top level declaration
    lexer_init(&lexer, "foo: type = 32;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);
    message = ":PARSER - SyntaxError: Expected type specifier, got 'type'\n";

    // assert(specifier == TYPE_SPECIFIER_NONE);
    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    
    assert(strcmp(diagnostic->message, message) == 0);
    
    declaration_free(declaration);
    parser_free(&parser);
    lexer_free(&lexer);

    // in declaration statement
    lexer_init(&lexer, "foo: type = 32;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Expected type specifier, got 'type'\n";

    // assert(specifier == TYPE_SPECIFIER_NONE);
    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    
    assert(strcmp(diagnostic->message, message) == 0);
    
    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_invalid_primary_expression_starter(Test_Runner* runner)
{
    printf("\tDiagnose invalid primay expression starter...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;

    lexer_init(&lexer, "1 + while");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    message = ":PARSER - SyntaxError: Invalid token 'while' in primary expression\n";

    assert(expression->kind == EXPRESSION_BINARY);

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];

    assert(strcmp(diagnostic->message, message) == 0);
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_invalid_assignment_target(Test_Runner* runner)
{
    printf("\tDiagnose invalid assignment target...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    Diagnostic* diagnostic;
    char* message;
    
    lexer_init(&lexer, "1 := 42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    message = ":PARSER - SyntaxError: Invalid assignment target, expected a variable.\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ---

    lexer_init(&lexer, "true := false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    message = ":PARSER - SyntaxError: Invalid assignment target, expected a variable.\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_missing_semicolon(Test_Runner* runner)
{
    printf("\tDiagnose missing semicolon...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    char* message;
    
    // ---
    lexer_init(&lexer, "1 + 1");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token '<EoF>', expected ';'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    // ---
    lexer_init(&lexer, "return 0");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token '<EoF>', expected ';'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    // ---
    lexer_init(&lexer, "return 0 foo");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token 'foo', expected ';'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_missing_closing_curlybrace(Test_Runner* runner)
{
    printf("\tDiagnose missing closing curlybrace...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    char* message;

    lexer_init(&lexer, "{\n    foo: int = 42;\n    foo := 7;\n");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token '<EoF>', expected '}'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_missing_closing_parenthesis(Test_Runner* runner)
{
    printf("\tDiagnose missing closing parenthesis...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    char* message;

    // TODO(timo): parenthesized expressions

    // call expression statement
    lexer_init(&lexer, "foo(3, true;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token ';', expected ')'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    // TODO(timo): function parameters

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_invalid_while_statement(Test_Runner* runner)
{
    printf("\tDiagnose invalid while statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    char* message;

    // missing 'do' keyword
    lexer_init(&lexer, "while a < b {\n    a := a + b;\n}");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token '{', expected 'do'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_invalid_if_statement(Test_Runner* runner)
{
    printf("\tDiagnose invalid if statement...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    char* message;

    // missing 'then' keyword
    lexer_init(&lexer, "if a < b {\n    a := a + b;\n}");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    message = ":PARSER - SyntaxError: Invalid token '{', expected 'then'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_panic_mode_statement(Test_Runner* runner)
{
    printf("\tPanic mode at statement level...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Statement* statement;
    Diagnostic* diagnostic;
    char* message;
    char* source;

    // declaration statements missing a semicolon between
    source = "{\n"
             "    foo: int = 42\n"
             "    bar: bool = true;"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    
    assert(statement->block.statements->length == 2);
    assert(parser.diagnostics->length == 1);

    message = ":PARSER - SyntaxError: Invalid token 'bar', expected ';'\n";
    diagnostic = parser.diagnostics->items[0];

    assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    // some weird things
    source = "{\n"
             "    : int = 42\n"
             "    bar: bool = true;"
             "}";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);
    
    assert(statement->block.statements->length == 2);
    assert(parser.diagnostics->length == 2);

    message = ":PARSER - SyntaxError: Invalid token ':' in primary expression\n";
    diagnostic = parser.diagnostics->items[0];

    assert(strcmp(diagnostic->message, message) == 0);
    
    // NOTE(timo): This is not the correct behaviour
    // message = ":PARSER - SyntaxError: Invalid token 'int', expected ';'\n";
    // diagnostic = parser.diagnostics->items[1];

    // assert(strcmp(diagnostic->message, message) == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_panic_mode_declaration(Test_Runner* runner)
{
    printf("\tPanic mode at declaration level...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Declaration* declaration;
    Diagnostic* diagnostic;
    char* message;
    char* source;
    
    // missing semicolon between two declarations
    source = "foo: int = 42\n"
             "bar: bool = true\n;";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    assert(parser.declarations->length == 2);

    message = ":PARSER - SyntaxError: Invalid token 'bar', expected ';'\n";

    assert(parser.diagnostics->length == 1);
    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    parser_free(&parser);
    lexer_free(&lexer);

    // bigger mistake
    source = "foo: int 42\n"
             "bar: bool = true\n;";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    assert(parser.declarations->length == 2);
    assert(parser.diagnostics->length == 2);

    message = ":PARSER - SyntaxError: Invalid token '42', expected '='\n";

    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    message = ":PARSER - SyntaxError: Invalid token 'bar', expected ';'\n";

    diagnostic = parser.diagnostics->items[1];
    assert(strcmp(diagnostic->message, message) == 0);

    parser_free(&parser);
    lexer_free(&lexer);

    // even bigger mistake
    source = ": int 42\n"
             "bar: bool = true\n;";

    lexer_init(&lexer, source);
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    assert(parser.declarations->length == 2);
    assert(parser.diagnostics->length == 3);

    message = ":PARSER - SyntaxError: Invalid token ':', expected 'identifier'\n";

    diagnostic = parser.diagnostics->items[0];
    assert(strcmp(diagnostic->message, message) == 0);

    message = ":PARSER - SyntaxError: Invalid token '42', expected '='\n";

    diagnostic = parser.diagnostics->items[1];
    assert(strcmp(diagnostic->message, message) == 0);

    message = ":PARSER - SyntaxError: Invalid token 'bar', expected ';'\n";

    diagnostic = parser.diagnostics->items[2];
    assert(strcmp(diagnostic->message, message) == 0);

    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}
*/


Test_Set* parser_test_set()
{
    Test_Set* set = test_set("Parser");

    array_push(set->tests, test_case("Literal expression integer", test_literal_expression_integer));
    array_push(set->tests, test_case("Literal expression boolean", test_literal_expression_boolean));
    array_push(set->tests, test_case("Unary expression plus", test_unary_expression_plus));
    array_push(set->tests, test_case("Unary expression minus", test_unary_expression_minus));
    array_push(set->tests, test_case("Binary expression plus", test_binary_expression_plus));
    array_push(set->tests, test_case("Binary expression minus", test_binary_expression_minus));
    array_push(set->tests, test_case("Binary expression multiply", test_binary_expression_multiply));
    array_push(set->tests, test_case("Binary expression division", test_binary_expression_division));
    array_push(set->tests, test_case("Binary expression equal", test_binary_expression_equal));
    array_push(set->tests, test_case("Binary expression not equal", test_binary_expression_not_equal));
    array_push(set->tests, test_case("Binary expression less than", test_binary_expression_less_than));
    array_push(set->tests, test_case("Binary expression less than or equal", test_binary_expression_less_than_equal));
    array_push(set->tests, test_case("Binary expression greater than", test_binary_expression_greater_than));
    array_push(set->tests, test_case("Binary expression greater than or equal", test_binary_expression_greater_than_equal));
    array_push(set->tests, test_case("Logical expression and", test_logical_expression_and));
    array_push(set->tests, test_case("Logical expression or", test_logical_expression_or));
    array_push(set->tests, test_case("Logical expression not", test_logical_expression_not));
    array_push(set->tests, test_case("Variable expression", test_variable_expression));
    array_push(set->tests, test_case("Assignment expression", test_assignment_expression));
    array_push(set->tests, test_case("Index expression", test_index_expression));
    array_push(set->tests, test_case("Function expression", test_function_expression));
    array_push(set->tests, test_case("Call expression", test_call_expression));

    array_push(set->tests, test_case("Expression statement", test_expression_statement));

    set->length = set->tests->length;

    return set;
}


/*
void test_parser()
{
    printf("Running parser tests...\n");
    
    test_literal_expression();
    test_unary_expression();
    test_binary_expression_arithmetic();
    test_binary_expression_comparison();
    test_logical_expression();
    test_variable_expression();
    test_assignment_expression();
    test_index_expression();
    test_function_expression();
    test_call_expression();

    test_order_of_arithmetic_operations();
    // TODO(timo): correctly parsed expressions as string

    test_expression_statement();
    // TODO(timo): Variable statement expression 'foo;', for now it expects a colon
    // and type. So we should make sure that also this case is handled so we can
    // assign variables etc.
    // TODO(timo): test_expression_statement_variable();
    // - 'foo' => starts to parse declaration if there is nothing else after the identifier
    // - foo; => variable, not declaration
    // - foo(); => call, not declaration
    // - a := foo; => assignment, not declaration
    // - a: int = foo; declaration,
    // Also the 2nd check is needed to make sure that 'foo;' expresison is
    // handled correctly as a variable expression.
    // The 3rd check is needed to distinguish declaration and call expression
    // Falls down to the expression statement if the condition is not met.
    test_block_statement();
    test_if_statement();
    test_while_statement();
    test_break_statement();
    test_return_statement();
    test_declaration_statement();
    // TODO(timo): correctly parsed statements as string

    test_type_specifier();

    test_variable_declaration();
    test_function_declaration();
    // TODO(timo): correctly parsed declarations as string

    test_small_program();

    test_diagnose_invalid_type_specifier();
    test_diagnose_invalid_primary_expression_starter();
    test_diagnose_invalid_assignment_target();
    test_diagnose_missing_semicolon();
    test_diagnose_missing_closing_curlybrace();
    test_diagnose_missing_closing_parenthesis();
    test_diagnose_invalid_while_statement();
    test_diagnose_invalid_if_statement();
     
    test_panic_mode_statement();
    test_panic_mode_declaration();
}
*/
