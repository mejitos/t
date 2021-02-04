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


AST_Statement* block_statement(array* statements, int statements_length)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_BLOCK;
    statement->block.statements = statements;
    statement->block.statements_length = statements_length;

    return statement;
}


AST_Statement* return_statement(AST_Expression* value)
{
    AST_Statement* statement = xcalloc(1, sizeof (AST_Statement));
    statement->kind = STATEMENT_RETURN;
    statement->_return.value = value;

    return statement;
}


AST_Expression* literal_expression(Token* literal)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_LITERAL;
    expression->literal = literal;

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
