#include "t.h"


void resolver_init(Resolver* resolver)
{
    *resolver = (Resolver){ .global = scope_init(NULL),
                            .context.not_in_loop = true,
                            .context.not_in_function = true, };
}


void resolver_free(Resolver* resolver)
{
    // TODO(timo): We also should free the contents of the symbol table
    // scope_free(resolver->global)
    free(resolver->global);
    resolver->global = NULL;

    // NOTE(timo): The resolver itself is not being freed since it is
    // being initialized to the stack in the top level function
}


static Type* resolve_literal_expression(AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    
    Type* type;
    Value value;
    AST_Literal literal = expression->literal;

    switch (literal.literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        {
            const char* lexeme = literal.literal->lexeme;
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
            value = (Value){ .type = VALUE_INTEGER, 
                             .integer = integer_value };
            // TODO(timo): Add the value (and type?) to the expression
            break;
        }
        case TOKEN_BOOLEAN_LITERAL:
        {
            bool boolean_value;

            if (strcmp(literal.literal->lexeme, "true") == 0)
                boolean_value = true;
            else if (strcmp(literal.literal->lexeme, "false") == 0)
                boolean_value = false;

            type = type_boolean();
            value = (Value){ .type = VALUE_BOOLEAN,
                              .boolean = boolean_value };
            // TODO(timo): Add the value (and type?) to the expression
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


static Type* resolve_unary_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* operand_type = resolve_expression(resolver, expression->unary.operand);
    Token* _operator = expression->unary._operator;

    // TODO(timo): We should probably check for the negative overflow
    // in here, since the negative value is being returned from here

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            if (operand_type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand type\n");
                exit(1);
            }
            break;
        case TOKEN_MINUS:
            if (operand_type->kind != TYPE_INTEGER)
            {
                printf("Invalid operand type\n");
                exit(1);
            }
            // TODO(timo): Check for the negative integer overflow
            // The smallest 32-bit integer is -2147483648
            // The biggest 32-bit integer is 2147483647
            break;
        case TOKEN_NOT:
            if (operand_type->kind != TYPE_BOOLEAN)
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

    expression->type = operand_type;

    return operand_type;
}


static Type* resolve_binary_expression(Resolver* resolver, AST_Expression* expression)
{
    // TODO(timo): Since the integer overlows can happen with these operations
    // too, we should check them in here also => make some utility function
    Type* type_left = resolve_expression(resolver, expression->binary.left);
    Type* type_right = resolve_expression(resolver, expression->binary.right);
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
            if (type_left->kind != TYPE_INTEGER && type_right->kind != TYPE_INTEGER)
            {
                printf("Invalid operand types\n");
                exit(1);
            }
            // TODO(timo): Check for integer overflow here too
            type = type_left;
            break;
        case TOKEN_IS_EQUAL:
        case TOKEN_NOT_EQUAL:
            // The left type and the right type has to be the same
            // Also the equality of the booleans can be compared
            if (type_left->kind == TYPE_INTEGER && type_right->kind != TYPE_INTEGER)
            {
                printf("Invalid operand types\n");
                exit(1);
            }
            else if (type_left->kind == TYPE_BOOLEAN && type_right->kind != TYPE_BOOLEAN)
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
            if (type_left->kind == TYPE_INTEGER && type_right->kind != TYPE_INTEGER)
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


static Type* resolve_variable_expression(Resolver* resolver, AST_Expression* expression)
{
    Symbol* symbol = scope_lookup(resolver->global, expression->identifier->lexeme);
    
    if (symbol == NULL)
    {
        // TODO(timo): Error
        printf("Undeclared identifier '%s'\n", expression->identifier->lexeme);
        exit(1);
    }

    Type* type = symbol->type; 

    return type;
}


static Type* resolve_assignment_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* variable_type = resolve_expression(resolver, expression->assignment.variable);
    Type* value_type = resolve_expression(resolver, expression->assignment.value);
    
    if (variable_type->kind != value_type->kind)
    {
        // TODO(timo): Error
        printf("Conflicting types in assignment expression\n");
        exit(1);
    }

    return variable_type;
}


static Type* resolve_index_expression(Resolver* resolver, AST_Expression* expression)
{
    // NOTE(timo): Index expression at this point is only allowed to be used with
    // command line arguments so no general array functionality is added for now.

    // Make sure that the accessed variable is argv and nothing else
    // TODO(timo): Are the program/command line arguments passed to scope?
    AST_Expression* variable = expression->index.variable;
    // Type* variable_type = resolve_expression(resolver, variable);
    // Symbol* argv = scope_lookup(resolver->global, variable->identifier->lexeme);
    // Symbol* argc = scope_lookup(resolver->global, "argc");

    // TODO(timo): probably should make sure that the variable type is array type so it can actually 
    // be subscripted. Since we don't have general arrays, we don't need this check for now.
    // Type* variable_type = resolve_expression(resolver, variable);
    // if (variable_type->kind != TYPE_ARRAY_INTEGER)
    
    /*
    if (strcmp(argv->identifier, "argv") != 0)
    {
        // TODO(timo): error
        printf("You cannot use index expressions with other values than argv of the main function\n");
        exit(1);
    }
    */

    // we should make sure that the type of the expression is integer
    Type* index_type = resolve_expression(resolver, expression->index.value);

    if (index_type->kind != TYPE_INTEGER)
    {
        // TODO(timo): Error
        printf("Array subscript cannot be other type than integer\n");
        exit(1);
    }
    
    /*
    // the index cannot be < 0 and not > array length - 1
    Value index_value = expression->index.value->literal.value;

    assert(index_value.type == VALUE_INTEGER);

    if (index_value.integer < 0)
    {
        // TODO(timo): Error
        printf("Array subscript less than zero is not allowed\n");
        exit(1);
    }
    if (index_value.integer > argc->value.integer)
    {
        // TODO(timo): Error
        printf("Array subscript greater than array length\n");
        exit(1);
    }
    */

    return index_type;
}


static Type* resolve_function_expression(Resolver* resolver, AST_Expression* expression)
{
    resolver->context.not_in_function = false;
    // resolver->context.return_type = type_none();

    // Begin a new scope

    // Resolve parameters if there is some
    if (expression->function.arity > 0)
    {
        array* parameters = expression->function.parameters;

        for (int i = 0; i < parameters->length; i++)
            scope_declare(resolver->global, symbol_parameter((Parameter*)parameters->items[i]));
    }

    // Resolve body and return type
    resolve_statement(resolver, expression->function.body);
     
    // End scope
    
    resolver->context.not_in_function = true;

    // NOTE(timo): Decided to force only single return statement per function, so this can actually 
    // return something. Even though this information could carry inside the context structure.
    return resolver->context.return_type;
}


static Type* resolve_call_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* variable_type = resolve_expression(resolver, expression->call.variable);
    array* arguments = expression->call.arguments;
    Symbol* function = scope_lookup(resolver->global, expression->call.variable->identifier->lexeme);

    // Number of arguments == arity of the called function
    if (function->arity != arguments->length)
    {
        // TODO(timo): Error
        printf("Function X expected %d arguments, but %d was given\n", function->arity, arguments->length);
        exit(1);
    }
    
    // TODO(timo): Types of the passed arguments cannot be resolved if we do things like this
    // since we cannot map the passed arguments to the function parameters.
    /*
    // types of the arguments == types of the parameters
    for (int i = 0; i < arguments->length; i++)
    {
        Type* argument_type = resolve_expression(resolve, (Expression*)arguments->items[i]);
        Symbol* parameter = scope_lookup(resolver->global, 

        if 
    }
    */

    // return the return type of the called function
    return variable_type;
}


Type* resolve_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;

    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            type = resolve_literal_expression(expression);
            break;
        case EXPRESSION_VARIABLE:
            type = resolve_variable_expression(resolver, expression);
            break;
        case EXPRESSION_ASSIGNMENT:
            type = resolve_assignment_expression(resolver, expression);
            break;
        case EXPRESSION_UNARY:
            type = resolve_unary_expression(resolver, expression);
            break;
        case EXPRESSION_BINARY:
            type = resolve_binary_expression(resolver, expression);
            break;
        case EXPRESSION_INDEX:
            type = resolve_index_expression(resolver, expression);
            break;
        case EXPRESSION_CALL:
            type = resolve_call_expression(resolver, expression);
            break;
        case EXPRESSION_FUNCTION:
            type = resolve_function_expression(resolver, expression);
            break;
        default:
            break;
    }

    return type;
}


void resolve_block_statement(Resolver* resolver, AST_Statement* statement)
{
    // TODO(timo): enter the scope of the block

    array* statements = statement->block.statements;
    
    for (int i = 0; i < statements->length; i++)
        resolve_statement(resolver, statements->items[i]); 

    // TODO(timo): leave the scope of the block
}


void resolve_if_statement(Resolver* resolver, AST_Statement* statement)
{
    Type* condition = resolve_expression(resolver, statement->_if.condition);

    if (condition->kind != TYPE_BOOLEAN)
    {
        printf("Conditional expression must produce boolean value\n");
        exit(1);
    }

    resolve_statement(resolver, statement->_if.then);

    if (statement->_if._else) resolve_statement(resolver, statement->_if._else);
}


void resolve_while_statement(Resolver* resolver, AST_Statement* statement)
{
    Type* condition = resolve_expression(resolver, statement->_while.condition);

    if (condition->kind != TYPE_BOOLEAN)
    {
        printf("Conditional expression must produce boolean value\n");
        exit(1);
    }

    resolver->context.not_in_loop = false;
    resolve_statement(resolver, statement->_while.body);
    resolver->context.not_in_loop = true;
}


void resolve_return_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_RETURN);

    if (resolver->context.not_in_function)
    {
        printf("Can't return outside of functions\n");
        exit(1);
    }

    if (resolver->context.returned)
    {
        printf("YORO - You only return once\n");
        exit(1);
    }
    
    // NOTE(timo): Since we are in return statement, we have something to return
    // and for now explicit returning without value is not possible.
    // TODO(timo): There must be a better way
    // free(resolver->context.return_type);

    resolver->context.return_type = resolve_expression(resolver, statement->_return.value);
}


void resolve_break_statement(Resolver* resolver)
{
    if (resolver->context.not_in_loop)
    {
        printf("Can't break outside of loops");
        exit(1);
    }
}


void resolve_statement(Resolver* resolver, AST_Statement* statement)
{
    // NOTE(timo): In case of statements we cant really return anything
    // since the statements themselves don't return anything. For example
    // the return statement. Function can return at any point so we can't
    // resolve the function body in a way that we return just a single value

    switch (statement->kind)
    {
        case STATEMENT_EXPRESSION:
            resolve_expression(resolver, statement->expression);
            break;
        case STATEMENT_BLOCK:
            resolve_block_statement(resolver, statement);
            break;
        case STATEMENT_DECLARATION:
            resolve_declaration(resolver, statement->declaration);
            break;
        case STATEMENT_RETURN:
            resolve_return_statement(resolver, statement);
            break;
        case STATEMENT_IF:
            resolve_if_statement(resolver, statement);
            break;
        case STATEMENT_WHILE:
            resolve_while_statement(resolver, statement);
            break;
        case STATEMENT_BREAK:
            resolve_break_statement(resolver);
            break;
        default:
            // TODO(timo): Error
            break;
    }
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


void resolve_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    // Declare the symbol into the current scope
    Symbol* symbol;

    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
            symbol = symbol_variable(declaration);
            break;
        case DECLARATION_FUNCTION:
            symbol = symbol_function(declaration);
            break;
        default:
            // TODO(timo): Error
            break;
    }

    scope_declare(resolver->global, symbol);

    // Check for the state of the symbol
    // TODO(timo): This symbol state is not probably needed when we are resolving
    // things in order?
    assert(symbol->state == STATE_UNRESOLVED);

    if (symbol->state == STATE_RESOLVED) return;
    if (symbol->state == STATE_RESOLVING)
    {
        // TODO(timo): Error
        printf("For some reason we are trying to resolve the same declaration\n");
        exit(1);
    }

    symbol->state = STATE_RESOLVING;

    // NOTE(timo): So this type will be the type of the value assigned to the variable 
    // or if the declaration is function, this will be the type of the return value.
    Type* type = resolve_expression(resolver, declaration->initializer);
    
    // Check if the expected type and the actual type match
    if (symbol->type->kind != type->kind)
    {
        printf("Conflicting types in declaration\n");
        exit(1);
    }
    
    // Symbol resolved
    symbol->state = STATE_RESOLVED;
}


void resolve(Resolver* resolver, array* declarations)
{
    // TODO(timo): We probably should create a abstraction for the program itself
    for (int i = 0; i < declarations->length; i++)
    {
        AST_Declaration* declaration = declarations->items[i];
        resolve_declaration(resolver, declarations->items[i]);
    }
}
