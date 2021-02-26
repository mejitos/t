#include "t.h"


void interpreter_init(Interpreter* interpreter)
{
    *interpreter = (Interpreter) { .global = scope_init(NULL) };
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


Value evaluate_variable_expression(Interpreter* interpreter, AST_Expression* expression)
{
    Symbol* symbol = scope_lookup(interpreter->global, expression->identifier->lexeme);

    // NOTE(timo): We could check for null value, but resolver should 
    // have handled this. The famous last words.
    
    return symbol->value;
}


Value evaluate_assignment_expression(Interpreter* interpreter, AST_Expression* expression)
{
    AST_Expression* variable = expression->assignment.variable;
    Symbol* symbol = scope_lookup(interpreter->global, variable->identifier->lexeme);
    
    // TODO(timo): Some error handling here?
    symbol->value = evaluate_expression(interpreter, expression->assignment.value);

    return symbol->value;
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
        case EXPRESSION_VARIABLE:
            return evaluate_variable_expression(interpreter, expression);
        case EXPRESSION_ASSIGNMENT:
            return evaluate_assignment_expression(interpreter, expression);
        // TODO(timo):
        // case EXPRESSION_FUNCTION:
        // case EXPRESSION_CALL:
        // case EXPRESSION_INDEX:
        default:
            // TODO(timo): Error
            break;
    }
}


static void evaluate_expression_statement(Interpreter* interpreter, AST_Statement* statement)
{
    //
}


static void evaluate_block_statement(Interpreter* interpreter, AST_Statement* statement)
{
    // begin scope

    for (int i = 0; i < statement->block.statements->length; i++)
        evaluate_statement(interpreter, statement->block.statements->items[i]);

    // leave scope
}


static void evaluate_if_statement(Interpreter* interpreter, AST_Statement* statement)
{
    //
}


static void evaluate_while_statement(Interpreter* interpreter, AST_Statement* statement)
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
            evaluate_expression_statement(interpreter, statement);
            break;
        case STATEMENT_DECLARATION:
            // TODO(timo): Is this enough?
            evaluate_declaration(interpreter, statement->declaration);
            break;
        case STATEMENT_BLOCK:
            evaluate_block_statement(interpreter, statement);
            break;
        case STATEMENT_IF:
            evaluate_if_statement(interpreter, statement);
            break;
        case STATEMENT_WHILE:
            evaluate_while_statement(interpreter, statement);
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
    // Name collisions should've been handled by the resolver,
    // now we are more interested of the value
    Symbol* symbol = symbol_variable(declaration);
    symbol->value = evaluate_expression(interpreter, declaration->initializer);

    scope_declare(interpreter->global, symbol);
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
    // TODO(timo): We should also pass the finished symbol table at least for the
    // global symbols to be used by the interpreter. That way we only need the
    // entry point of the program and we just just interpret that.
    // TODO(timo): Therefore we really can't have this loop in here but instead
    // normal routine that handles the program parameters etc. stuff and then 
    // just starts interpreting the body of the program
    for (int i = 0; i < interpreter->declarations->length; i++)
        evaluate_declaration(interpreter, interpreter->declarations->items[i]);
}
