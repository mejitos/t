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
    scope_free(resolver->global);

    // NOTE(timo): The resolver itself is not being freed since it is
    // being initialized to the stack in the top level function
}


static Type* resolve_literal_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    
    Type* type;
    Value value;
    Token* literal = expression->literal;

    switch (literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        {
            const char* lexeme = literal->lexeme;
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
            break;
        }
        case TOKEN_BOOLEAN_LITERAL:
        {
            bool boolean_value;

            if (strcmp(literal->lexeme, "true") == 0)
                boolean_value = true;
            else if (strcmp(literal->lexeme, "false") == 0)
                boolean_value = false;

            type = type_boolean();
            value = (Value){ .type = VALUE_BOOLEAN,
                              .boolean = boolean_value };
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, expression->position,
                ":RESOLVER - Unreachable: Invalid literal '%s'", 
                literal->lexeme);
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
    }

    expression->type = type;
    expression->value = value;

    return type;
}


const char* type_as_string(Type_Kind type)
{
    switch (type)
    {
        case TYPE_INTEGER:
            return "int";
        case TYPE_BOOLEAN:
            return "bool";
        default:
            return "unknown type";
    }
}


static Type* resolve_unary_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* operand_type = resolve_expression(resolver, expression->unary.operand);
    Token* _operator = expression->unary._operator;

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
            if (operand_type->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary '+'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);
            }
            break;
        case TOKEN_MINUS:
            if (operand_type->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary '-'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);
            }
            // TODO(timo): Check for the negative integer overflow
            // The smallest 32-bit integer is -2147483648
            // The biggest 32-bit integer is 2147483647
            break;
        case TOKEN_NOT:
            if (operand_type->kind != TYPE_BOOLEAN)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary 'not'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);
            }
            break;
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, _operator->position,
                ":RESOLVER - Unreachable: Invalid unary operator '%s'",
                _operator->lexeme);
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
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
    
    switch (_operator->kind)
    {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
            if (type_left->kind != TYPE_INTEGER && type_right->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);
            }
            // TODO(timo): Check for integer overflow here too
            type = type_left;
            break;
        case TOKEN_IS_EQUAL:
        case TOKEN_NOT_EQUAL:
            if ((type_left->kind == TYPE_INTEGER && type_right->kind != TYPE_INTEGER) || 
                (type_left->kind == TYPE_BOOLEAN && type_right->kind != TYPE_BOOLEAN))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);
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
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);
            }
            type = type_boolean();
            break;
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, _operator->position,
                ":RESOLVER - Unreachable: Invalid binary operator '%s'",
                _operator->lexeme);
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
    }

    expression->type = type;

    return type;
}


static Type* resolve_variable_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;
    Symbol* symbol = scope_lookup(resolver->global, expression->identifier->lexeme);
     
    if (symbol == NULL)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - SyntaxError: Referencing identifier '%s' before declaring it",
            expression->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);
        
        type = type_none();
    }
    else
        type = symbol->type;

    return type;
}


static Type* resolve_assignment_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* variable_type = resolve_expression(resolver, expression->assignment.variable);
    Type* value_type = resolve_expression(resolver, expression->assignment.value);
    
    if (variable_type->kind != value_type->kind)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Conflicting types in assignment expression. Assigning to '%s' from '%s'",
            type_as_string(variable_type->kind), type_as_string(value_type->kind));
        array_push(resolver->diagnostics, _diagnostic);
    }

    return variable_type;
}


static Type* resolve_index_expression(Resolver* resolver, AST_Expression* expression)
{
    // Make sure that the accessed variable is argv and nothing else
    // TODO(timo): Are the program/command line arguments passed to scope?
    AST_Expression* variable = expression->index.variable;
    
    // Make sure the subscript target is an array type
    Type* variable_type = resolve_expression(resolver, variable);
    Symbol* symbol = scope_lookup(resolver->global, variable->identifier->lexeme);

    if (variable_type->kind != TYPE_ARRAY)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: '%s' is not subscriptable.",
            variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);
    }
    /*
    // TODO(timo): We should also make sure that the current context is the main functions context
    if (strcmp(symbol->identifier, "argv") != 0)
    {
        // TODO(timo): Error
        printf("You cannot use index expressions with other values than argv of the main function\n");
        exit(1);
    }
    */

    // we should make sure that the type of the expression is integer
    Type* index_type = resolve_expression(resolver, expression->index.value);
    
    if (index_type->kind != TYPE_INTEGER)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Array subscripts must be integers.",
            variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);
    }
    
    // TODO(timo): Boundary checks => the index cannot be < 0 and not > array length - 1
    Value index_value = expression->index.value->value;

    if (index_value.integer < 0)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - IndexError: Array subscript less than zero.",
            variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);
    }
    

    // TODO(timo): The array size should be gotten from initializing the array but since
    // we have the arrays only for the argv, they are resolved elsewhere
    // TODO(timo): We don't have a way to get the value of the argc for now
    /*
    if (index_value.integer > argc->value.integer)
    {
        // TODO(timo): Error
        printf("Array subscript greater than array length\n");
        exit(1);
    }
    */
    
    // TODO(timo): We prbably should make sure that the elements of the array are integers
    // but that is not needed for now since we only have arrays for argv

    // TODO(timo): We also have the element_type member in the array type, can/should we use it for something?

    // TODO(timo): Should we return the type of the index or the element type?
    return index_type;
}


static Type* resolve_function_expression(Resolver* resolver, AST_Expression* expression)
{
    // TODO(timo): If we are in function at this point, error out.
    // if not in function -> resolve function -> return function type
    // else -> error -> return none type

    resolver->context.not_in_function = false;

    // Begin a new scope

    Type* type = type_function();

    // Resolve parameters if there is some
    if (expression->function.arity > 0)
    {
        array* parameters = expression->function.parameters;

        for (int i = 0; i < parameters->length; i++)
        {
            Parameter* parameter = parameters->items[i];
            Type* parameter_type = resolve_type_specifier(resolver, parameter->specifier);
            Symbol* symbol = symbol_parameter(parameter->identifier->lexeme, parameter_type);
            // array_push(type->function.parameters, parameter_type);
            scope_declare(resolver->global, symbol);
            array_push(type->function.parameters, symbol);
        }

        type->function.arity = type->function.parameters->length;
    }

    // Resolve body and return type
    resolve_statement(resolver, expression->function.body);
     
    // End scope
    
    resolver->context.not_in_function = true;

    // NOTE(timo): Decided to force only single return statement per function, so this can actually 
    // return something. Even though this information could carry inside the context structure.
    type->function.return_type = resolver->context.return_type;

    return type;
}


static Type* resolve_call_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type = resolve_expression(resolver, expression->call.variable);
    array* arguments = expression->call.arguments;
    Symbol* symbol = scope_lookup(resolver->global, expression->call.variable->identifier->lexeme);

    // Make sure the called variable is actually callable - a function in our case
    if (type->kind != TYPE_FUNCTION)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: '%s' is not callable.",
            symbol->identifier);
        array_push(resolver->diagnostics, _diagnostic);
        // TODO(timo): return type none?
    }

    // Number of arguments == arity of the called function
    if (symbol->type->function.arity != arguments->length)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Function '%s' expected %d arguments, but %d was given\n", 
            symbol->identifier, symbol->type->function.arity, arguments->length);
        array_push(resolver->diagnostics, _diagnostic);
        // TODO(timo): return type none?
    }
    
    // types of the arguments == types of the parameters
    for (int i = 0; i < arguments->length; i++)
    {
        AST_Expression* argument = (AST_Expression*)arguments->items[i];
        Symbol* parameter = symbol->type->function.parameters->items[i];

        Type* argument_type = resolve_expression(resolver, argument);
        Type* parameter_type = parameter->type;

        if (argument_type->kind != parameter_type->kind)
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, expression->position,
                ":RESOLVER - TypeError: Parameter '%s' is of type '%s', but argument of type '%s' was given.", 
                parameter->identifier, type_as_string(parameter_type->kind), type_as_string(argument_type->kind));
            array_push(resolver->diagnostics, _diagnostic);
        }
    }

    // return the return type of the called function
    return type->function.return_type;
}


Type* resolve_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;

    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            type = resolve_literal_expression(resolver, expression);
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
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, expression->position,
                ":RESOLVER - Unreachable: Invalid expression");
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
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

    if (statement->_if._else) 
        resolve_statement(resolver, statement->_if._else);
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
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, statement->position,
                ":RESOLVER - Unreachable: Invalid statement");
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
    }
}


Type* resolve_type_specifier(Resolver* resolver, Type_Specifier specifier)
{
    switch (specifier)
    {
        case TYPE_SPECIFIER_INT:
            return type_integer();
        case TYPE_SPECIFIER_BOOL:
            return type_boolean();
        case TYPE_SPECIFIER_ARRAY_INT:
            return type_array(type_integer());
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, (Position) {0},
                ":RESOLVER - Unreachable: Invalid type specifier");
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
    }
}


void resolve_variable_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    // NOTE(timo): So this type will be the type of the value assigned to the variable 
    // or if the declaration is function, this will be the type of the return value.
    Type* expected_type = resolve_type_specifier(resolver, declaration->specifier);
    Type* actual_type = resolve_expression(resolver, declaration->initializer);

    if (expected_type->kind != actual_type->kind)
    {
        // TODO(timo): Error
        printf("Conflicting types in variable declaration\n");
        exit(1);
    }

    // Declare the symbol into the current scope
    Symbol* symbol = symbol_variable(declaration->identifier->lexeme, actual_type);
    
    // TODO(timo): Should we take the responsibility of declaring errors of
    // already diagnosed variables instead of doing it in the scope
    scope_declare(resolver->global, symbol);
}


void resolve_function_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    // TODO(timo): This is the place where we create the routines which has
    // the name of the routine, its parameters, its scope etc.
    //
    // TODO(timo): We should actually open and close the new scope in here considering our
    // implementation at this point
    //
    // NOTE(timo): So this type will be the type of the value assigned to the variable 
    // or if the declaration is function, this will be the type of the return value.
    Type* expected_type = resolve_type_specifier(resolver, declaration->specifier);
    Type* actual_type = resolve_expression(resolver, declaration->initializer);
    
    // Check if the expected type and the actual type match
    if (expected_type->kind != actual_type->function.return_type->kind)
    {
        // TODO(timo): Error
        printf("Conflicting types in function declaration\n");
        exit(1);
    }

    // Declare the symbol into the current scope
    Symbol* symbol = symbol_function(declaration->identifier->lexeme, actual_type);

    // TODO(timo): Should we take the responsibility of declaring errors of
    // already diagnosed variables instead of doing it in the scope
    scope_declare(resolver->global, symbol);
}


void resolve_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
            resolve_variable_declaration(resolver, declaration);
            break;
        case DECLARATION_FUNCTION:
            resolve_function_declaration(resolver, declaration);
            break;
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, declaration->position,
                ":RESOLVER - Unreachable: Invalid declaration");
            array_push(resolver->diagnostics, _diagnostic);
            break;
        }
    }
}


void resolve(Resolver* resolver, array* declarations)
{
    // TODO(timo): We probably should create a abstraction for the program itself
    // The program is probably not needed, but we still should handle the program
    // arguments somehow and it is probably best done in here
    for (int i = 0; i < declarations->length; i++)
    {
        resolve_declaration(resolver, declarations->items[i]);
    }
}
