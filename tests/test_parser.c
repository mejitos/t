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
            switch (expression->literal->kind)
            {
                // TODO(timo): function to turn ints and bools to strings
                // for now I use this "dumb and boring" way just to get 
                // things to work. I could implement my own int_to_char later.
                case TOKEN_INTEGER_LITERAL:
                {
                    // Integers will fit into 12 characters, but if 64-bit values
                    // are used, then we need at least 21 characters
                    char digit[12];
                    sprintf(digit, "%d", expression->literal->integer_value);
                    sb_append(sb, digit);

                    return sb;
                }
                case TOKEN_BOOLEAN_LITERAL:
                    if (expression->literal->boolean_value == true)
                        sb_append(sb, "true");
                    else if (expression->literal->boolean_value == false)
                        sb_append(sb, "false");

                    return sb;
                default:
                    sb_append(sb, "unknown literal");
                    break;
            }
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


void assert_literal_expression_integer(AST_Expression* expression, int value)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    assert(expression->literal->kind == TOKEN_INTEGER_LITERAL);
    assert(expression->literal->integer_value == value);
}


void assert_literal_expression_boolean(AST_Expression* expression, bool value)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    assert(expression->literal->kind == TOKEN_BOOLEAN_LITERAL);
    assert(expression->literal->boolean_value == value);
}


void assert_unary_expression(AST_Expression* expression, Token_Kind _operator, Expression_Kind operand)
{
    assert(expression->kind == EXPRESSION_UNARY);
    assert(expression->unary._operator->kind == _operator);
    assert(expression->unary.operand->kind == operand);
}


void assert_unary_expression_integer(AST_Expression* expression, Token_Kind _operator, int operand)
{
    assert_unary_expression(expression, _operator, EXPRESSION_LITERAL);
    assert_literal_expression_integer(expression->unary.operand, operand);
}


void assert_unary_expression_boolean(AST_Expression* expression, Token_Kind _operator, bool operand)
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


void assert_binary_expression_integer(AST_Expression* expression, Token_Kind _operator, int left, int right)
{
    assert_binary_expression(expression, _operator, EXPRESSION_LITERAL, EXPRESSION_LITERAL);
    assert_literal_expression_integer(expression->binary.left, left);
    assert_literal_expression_integer(expression->binary.right, right);
}


void assert_binary_expression_boolean(AST_Expression* expression, Token_Kind _operator, bool left, bool right)
{
    assert_binary_expression(expression, _operator, EXPRESSION_LITERAL, EXPRESSION_LITERAL);
    assert_literal_expression_boolean(expression->binary.left, left);
    assert_literal_expression_boolean(expression->binary.right, right);
}


void assert_declaration_variable_integer(AST_Declaration* declaration, const char* identifier, Type_Specifier specifier, int value)
{
    assert(declaration->kind == DECLARATION_VARIABLE);
    assert(strcmp(declaration->identifier->identifier, identifier) == 0);
    assert(declaration->specifier == specifier);
    assert(declaration->initializer->kind == EXPRESSION_LITERAL);
    assert(declaration->initializer->literal->integer_value == value);
}


void assert_declaration_function(AST_Declaration* declaration, const char* identifier, Type_Specifier specifier, int arity)
{
    assert(declaration->kind == DECLARATION_FUNCTION);
    assert(strcmp(declaration->identifier->identifier, identifier) == 0);
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


    // Base cases for expression statements
    lexer_init(&lexer, "1 + 1;");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    statement = parse_statement(&parser);

    assert(statement->kind == STATEMENT_EXPRESSION);

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
    assert(statement->_return.value->literal->integer_value == 0);

    statement_free(statement);
    parser_free(&parser);
    lexer_free(&lexer);

    // Base case for variable delcaration
    source = "foo: int = 42;";

    lexer_init(&lexer, source); 
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    declaration = parse_declaration(&parser);

    assert_declaration_variable_integer(declaration, "foo", TYPE_SPECIFIER_INT, 42);

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
    
    assert_literal_expression_integer(expression, 767);
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Boolean literal expression
    lexer_init(&lexer, "true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_literal_expression_boolean(expression, true);
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Boolean literal expression
    lexer_init(&lexer, "false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_literal_expression_boolean(expression, false);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Binary plus expression
    lexer_init(&lexer, "1 + 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_binary_expression_integer(expression, TOKEN_PLUS, 1, 2);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
        
    // Binary minus expression
    lexer_init(&lexer, "3 - 4");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_MINUS, 3, 4);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Binary multiply expression
    lexer_init(&lexer, "5 * 6");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_MULTIPLY, 5, 6);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Binary division expression
    lexer_init(&lexer, "7 / 8");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_DIVIDE, 7, 8);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Unary minus expression
    lexer_init(&lexer, "-42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_unary_expression_integer(expression, TOKEN_MINUS, 42);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Unary plus expression
    lexer_init(&lexer, "+42");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_unary_expression_integer(expression, TOKEN_PLUS, 42);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Equality expression
    lexer_init(&lexer, "1 == 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);
    
    assert_binary_expression_integer(expression, TOKEN_IS_EQUAL, 1, 2);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 != 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_NOT_EQUAL, 1, 2);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Ordering expression
    lexer_init(&lexer, "1 < 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_LESS_THAN, 1, 2);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // ----
    lexer_init(&lexer, "1 <= 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_LESS_THAN_EQUAL, 1, 2);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 > 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_GREATER_THAN, 1, 2);

    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // ----
    lexer_init(&lexer, "1 >= 2");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_integer(expression, TOKEN_GREATER_THAN_EQUAL, 1, 2);
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // Logical expressions
    // --- and
    lexer_init(&lexer, "true and false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_boolean(expression, TOKEN_AND, true, false);
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // --- or 
    lexer_init(&lexer, "true or false");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_binary_expression_boolean(expression, TOKEN_OR, true, false);
    
    expression_free(expression);
    parser_free(&parser);
    lexer_free(&lexer);

    // --- not
    lexer_init(&lexer, "not true");
    lex(&lexer);

    parser_init(&parser, lexer.tokens);
    expression = parse_expression(&parser);

    assert_unary_expression_boolean(expression, TOKEN_NOT, true);
    
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
