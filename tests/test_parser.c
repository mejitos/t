#include "tests.h"
#include "../src/t.h"
#include "../src/stringbuilder.h"


static bool not_error = true;
const char* lexemes[] = 
{
    [TOKEN_PLUS]                = "+",
    [TOKEN_MINUS]               = "-",
    [TOKEN_MULTIPLY]            = "*",
    [TOKEN_DIVIDE]              = "/",
    [TOKEN_IS_EQUAL]            = "==",
    [TOKEN_NOT_EQUAL]           = "!=",
    [TOKEN_LESS_THAN]           = "<",
    [TOKEN_LESS_THAN_EQUAL]     = "<=",
    [TOKEN_GREATER_THAN]        = ">",
    [TOKEN_GREATER_THAN_EQUAL]  = ">=",
    [TOKEN_AND]                 = "and",
    [TOKEN_OR]                  = "or",
    [TOKEN_NOT]                 = "not",
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
            sb_append(sb, lexemes[expression->binary._operator->kind]);
            expression_to_string(expression->binary.right, sb);
            sb_append(sb, ")");
            break;
        case EXPRESSION_UNARY:
            sb_append(sb, "(");
            sb_append(sb, lexemes[expression->unary._operator->kind]);
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
    assert(expression->kind == EXPRESSION_LITERAL);
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


void test_parser()
{
    printf("Running parser tests...");
    char* source;
    Lexer lexer;
    Parser parser;
    AST_Expression* expression;
    AST_Statement* statement;
    AST_Declaration* declaration;
    stringbuilder* sb;

    // Small program which declares variable, assigns new value to it and returns it
    source = "main: int = (argc: int, argv: int) => {\n"
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

    // Base cases for expression statements
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

    // Base case for block statement
    lexer_init(&lexer, "{ 1 + 2;\n 3 * 4;\nreturn 0; }");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_BLOCK);
    assert(statement->block.statements_length == 3);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Base case for return statement
    lexer_init(&lexer, "return 0;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_RETURN);
    assert_literal_expression_integer(statement->_return.value, "0");

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    // Base case for declaration statement
    lexer_init(&lexer, "BAR: int = 0;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_DECLARATION);
    assert_declaration_variable_integer(statement->declaration, "BAR", TYPE_SPECIFIER_INT, "0");

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);


    // Base case for variable delcaration
    source = "foo: int = 42;";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration_variable_integer(declaration, "foo", TYPE_SPECIFIER_INT, "42");

    declaration_free(declaration);
    parser_free(&parser);
    lexer_free(&lexer);

    // Base case for main program / function declaration
    source = "main: int = (argc: int, argv: int) => {"
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

    // Integer literal expression
    lexer_init(&lexer, "767");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_literal_expression_integer(expression, "767");
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Boolean literal expression
    lexer_init(&lexer, "true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_literal_expression_boolean(expression, "true");
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Boolean literal expression
    lexer_init(&lexer, "false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_literal_expression_boolean(expression, "false");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
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

    // Unary minus expression
    lexer_init(&lexer, "-42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_unary_expression_integer(expression, TOKEN_MINUS, "42");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Unary plus expression
    lexer_init(&lexer, "+42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_unary_expression_integer(expression, TOKEN_PLUS, "42");

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

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

    // Logical expressions
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

    // Base case for variable expression
    lexer_init(&lexer, "foo");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_variable_expression(expression, "foo");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Base case for assignment expression
    lexer_init(&lexer, "foo := 345");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_assignment_expression_integer(expression, "foo", "345");
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Test for correct order of operations with arithmetics 
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

    if (not_error) printf("OK\n");
    else printf("\n");
}
