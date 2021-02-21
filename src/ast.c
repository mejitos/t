#include "t.h"


AST_Declaration* function_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer)
{
    AST_Declaration* declaration = xcalloc(1, sizeof (AST_Declaration));
    declaration->kind = DECLARATION_FUNCTION;
    declaration->identifier = identifier;
    declaration->specifier = specifier;
    declaration->initializer = initializer;

    return declaration;
}


AST_Declaration* variable_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer)
{
    AST_Declaration* declaration = xcalloc(1, sizeof (AST_Declaration));
    declaration->kind = DECLARATION_VARIABLE;
    declaration->identifier = identifier;
    declaration->specifier = specifier;
    declaration->initializer = initializer;

    return declaration;
}


AST_Statement* expression_statement(AST_Expression* expression)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_EXPRESSION;
    statement->expression = expression;

    return statement;
}


AST_Statement* block_statement(array* statements, int statements_length)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_BLOCK;
    statement->block.statements = statements;
    statement->block.statements_length = statements_length;

    return statement;
}


AST_Statement* if_statement(AST_Expression* condition, AST_Statement* then, AST_Statement* _else)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_IF;
    statement->_if.condition = condition;
    statement->_if.then = then;
    statement->_if._else = _else;

    return statement;
}


AST_Statement* while_statement(AST_Expression* condition, AST_Statement* body)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_WHILE;
    statement->_while.condition = condition;
    statement->_while.body = body;

    return statement;
}


AST_Statement* break_statement()
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_BREAK;

    return statement;
}


AST_Statement* return_statement(AST_Expression* value)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_RETURN;
    statement->_return.value = value;

    return statement;
}


AST_Statement* declaration_statement(AST_Declaration* declaration)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_DECLARATION;
    statement->declaration = declaration;

    return statement;
}


AST_Expression* literal_expression(Token* literal)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_LITERAL;
    expression->literal.literal = literal;

    return expression;
}


AST_Expression* unary_expression(Token* _operator, AST_Expression* operand)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_UNARY;
    expression->unary._operator = _operator;
    expression->unary.operand = operand;

    return expression;
}


AST_Expression* binary_expression(AST_Expression* left, Token* _operator, AST_Expression* right)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_BINARY;
    expression->binary.left = left;
    expression->binary._operator = _operator;
    expression->binary.right = right;

    return expression;
}


AST_Expression* variable_expression(Token* identifier)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_VARIABLE;
    expression->identifier = identifier;

    return expression;
}


AST_Expression* assignment_expression(AST_Expression* variable, AST_Expression* value)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_ASSIGNMENT;
    expression->assignment.variable = variable;
    expression->assignment.value = value;

    return expression;
}


AST_Expression* index_expression(AST_Expression* variable, AST_Expression* value)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_INDEX;
    expression->index.variable = variable;
    expression->index.value = value;

    return expression;
}


Parameter* function_parameter(Token* identifier, Type_Specifier specifier)
{
    Parameter* parameter = xcalloc(1, sizeof (Parameter));
    parameter->identifier = identifier;
    parameter->specifier = specifier;

    return parameter;
}


AST_Expression* function_expression(array* parameters, int arity, AST_Statement* body)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_FUNCTION;
    expression->function.parameters = parameters;
    expression->function.arity = arity;
    expression->function.body = body;

    return expression;
}


void expression_free(AST_Expression* expression)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            // NOTE(timo); Lexer will free the literal token 
            // and there is nothing else allocated
            break;
        case EXPRESSION_UNARY:
            expression_free(expression->unary.operand);
            break;
        case EXPRESSION_BINARY:
            expression_free(expression->binary.left);
            expression_free(expression->binary.right);
            break;
        case EXPRESSION_VARIABLE:
            // NOTE(timo); This can be just free'd since the expression
            // only has the identifier, and lexer will free the token
            break;
        case EXPRESSION_ASSIGNMENT:
            expression_free(expression->assignment.variable);
            expression_free(expression->assignment.value);
            break;
        case EXPRESSION_INDEX:
            expression_free(expression->index.variable);
            expression_free(expression->index.value);
            break;
        case EXPRESSION_FUNCTION:
            for (int i = 0; i < expression->function.parameters->length; i++)
            {
                free(expression->function.parameters->items[i]);
                expression->function.parameters->items[i] = NULL;
            }
            array_free(expression->function.parameters);
            statement_free(expression->function.body);
            break; 
    }

    free(expression);
    expression = NULL;
}


void statement_free(AST_Statement* statement)
{
    switch (statement->kind)
    {
        case STATEMENT_EXPRESSION:
            expression_free(statement->expression);
            break;
        case STATEMENT_BLOCK:
            for (int i = 0; i < statement->block.statements_length; i++)
            {
                statement_free(statement->block.statements->items[i]);
                statement->block.statements->items[i] = NULL;
            }

            array_free(statement->block.statements);
            break;
        case STATEMENT_IF:
            expression_free(statement->_if.condition);
            statement_free(statement->_if.then);
            if (statement->_if._else)
                statement_free(statement->_if._else);
            break;
        case STATEMENT_WHILE:
            expression_free(statement->_while.condition);
            statement_free(statement->_while.body);
            break;
        case STATEMENT_BREAK:
            break;
        case STATEMENT_RETURN:
            expression_free(statement->_return.value);
            break;
        case STATEMENT_DECLARATION:
            declaration_free(statement->declaration);
            break;
    }

    free(statement);
    statement = NULL;
}


void declaration_free(AST_Declaration* declaration)
{
    expression_free(declaration->initializer);
    free(declaration);
    declaration = NULL;
}
