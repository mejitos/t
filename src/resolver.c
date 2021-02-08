#include "t.h"


Type* type_integer()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_INTEGER;
    type->size = 4;
}


Type* type_boolean()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_BOOLEAN;
    type->size = 1;
}


void resolver_init(Resolver* resolver)
{
    *resolver = (Resolver){ .global = scope_init(NULL) };
}


void resolver_free(Resolver* resolver)
{
    // TODO(timo): We also should free the contents of the symbol table
    free(resolver->global);
    resolver->global = NULL;

    // NOTE(timo): The resolver itself is not being freed since it is
    // being initialized in the top level function
}


static Type* resolve_literal_expression(AST_Expression* expression)
// static void resolve_literal_expression(AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    // TODO(timo): We should probably annotate the expression with
    // the value. That way we could already free the literal token
    // if we wanted to.
    // TODO(timo): But we actually should just return the type of
    // the expression and not the value. Unless if we are doing
    // constant folding but even then the values could be accessed
    // from elsewhere.
    
    Type* type;
    Value value;

    switch (expression->literal.literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        {
            const char* lexeme = expression->literal.literal->lexeme;
            int integer_value = 0;

            while (*lexeme != '\0') 
            { 
                // TODO(timo): We should actually make sure that the character is a digit

                // Converts ascii digit to corresponding number
                int digit = *lexeme - '0';

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
                lexeme++;
            }
            
            type = type_integer();
            value = (Value){ .integer = integer_value };
            // TODO(timo): Add the value (and type?) to the expression
            
            // return type;
            break;
        }
        case TOKEN_BOOLEAN_LITERAL:
        {
            bool boolean_value;

            if (strcmp(expression->literal.literal->lexeme, "true") == 0)
                boolean_value = true;
            else if (strcmp(expression->literal.literal->lexeme, "false") == 0)
                boolean_value = false;

            type = type_boolean();
            value = (Value){ .boolean = boolean_value };
            // TODO(timo): Add the value (and type?) to the expression

            // return type;
            break;
        }
        default:
            // TODO(timo): Error
            break;
    }

    expression->type = type;
    expression->literal.value = value;

    return type;
}


// static Value resolve_unary_expression(AST_Expression* expression)
static Type* resolve_unary_expression(AST_Expression* expression)
// static void resolve_unary_expression(AST_Expression* expression)
{
    // Type* operand = resolve_expression(expression->unary.operand);
    resolve_expression(expression->unary.operand);

    AST_Expression* operand = expression->unary.operand;
    Token* _operator = expression->binary._operator;

    // TODO(timo): We should probably check for the negative overflow
    // in here, since the negative value is being returned from here

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            if (operand->type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand type\n");
                exit(1);
            }
            break;
        case TOKEN_MINUS:
            if (operand->type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand type\n");
                exit(1);
            }
            // TODO(timo): Check for the negative integer overflow
            // The smallest 32-bit integer is -2147483648
            // The biggest 32-bit integer is 2147483647
            break;
        case TOKEN_NOT:
            if (operand->type->kind != TYPE_BOOLEAN)
            {
                printf("Invalid operand type\n");
                exit(1);
            }
            break;
        default:
            // TODO(timo): Error
            // Even though this should never be reached, since this is
            // handled at the parsing stage already
            printf("Invalid unary operator\n");
            break;
    }

    expression->type = operand->type;

    return operand->type;
}


// static Value resolve_binary_expression(AST_Expression* expression)
static Type* resolve_binary_expression(AST_Expression* expression)
// static void resolve_binary_expression(AST_Expression* expression)
{
    // TODO(timo): Since the integer overlows can happen with these operations
    // too, we should check them in here also => make some utility function
    // Type* left = resolve_expression(expression->binary.left);
    // Type* right = resolve_expression(expression->binary.right);
    resolve_expression(expression->binary.left);
    resolve_expression(expression->binary.right);

    AST_Expression* left = expression->binary.left;
    AST_Expression* right = expression->binary.right;
    Token* _operator = expression->binary._operator;

    Type* type;
    
    // TODO(timo): Do we want booleans to have some other expressions
    // allowed than the logical expressions? 
    switch (_operator->kind)
    {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
            if (left->type->kind != TYPE_INTEGER && right->type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand types\n");
                exit(1);
            }
            // TODO(timo): Check for integer overflow here too
            type = left->type;
            break;
        case TOKEN_IS_EQUAL:
        case TOKEN_NOT_EQUAL:
            // The left type and the right type has to be the same
            // Also the equality of the booleans can be compared
            if (left->type->kind == TYPE_INTEGER && right->type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand types\n");
                exit(1);
            }
            else if (left->type->kind == TYPE_BOOLEAN && right->type->kind != TYPE_BOOLEAN)
            {
                printf("Invalid operand types\n");
                exit(1);
            }
            type = type_boolean();
            break;
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_EQUAL:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_EQUAL:
            // TODO(timo): Operand/values has to be scalar types e.g. integers
            // in this case for the ordering expression
            if (left->type->kind == TYPE_INTEGER && right->type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand types\n");
                exit(1);
            }
            type = type_boolean();
            break;
        default:
            // TODO(timo): Error
            // Even though this should never be reached, since this is
            // handled at the parsing stage already
            printf("Invalid binary operator\n");
            break;
    }

    expression->type = type;

    return type;
}


static Type* resolve_variable_expression(AST_Expression* expression)
// static void resolve_variable_expression(AST_Expression* expression)
{
    Type* type;

    // We check if the variable is declared
    // if not => error

    // We get the variable type from the symbol table
    // return the type

    return type;
}


// Value resolve_expression(AST_Expression* expression)
Type* resolve_expression(AST_Expression* expression)
// void resolve_expression(AST_Expression* expression)
{
    // TODO(timo): We should pass a expected type into here so we can also 
    // make sure that the types are also something that we want then to be
    // NOTE(timo): The type of thing X will be in the symbol table and therefore
    // it can be accessed from there. Expressions only produce and return values
    // and therefore the typechecking of the expected type wont happen in the
    // expression resolving

    Type* type;

    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            type = resolve_literal_expression(expression);
            // resolve_literal_expression(expression);
            break;
        case EXPRESSION_UNARY:
            type = resolve_unary_expression(expression);
            // resolve_unary_expression(expression);
            break;
        case EXPRESSION_BINARY:
            type = resolve_binary_expression(expression);
            // resolve_binary_expression(expression);
            break;
        default:
            break;
    }

    /*
    if (result.type != expected_type)
    {
        printf("Conflicting types\n");
        exit(1);
    }
    */

    return type;
}


void resolve_statement(AST_Statement* statement)
{
    //
}


Type* resolve_type_specifier(Type_Specifier specifier)
{
    switch (specifier)
    {
        case TYPE_SPECIFIER_INT:
            return type_integer();
        case TYPE_SPECIFIER_BOOL:
            return type_boolean();
        default:
            // TODO(timo): Error
            break;
    }
}


void resolve_variable_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    // Create the symbol from the declaration
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->kind = SYMBOL_VARIABLE;
    symbol->identifier = declaration->identifier->lexeme;

    // Declare it in the scope symbol table. Scope handles the possible name collisions
    scope_declare(resolver->global, symbol);

    // Then we should resolve the type of the name
    Type* expected_type = resolve_type_specifier(declaration->specifier);
    
    // Then resolve the initializer
    Type* actual_type = resolve_expression(declaration->initializer);
    
    // Check if the expected type and the actual type match
    if (expected_type->kind != actual_type->kind)
    {
        printf("Conflicting types in variable declaration\n");
        exit(1);
    }
    
    // Symbol resolved

    symbol->type = expected_type;
}


void resolve_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
            resolve_variable_declaration(resolver, declaration);
            break;
        case DECLARATION_FUNCTION:
        default:
            // TODO(timo): Error
            break;
    }
}


void resolve(array* declarations)
{
    // TODO(timo): Since we are not creating some lightweight scripting language
    // this time, this function should also work differently
    // We cannot just "resolve everything by iterating through the declarations"
    // and instead at this level we should just declare all the top level declarations
    // and after that we actually resolve the declarations. This is just to avoid
    // the forcing of the ordering in the top level code. We shouldn't really care
    // when the function x for variable y is declared in relation to the scope
    // it is actually being used.
    // TODO(timo): We probably should also create a abstraction for the program itself
}
