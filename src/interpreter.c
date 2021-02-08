#include "t.h"


static Value evaluate_literal_expression(AST_Expression* expression)
{
    // TODO(timo): At this point we should have this value somewhere saved
    // but now it isn't. We have just resolved it, but nothing else.
    // Is it time to annotate the tree?
    switch (expression->literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_BOOLEAN_LITERAL:
            return expression->value;
        default:
            // TODO(timo): Error
            break;
    }

}


static Value evaluate_unary_expression(AST_Expression* expression)
{
    Value operand = evaluate_expression(expression->unary.operand);
    Token* _operator = expression->unary._operator;

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = +operand.integer_value};
        case TOKEN_MINUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = -operand.integer_value};
        case TOKEN_NOT:
        default:
            break;
    }
}


static Value evaluate_binary_expression(AST_Expression* expression)
{
    Value left = evaluate_expression(expression->binary.left);
    Value right = evaluate_expression(expression->binary.right);
    Token* _operator = expression->binary._operator;

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value + right.integer_value };
        case TOKEN_MINUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value - right.integer_value };
        case TOKEN_MULTIPLY:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value * right.integer_value };
        case TOKEN_DIVIDE:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value / right.integer_value };
        case TOKEN_IS_EQUAL:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value == right.integer_value };
        case TOKEN_NOT_EQUAL:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value != right.integer_value };
        case TOKEN_LESS_THAN:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value < right.integer_value };
        case TOKEN_LESS_THAN_EQUAL:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value <= right.integer_value };
        case TOKEN_GREATER_THAN:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value > right.integer_value };
        case TOKEN_GREATER_THAN_EQUAL:
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = left.integer_value >= right.integer_value };
        default:
            // TODO(timo): Error
            break;
    }
}


Value evaluate_expression(AST_Expression* expression)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            return evaluate_literal_expression(expression);
        case EXPRESSION_UNARY:
            return evaluate_unary_expression(expression);
        case EXPRESSION_BINARY:
            return evaluate_binary_expression(expression);
        default:
            // TODO(timo): Error
            break;
    }
}


void evaluate_statement(AST_Statement* statement)
{
    switch (statement->kind)
    {
        case STATEMENT_EXPRESSION:
        case STATEMENT_BLOCK:
        case STATEMENT_RETURN:
        default:
            break;
    }
}


static void evaluate_variable_declaration(AST_Declaration* declaration)
{
    assert(declaration->kind == DECLARATION_VARIABLE);

    // Declare the identifier in the current scope
    // Name collisions should've been handled by the resolver

    // There is really is not anything else to do
}


static void evaluate_function_declaration(AST_Declaration* declaration)
{
    assert(declaration->kind == DECLARATION_FUNCTION);

    // Declare the identifier in the current scope
    // Name collisions should've been handled by the resolver

    // Begin a new scope for the function

    // Add the variables into the scope

    // Evaluate the body

    // Can this function actually be void, since we should be
    // able to return something?
    // No since this is a declaration and not a function call,
    // here we just gather the information of the declared function
}


void evaluate_declaration(AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
            evaluate_variable_declaration(declaration);
            break;
        case DECLARATION_FUNCTION:
            evaluate_function_declaration(declaration);
            break;
        default:
            // Error
            break;
    }
}


void interpret(array* declarations)
{
    for (int i = 0; i < declarations->length; i++)
        evaluate_declaration(declarations->items[i]);
}
