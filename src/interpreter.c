#include "t.h"


void interpreter_init(Interpreter* interpreter)
{
    *interpreter = (Interpreter) {};
}


static Value evaluate_literal_expression(AST_Expression* expression)
{
    switch (expression->literal.literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_BOOLEAN_LITERAL:
            return expression->literal.value;
        default:
            // TODO(timo): Error
            break;
    }

}


static Value evaluate_unary_expression(Interpreter* interpreter, AST_Expression* expression)
{
    Value operand = evaluate_expression(interpreter, expression->unary.operand);
    Token* _operator = expression->unary._operator;

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer = +operand.integer };
        case TOKEN_MINUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer = -operand.integer };
        case TOKEN_NOT:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = !operand.boolean };
        default:
            break;
    }
}


static Value evaluate_binary_expression(Interpreter* interpreter, AST_Expression* expression)
{
    Value left = evaluate_expression(interpreter, expression->binary.left);
    Value right = evaluate_expression(interpreter, expression->binary.right);
    Token* _operator = expression->binary._operator;

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer = left.integer + right.integer };
        case TOKEN_MINUS:
            return (Value){ .type = VALUE_INTEGER,
                            .integer = left.integer - right.integer };
        case TOKEN_MULTIPLY:
            return (Value){ .type = VALUE_INTEGER,
                            .integer = left.integer * right.integer };
        case TOKEN_DIVIDE:
            return (Value){ .type = VALUE_INTEGER,
                            .integer = left.integer / right.integer };
        case TOKEN_IS_EQUAL:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = left.integer == right.integer };
        case TOKEN_NOT_EQUAL:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = left.integer != right.integer };
        case TOKEN_LESS_THAN:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = left.integer < right.integer };
        case TOKEN_LESS_THAN_EQUAL:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = left.integer <= right.integer };
        case TOKEN_GREATER_THAN:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = left.integer > right.integer };
        case TOKEN_GREATER_THAN_EQUAL:
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean = left.integer >= right.integer };
        default:
            // TODO(timo): Error
            break;
    }
}


Value evaluate_expression(Interpreter* interpreter, AST_Expression* expression)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            return evaluate_literal_expression(expression);
        case EXPRESSION_UNARY:
            return evaluate_unary_expression(interpreter, expression);
        case EXPRESSION_BINARY:
            return evaluate_binary_expression(interpreter, expression);
        default:
            // TODO(timo): Error
            break;
    }
}


static void evaluate_block_statement(Interpreter* interpreter, AST_Statement* statement)
{
    //
}


static void evaluate_return_statement(Interpreter* interpreter, AST_Statement* statement)
{
    //
}


void evaluate_statement(Interpreter* interpreter, AST_Statement* statement)
{
    switch (statement->kind)
    {
        case STATEMENT_EXPRESSION:
            // evaluate_expression(interpreter, statement);
            break;
        case STATEMENT_BLOCK:
            evaluate_block_statement(interpreter, statement);
            break;
        case STATEMENT_RETURN:
            evaluate_return_statement(interpreter, statement);
            break;
        default:
            // TODO(timo): error
            break;
    }
}


static void evaluate_variable_declaration(Interpreter* interpreter, AST_Declaration* declaration)
{
    assert(declaration->kind == DECLARATION_VARIABLE);

    // Declare the identifier in the current scope
    // Name collisions should've been handled by the resolver

    // There is really is not anything else to do
}


static void evaluate_function_declaration(Interpreter* interpreter, AST_Declaration* declaration)
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


void evaluate_declaration(Interpreter* interpreter, AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
            evaluate_variable_declaration(interpreter, declaration);
            break;
        case DECLARATION_FUNCTION:
            evaluate_function_declaration(interpreter, declaration);
            break;
        default:
            // TODO(timo): Error
            break;
    }
}


void interpret(Interpreter* interpreter)
{
    for (int i = 0; i < interpreter->declarations->length; i++)
        evaluate_declaration(interpreter, interpreter->declarations->items[i]);
}
