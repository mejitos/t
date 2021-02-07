#include "t.h"


Value resolve_literal_expression(AST_Expression* expression)
{
    switch (expression->literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        {
            const char* literal = expression->literal->lexeme;
            int integer_value = 0;

            while (*literal != '\0') 
            { 
                // TODO(timo): We should actually make sure that the character is a digit

                // Converts ascii digit to corresponding number
                int digit = *literal - '0';

                // Check for integer overflow in integer literal
                // TODO(timo): We probably should handle the negative overflow too?
                // like value < INT_MIN - digit
                // TODO(timo): Make separate functions out of these overflow checks
                if (integer_value > INT_MAX - digit) 
                {   
                    // TODO(timo): Error
                    printf("Integer overflow\n");
                    exit(1);
                }

                integer_value *= 10; 
                integer_value += digit;
                literal++;
            }
            
            // TODO(timo): With integers we actually know that the value is a
            // constant value and therefore we could do some constant folding
            // so do we need a flag for that or is the type itself enough?
            return (Value){ .type = VALUE_INTEGER,
                            .integer_value = integer_value };
        }
        case TOKEN_BOOLEAN_LITERAL:
        {
            bool boolean_value;

            if (strcmp(expression->literal->lexeme, "true") == 0)
                boolean_value = true;
            else if (strcmp(expression->literal->lexeme, "false") == 0)
                boolean_value = false;
            
            return (Value){ .type = VALUE_BOOLEAN,
                            .boolean_value = boolean_value };
        }
        default:
            // TODO(timo): Error
            break;
    }
}


Value resolve_unary_expression(AST_Expression* expression)
{
    Value operand = resolve_expression(expression->unary.operand);
    // TODO(timo): We should probably check for the negative overflow
    // in here, since the negative value is being returned from here
    return NULL;
}


Value resolve_binary_expression(AST_Expression* expression)
{
    // TODO(timo): Since the integer overlows can happen with these operations
    // too, we should check them in here also => make some utility function
    Value left = resolve_expression(expression->binary.left);
    Value right = resolve_expression(expression->binary.right);
    
    // TODO(timo): Do we want booleans to have some other expressions
    // allowed than the logical expressions? 
    if (! (is_int(left) && is_int(right)))
    {
        printf("Invalid operand types\n");
        exit(1);
    }
    switch (expression->binary._operator->kind)
    {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
            break;
        case TOKEN_IS_EQUAL:
        case TOKEN_NOT_EQUAL:
            break;
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_EQUAL:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_EQUAL:
            // TODO(timo): Operand/values has to be scalar types e.g. integers
            // in this case for hits operation to be possible
            break;
        default:
            // TODO(timo): Error
            break;
    }

    return NULL;
}


Value resolve_expression(AST_Expression* expression, Value_Type expected_type)
{
    // TODO(timo): We should pass a expected type into here so we can also 
    // make sure that the types are also something that we want then to be
    Value result;

    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            result = resolve_literal_expression(expression);
            break;
        case EXPRESSION_UNARY:
            result = resolve_unary_expression(expression);
            break;
        case EXPRESSION_BINARY:
            result = resolve_binary_expression(expression);
            break;
        default:
            break;
    }

    // TODO(timo): This is also the place to add the type to 
    // the expression if we want to annotate the AST? 
    // e.g.
    // expression->type = result.type;
    //
    // Do we need a separate abstraction for the type itself?

    if (result.type != expected_type)
    {
        printf("Conflicting types\n");
        exit(1);
    }

    return result;
}


void resolve_statement(AST_Statement* statement)
{
    //
}


void resolve_declaration(AST_Declaration* declaration)
{
    //
}


void resolve(array* declarations)
{
    //
}
