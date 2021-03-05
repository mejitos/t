#include "tests.h"
#include "../src/t.h"
#include "../src/stringbuilder.h"


static bool not_error = true;


const char* expression_kinds[] =
{
    [EXPRESSION_NONE] = "none",
    [EXPRESSION_LITERAL] = "literal expression",
    [EXPRESSION_UNARY] = "unary expression",
    [EXPRESSION_BINARY] = "binary expression",
    [EXPRESSION_VARIABLE] = "variable expression",
    [EXPRESSION_ASSIGNMENT] = "assignment expression",
    [EXPRESSION_INDEX] = "index expression",
    [EXPRESSION_FUNCTION] = "function expression",
    [EXPRESSION_CALL] = "call expression",
};


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
            // sb_append(sb, lexemes[expression->binary._operator->kind]);
            sb_append(sb, expression->binary._operator->lexeme);
            expression_to_string(expression->binary.right, sb);
            sb_append(sb, ")");
            break;
        case EXPRESSION_UNARY:
            sb_append(sb, "(");
            // sb_append(sb, lexemes[expression->unary._operator->kind]);
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


void assert_expression_str(char* result, const char* expected)
{
    if (strcmp(result, expected) == 0) return;
    else
    {
        printf("Invalid expression '%s', expected '%s'", result, expected);
        not_error = false;
    }
}


void assert_literal_expression_integer(AST_Expression* expression, const char* value)
{
    if (expression->kind != EXPRESSION_LITERAL)
    {
        printf("\n\t\tInvalid expression kind '%s', expected literal expression", expression_kinds[expression->kind]);
        not_error = false;
    }
    assert(expression->literal->kind == TOKEN_INTEGER_LITERAL);
    assert(strcmp(expression->literal->lexeme, value) == 0);
}


void assert_literal_expression_boolean(AST_Expression* expression, const char* value)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    assert(expression->literal->kind == TOKEN_BOOLEAN_LITERAL);
    assert(strcmp(expression->literal->lexeme, value) == 0);
}


void assert_unary_expression(AST_Expression* expression, Token_Kind _operator, Expression_Kind operand)
{
    assert(expression->kind == EXPRESSION_UNARY);
    assert(expression->unary._operator->kind == _operator);
    assert(expression->unary.operand->kind == operand);
}


void assert_unary_expression_integer(AST_Expression* expression, Token_Kind _operator, const char* operand)
{
    assert_unary_expression(expression, _operator, EXPRESSION_LITERAL);
    assert_literal_expression_integer(expression->unary.operand, operand);
}


void assert_unary_expression_boolean(AST_Expression* expression, Token_Kind _operator, const char* operand)
{
    assert_unary_expression(expression, _operator, EXPRESSION_LITERAL);
    assert_literal_expression_boolean(expression->unary.operand, operand);
}


void assert_binary_expression(AST_Expression* expression, Token_Kind _operator, Expression_Kind left, Expression_Kind right)
{
    assert(expression->kind == EXPRESSION_BINARY);
    assert(expression->binary._operator->kind == _operator);
    assert(expression->binary.left->kind = left);
    assert(expression->binary.right->kind = right);
}


void assert_binary_expression_integer(AST_Expression* expression, Token_Kind _operator, const char* left, const char* right)
{
    assert_binary_expression(expression, _operator, EXPRESSION_LITERAL, EXPRESSION_LITERAL);
    assert_literal_expression_integer(expression->binary.left, left);
    assert_literal_expression_integer(expression->binary.right, right);
}


void assert_binary_expression_boolean(AST_Expression* expression, Token_Kind _operator, const char* left, const char* right)
{
    assert_binary_expression(expression, _operator, EXPRESSION_LITERAL, EXPRESSION_LITERAL);
    assert_literal_expression_boolean(expression->binary.left, left);
    assert_literal_expression_boolean(expression->binary.right, right);
}


void assert_variable_expression(AST_Expression* expression, const char* identifier)
{
    assert(expression->kind == EXPRESSION_VARIABLE);
    assert(strcmp(expression->identifier->lexeme, identifier) == 0);
}


void assert_assignment_expression_integer(AST_Expression* expression, const char* identifier, const char* value)
{
    assert(expression->kind == EXPRESSION_ASSIGNMENT);
    assert_variable_expression(expression->assignment.variable, identifier);
    assert_literal_expression_integer(expression->assignment.value, value);
}


void assert_index_expression(AST_Expression* expression, const char* identifier, const char* value)
{
    if (expression->kind != EXPRESSION_INDEX)
    {
        printf("\n\t\tInvalid expression kind '%d', expected index expression");
        not_error = false;
    }
    assert_variable_expression(expression->index.variable, identifier);
    assert_literal_expression_integer(expression->index.value, value);
}


void assert_declaration_variable_integer(AST_Declaration* declaration, const char* identifier, Type_Specifier specifier, const char* value)
{
    assert(declaration->kind == DECLARATION_VARIABLE);
    assert(strcmp(declaration->identifier->lexeme, identifier) == 0);
    assert(declaration->specifier == specifier);
    assert_literal_expression_integer(declaration->initializer, value);
}


void assert_declaration_function(AST_Declaration* declaration, const char* identifier, Type_Specifier specifier, int arity)
{
    assert(declaration->kind == DECLARATION_FUNCTION);
    assert(strcmp(declaration->identifier->lexeme, identifier) == 0);
    assert(declaration->specifier == specifier);
    assert(declaration->initializer->kind == EXPRESSION_FUNCTION);
    assert(declaration->initializer->function.arity == arity);
}


static void test_literal_expression()
{
    printf("\tLiteral expressions...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    // Integer literal
    lexer_init(&lexer, "767");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_literal_expression_integer(expression, "767");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Boolean literal 
    lexer_init(&lexer, "true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_literal_expression_boolean(expression, "true");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Boolean literal 
    lexer_init(&lexer, "false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_literal_expression_boolean(expression, "false");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_unary_expression()
{
    printf("\tUnary expressions...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    // Unary minus
    lexer_init(&lexer, "-42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_unary_expression_integer(expression, TOKEN_MINUS, "42");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Unary plus 
    lexer_init(&lexer, "+42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_unary_expression_integer(expression, TOKEN_PLUS, "42");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_binary_expression_arithmetic()
{
    printf("\tArithmetic binary expressions...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    // Binary plus expression
    lexer_init(&lexer, "1 + 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_binary_expression_integer(expression, TOKEN_PLUS, "1", "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
        
    // Binary minus expression
    lexer_init(&lexer, "3 - 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_MINUS, "3", "4");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Binary multiply expression
    lexer_init(&lexer, "5 * 6");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_MULTIPLY, "5", "6");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Binary division expression
    lexer_init(&lexer, "7 / 8");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_DIVIDE, "7", "8");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_binary_expression_comparison()
{
    printf("\tComparison binary expressions...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    
    // Equality expression
    lexer_init(&lexer, "1 == 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_binary_expression_integer(expression, TOKEN_IS_EQUAL, "1", "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 != 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_NOT_EQUAL, "1", "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Ordering expression
    lexer_init(&lexer, "1 < 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_LESS_THAN, "1", "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // ----
    lexer_init(&lexer, "1 <= 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_LESS_THAN_EQUAL, "1", "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 > 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_GREATER_THAN, "1", "2");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 >= 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_GREATER_THAN_EQUAL, "1", "2");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_logical_expression()
{
    printf("\tLogical expressions...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    // --- and
    lexer_init(&lexer, "true and false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_boolean(expression, TOKEN_AND, "true", "false");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // --- or 
    lexer_init(&lexer, "true or false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_boolean(expression, TOKEN_OR, "true", "false");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // --- not
    lexer_init(&lexer, "not true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_unary_expression_boolean(expression, TOKEN_NOT, "true");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_variable_declaration()
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


static void test_variable_expression()
{
    printf("\tVariable expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    // Base case for variable expression
    lexer_init(&lexer, "foo");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_variable_expression(expression, "foo");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_assignment_expression()
{
    printf("\tAssignment expression...");
    not_error = true;
    
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "foo := 345");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_assignment_expression_integer(expression, "foo", "345");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_index_expression()
{
    printf("\tIndex expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;

    lexer_init(&lexer, "foo[0]");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_index_expression(expression, "foo", "0");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_function_expression()
{
    printf("\tFunction expression...");
    not_error = true;

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

    assert(expression->kind == EXPRESSION_FUNCTION);
    assert(expression->function.arity == 2);

    parameters = expression->function.parameters;
    assert(parameters->length == 2);
    
    parameter = parameters->items[0];
    assert(strcmp(parameter->identifier->lexeme, "foo") == 0);
    assert(parameter->specifier == TYPE_SPECIFIER_INT);
    
    parameter = parameters->items[1];
    assert(strcmp(parameter->identifier->lexeme, "bar") == 0);
    assert(parameter->specifier == TYPE_SPECIFIER_BOOL);

    assert(expression->function.body->kind == STATEMENT_BLOCK);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Function expression with no parameters
    lexer_init(&lexer, "() => { do_something; };");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert(expression->kind == EXPRESSION_FUNCTION);
    assert(expression->function.arity == 0);

    parameters = expression->function.parameters;
    assert(parameters == NULL);
    
    assert(expression->function.body->kind == STATEMENT_BLOCK);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}



static void test_call_expression()
{
    printf("\tCall expression...");
    not_error = true;

    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    AST_Expression* argument;

    lexer_init(&lexer, "foo(0, bar)");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert(expression->kind == EXPRESSION_CALL);
    assert_variable_expression(expression->call.variable, "foo");
    
    array* arguments = expression->call.arguments;
    assert(arguments->length == 2);
    
    argument = arguments->items[0];
    assert_literal_expression_integer(argument, "0");

    argument = arguments->items[1];
    assert_variable_expression(argument, "bar");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_order_of_arithmetic_operations()
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


static void test_expression_statement()
{
    printf("\tExpression statements...");
    not_error = true;
    
    Lexer lexer;
    Parser parser;
    AST_Statement* statement;

    // ---- binary arithmetics
    lexer_init(&lexer, "1 + 1;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_EXPRESSION);
    assert_binary_expression_integer(statement->expression, TOKEN_PLUS, "1", "1");

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_block_statement()
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


static void test_if_statement()
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


static void test_while_statement()
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


static void test_break_statement()
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


static void test_return_statement()
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


static void test_declaration_statement()
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


static void test_type_specifier()
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


static void test_function_declaration()
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


static void test_small_program()
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


static void test_diagnose_invalid_type_specifier()
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


static void test_diagnose_invalid_primary_expression_starter()
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


static void test_diagnose_invalid_assignment_target()
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


static void test_diagnose_missing_semicolon()
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


static void test_panic_mode_statement()
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
    /*
    message = ":PARSER - SyntaxError: Invalid token 'int', expected ';'\n";
    diagnostic = parser.diagnostics->items[1];

    assert(strcmp(diagnostic->message, message) == 0);
    */

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_panic_mode_declaration()
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
     
    test_panic_mode_statement();
    test_panic_mode_declaration();
}
