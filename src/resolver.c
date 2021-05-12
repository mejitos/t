// Implementation for the resolver which does type checking and semantic
// checking for the abstract syntax tree.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"

#define INTEGER_MAX 2147483647
#define INTEGER_MIN 2147483648 // absolute value


void resolver_init(Resolver* resolver, hashtable* type_table)
{
    *resolver = (Resolver){ .global = scope_init(NULL, "global"),
                            .diagnostics = array_init(sizeof (Diagnostic*)),
                            .type_table = type_table,
                            .context.current_function = NULL,
                            .context.not_in_loop = true,
                            .context.not_in_function = true, 
                            .context.not_returned = true,
                            .context.return_type = NULL };

    resolver->local = resolver->global;
}


void resolver_free(Resolver* resolver)
{
    for (int i = 0; i < resolver->diagnostics->length; i++)
    {
        Diagnostic* diagnostic = resolver->diagnostics->items[i];

        free((char*)diagnostic->message);
        diagnostic->message = NULL;

        free(diagnostic);
        diagnostic = NULL;
    }

    array_free(resolver->diagnostics);

    // NOTE(timo): Scopes are being freed at the later stage since they
    // are needed for code generation
    
    // Now scopes are being freed in here for testing purpose
    scope_free(resolver->global);
    resolver->global = NULL;

    // NOTE(timo): The resolver itself is not being freed since it is
    // being initialized to the stack in the top level function
}


// Creates a new scope and assigns it as the current local scope.
//
// At the moment there is only two scopes in the language: global and functions
// local scope.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      name: Name of the new scope.
static inline void enter_scope(Resolver* resolver, const char* name)
{
    // TODO(timo): This should be removed after more flexible scoping is added
    assert(resolver->local == resolver->global);

    Scope* scope = scope_init(resolver->local, name);
    resolver->local = scope;
}


// Leaves current scope and sets the enclosing scope as the current local
// scope.
//
// At the moment there is only two scopes in the language: global and functions
// local scope.
//
// Arguments
//      resolver: Pointer to iniitalized Resolver.
static inline void leave_scope(Resolver* resolver)
{
    resolver->local = resolver->local->enclosing;

    // TODO(timo): This should be removed after more flexible scoping is added
    assert(resolver->local == resolver->global);
}


// Resolves type of a literal expression. Integer overflow of integer literal
// will also be checked.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_literal_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_LITERAL);
    
    Type* type;
    Token* literal = expression->literal;

    switch (literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        {
            const char* lexeme = literal->lexeme;
            uint64_t integer_value = 0;

            while (*lexeme != '\0') 
            { 
                // Converts ascii digit to corresponding number
                int digit = *lexeme - '0';
                integer_value *= 10; 
                
                // NOTE(timo): We can only check for the overflow of the literal in compile time, 
                // rest of the expressions are being evaluated and therefore we cannot know the value.
                // We just decieded that our maximum integer value is abs(2147483647)
                if (integer_value > INT_MAX - digit)
                {
                    Diagnostic* _diagnostic = diagnostic(
                        DIAGNOSTIC_ERROR, expression->position,
                        ":RESOLVER - OverflowError: Integer overflow in integer literal. Maximum integer value is abs(2147483647)");
                    array_push(resolver->diagnostics, _diagnostic);
                    break;
                }

                integer_value += digit;
                lexeme++;
            }
            
            type = hashtable_get(resolver->type_table, "int");
            expression->value = (Value){ .type = VALUE_INTEGER, 
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

            type = hashtable_get(resolver->type_table, "bool");
            expression->value = (Value){ .type = VALUE_BOOLEAN,
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

            type = hashtable_get(resolver->type_table, "none");
            break;
        }
    }

    expression->type = type;

    return type;
}


// Resolves type of a unary expression. If the unary expression is a literal,
// also integer overflows will be checked.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_unary_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_UNARY);

    Type* type;
    AST_Expression* operand = expression->unary.operand;
    Token* _operator = expression->unary._operator;
    Type* operand_type = resolve_expression(resolver, operand);

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
        {
            if (type_is_integer(operand_type))
            {
                if (value_is_integer(operand->value))
                {
                    if (operand->value.integer > INTEGER_MAX)
                    {
                        Diagnostic* _diagnostic = diagnostic(
                            DIAGNOSTIC_ERROR, expression->position,
                            ":RESOLVER - OverflowError: Integer overflow in integer literal. Maximum integer value is 2147483647");
                        array_push(resolver->diagnostics, _diagnostic);
                        break;
                    }
                    
                    expression->value = (Value){ .type = VALUE_INTEGER,
                                                 .integer = +operand->value.integer };
                }

                type = operand_type;
            }
            else
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary '+'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            break;
        }
        case TOKEN_MINUS:
        {
            if (type_is_integer(operand_type))
            {
                if (value_is_integer(operand->value))
                {
                    if (operand->value.integer > INTEGER_MIN)
                    {
                        Diagnostic* _diagnostic = diagnostic(
                            DIAGNOSTIC_ERROR, expression->position,
                            ":RESOLVER - OverflowError: Integer overflow in integer literal. Minimum integer value is -2147483648");
                        array_push(resolver->diagnostics, _diagnostic);
                        break;
                    }

                    expression->value = (Value){ .type = VALUE_INTEGER,
                                                 .integer = -operand->value.integer };
                }

                type = operand_type;
            }
            else
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary '-'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            break;
        }
        case TOKEN_NOT:
        {
            if (type_is_boolean(operand_type))
            {
                if (value_is_boolean(operand->value))
                    expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                 .boolean = ! operand->value.boolean };

                type = operand_type;
            }
            else
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary 'not'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, _operator->position,
                ":RESOLVER - Unreachable: Invalid unary operator '%s'",
                _operator->lexeme);
            array_push(resolver->diagnostics, _diagnostic);

            type = hashtable_get(resolver->type_table, "none");
            break;
        }
    }

    expression->type = type;

    return type;
}


// Resolves type of a binary expression.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_binary_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_BINARY);

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
        {
            if (type_is_not_integer(type_left) || type_is_not_integer(type_right))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            else
            {
                Value value_left = expression->binary.left->value;
                Value value_right = expression->binary.right->value;

                // If both values are integers => they are integer constants and
                // we can do some constant folding.
                if (value_is_integer(value_left) && value_is_integer(value_right))
                {
                    switch (_operator->kind)
                    {
                        case TOKEN_PLUS:
                        {
                            expression->value = (Value){ .type = VALUE_INTEGER,
                                                         .integer = value_left.integer + value_right.integer };
                            break;
                        }
                        case TOKEN_MINUS:
                        {
                            expression->value = (Value){ .type = VALUE_INTEGER,
                                                         .integer = value_left.integer - value_right.integer };
                            break;
                        }
                        case TOKEN_MULTIPLY:
                        {
                            expression->value = (Value){ .type = VALUE_INTEGER,
                                                         .integer = value_left.integer * value_right.integer };
                            break;
                        }
                        case TOKEN_DIVIDE:
                        {
                            expression->value = (Value){ .type = VALUE_INTEGER,
                                                         .integer = value_left.integer / value_right.integer };
                            break;
                        }
                    }
                }

                type = type_left;
            }

            break;
        }
        case TOKEN_IS_EQUAL:
        case TOKEN_NOT_EQUAL:
        {
            if ((type_is_integer(type_left) && type_is_not_integer(type_right)) ||
                (type_is_boolean(type_left) && type_is_not_boolean(type_right)))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                // TODO(timo): These none types create a lot of useless error messages in error situations
                // so consider just removing them
                type = hashtable_get(resolver->type_table, "none");
            }
            else
            {
                Value value_left = expression->binary.left->value;
                Value value_right = expression->binary.right->value;

                // If both values are booleans or both values are integers  => they 
                // are constants and we can do some constant folding.
                if (value_is_integer(value_left) && value_is_integer(value_right))
                {
                    switch (_operator->kind)
                    {
                        case TOKEN_IS_EQUAL:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.integer == value_right.integer};
                            break;
                        }
                        case TOKEN_NOT_EQUAL:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.integer != value_right.integer};
                            break;
                        }
                    }
                }
                else if (value_is_boolean(value_left) && value_is_boolean(value_right))
                {
                    switch (_operator->kind)
                    {
                        case TOKEN_IS_EQUAL:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.boolean == value_right.boolean };
                            break;
                        }
                        case TOKEN_NOT_EQUAL:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.boolean != value_right.boolean };
                            break;
                        }
                    }
                }

                type = hashtable_get(resolver->type_table, "bool");
            }

            break;
        }
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_EQUAL:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_EQUAL:
        {
            // Operand/values has to be scalar types e.g. integers in this 
            // case for the relational expression
            if (type_is_not_integer(type_left) || type_is_not_integer(type_right))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                // TODO(timo): These none types create a lot of useless error messages in error situations
                // so consider just removing them
                type = hashtable_get(resolver->type_table, "none");
            }
            else
            {
                Value value_left = expression->binary.left->value;
                Value value_right = expression->binary.right->value;

                // If both values are integers => they are integer constants and
                // we can do some constant folding.
                if (value_is_integer(value_left) && value_is_integer(value_right))
                {
                    switch (_operator->kind)
                    {
                        case TOKEN_LESS_THAN:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.integer < value_right.integer };
                            break;
                        }
                        case TOKEN_LESS_THAN_EQUAL:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.integer <= value_right.integer };
                            break;
                        }
                        case TOKEN_GREATER_THAN:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.integer > value_right.integer };
                            break;
                        }
                        case TOKEN_GREATER_THAN_EQUAL:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.integer >= value_right.integer };
                            break;
                        }
                    }
                }

                type = hashtable_get(resolver->type_table, "bool");
            }

            break;
        }
        case TOKEN_AND:
        case TOKEN_OR:
        {
            if (type_is_not_boolean(type_left) || type_is_not_boolean(type_right))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                // TODO(timo): These none types create a lot of useless error messages in error situations
                // so consider just removing them
                type = hashtable_get(resolver->type_table, "none");
            }
            else
            {
                Value value_left = expression->binary.left->value;
                Value value_right = expression->binary.right->value;

                // If both values are booleans => they are constants and we can do 
                // some constant folding.
                if (value_is_boolean(value_left) && value_is_boolean(value_right))
                {
                    switch (_operator->kind)
                    {
                        case TOKEN_AND:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.boolean && value_right.boolean };
                            break;
                        }
                        case TOKEN_OR:
                        {
                            expression->value = (Value){ .type = VALUE_BOOLEAN,
                                                         .boolean = value_left.boolean || value_right.boolean };
                            break;
                        }
                    }
                }

                type = type_left;
            }

            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, _operator->position,
                ":RESOLVER - Unreachable: Invalid binary operator '%s'",
                _operator->lexeme);
            array_push(resolver->diagnostics, _diagnostic);

            // TODO(timo): These none types create a lot of useless error messages in error situations
            // so consider just removing them
            type = hashtable_get(resolver->type_table, "none");
            break;
        }
    }

    expression->type = type;

    return type;
}


// Resolves type of a variable expression.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_variable_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_VARIABLE);

    Type* type;
    Symbol* symbol = scope_lookup(resolver->local, expression->identifier->lexeme);
     
    if (symbol == NULL)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - SyntaxError: Referencing identifier '%s' before declaring it",
            expression->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);
        
        // TODO(timo): These none types create a lot of useless error messages in error situations
        // so consider just removing them
        type = hashtable_get(resolver->type_table, "none");
    }
    else
        type = symbol->type;

    expression->type = type;

    return type;
}


// Resolves type of a assignment expression. Since assignment expression returns
// the value being assigned to a variable, the returned type can be either the
// type of the variable or the type of the value.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_assignment_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_ASSIGNMENT);

    AST_Expression* variable = expression->assignment.variable;
    AST_Expression* value = expression->assignment.value;
    Type* variable_type = resolve_expression(resolver, variable);
    Type* value_type = resolve_expression(resolver, value);
    
    if (types_not_equal(variable_type, value_type))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Conflicting types in assignment expression. Assigning to '%s' from '%s'",
            type_as_string(variable_type->kind), type_as_string(value_type->kind));
        array_push(resolver->diagnostics, _diagnostic);

        // NOTE(timo): We can return whatever type the value has, error or no error.
    }

    expression->type = value_type;

    return value_type;
}


// Resolves type of a index/subscript expression. The resolved type will be
// the type of the elements stored in the subscript target.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_index_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_INDEX);

    Type* type;
    AST_Expression* variable = expression->index.variable;
    Type* variable_type = resolve_expression(resolver, variable);
    Symbol* symbol = scope_lookup(resolver->local, variable->identifier->lexeme);

    // TODO(timo): Seriously refactor this mess

    // Make sure the subscript target is an array type
    if (type_is_not_array(variable_type))
    {
        // NOTE(timo): There is possibility that the variable is a whole lot of other things
        // than just a variable. It can be literal, function etc. so we cannot print just the 
        // identifier because it might not have one so we just print the type
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: '%s' is not subscriptable.",
            type_as_string(variable_type->kind));
        array_push(resolver->diagnostics, _diagnostic);

        // TODO(timo): These none types create a lot of useless error messages in error situations
        // so consider just removing them
        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }

    // TODO(timo): These checks for the argv and main are only for this stage of the 
    // compiler/language where we have arrays only for the argv of the main program
    // TODO(timo): Create str_not_equal() function
    // TODO(timo): str_equal is also a better name
    if ((symbol != NULL && ! str_equals(symbol->identifier, "argv")) || ! str_equals(resolver->local->name, "main"))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - Error: You cannot use index expressions with other values than argv of the main function");
        array_push(resolver->diagnostics, _diagnostic);

        // TODO(timo): These none types create a lot of useless error messages in error situations
        // so consider just removing them
        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }

    // Make sure that the type of the expression is integer
    Type* index_type = resolve_expression(resolver, expression->index.value);
    
    if (type_is_not_integer(index_type))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Array subscripts must be integers.",
            variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);

        // TODO(timo): These none types create a lot of useless error messages in error situations
        // so consider just removing them
        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }
    
    // TODO(timo): Boundary checks => the index cannot be < 0 and not > array length - 1
    // NOTE(timo): We cannot check the upper boundary at compile time since the arguments
    // for the argv is being passed at start of the compiled program
    Value index_value = expression->index.value->value;

    if (index_value.integer < 0)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - IndexError: Array subscript less than zero.",
            variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);

        // TODO(timo): These none types create a lot of useless error messages in error situations
        // so consider just removing them
        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }
    
    // TODO(timo): We probably should make sure that the elements of the array are integers
    // or the element types but that is not needed for now since we only have arrays for argv
    // which is handled at runtime only.

    // Return the type of the element in the array which the subscript accesses
    type = variable_type->array.element_type;
end:
    expression->type = type;

    return type;
}


// Resolves type of a function expression. The type will be the type of the
// value returned by the function.
// 
// NOTE(timo): We don't check the return type here, it is the callers responsibility.
// Expression just returns the value, or in this case, the type.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_function_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_FUNCTION);

    // TODO(timo): If we are in function at this point, error out.
    // if not in function -> resolve function -> return function type
    // else -> error -> return none type

    resolver->context.not_in_function = false;

    // Begin a new scope
    // TODO(timo): If anonymous functions are being supported, we
    // should check here whether the current function is NULL or not
    // TODO(timo): Other solution is to just simply open the scope
    // before entering this expression itself.
    enter_scope(resolver, resolver->context.current_function);

    Type* type = type_function();

    // Resolve parameters if there is some
    if (expression->function.arity > 0)
    {
        array* parameters = expression->function.parameters;

        for (int i = 0; i < parameters->length; i++)
        {
            Parameter* parameter = parameters->items[i];

            if (scope_get(resolver->local, parameter->identifier->lexeme) != NULL)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, parameter->position,
                    "RESOLVER - NameError: Redeclaration of identifier '%s' in '%s'",
                    parameter->identifier->lexeme, resolver->local->name);
                array_push(resolver->diagnostics, _diagnostic);
                // We can just skip iteration because we already have the identifier
                // declared -> future uses of the identifier won't mess things up.
                // TODO(timo): How to handle arity of the function in this case?
                // Just decrement the arity? That way we can at least avoid
                // unnecessary error messages.
                continue;
            }
            
            Type* parameter_type = resolve_type_specifier(resolver, parameter->specifier);
            Symbol* symbol = symbol_parameter(resolver->local, parameter->identifier->lexeme, parameter_type);

            scope_declare(resolver->local, symbol);
            array_push(type->function.parameters, symbol);
        }

        type->function.arity = type->function.parameters->length;
    }

    // Resolve body and return type
    resolve_statement(resolver, expression->function.body);

    // Function has to return value
    if (resolver->context.not_returned)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, (Position){0},
            ":RESOLVER - SyntaxError: Function has to return a value.");
        array_push(resolver->diagnostics, _diagnostic);
    }
    
    // Set the functions scope to the created local scope
    type->function.scope = resolver->local;
    // NOTE(timo): Decided to force only single return statement per function, so 
    // this can actually return something. Even though this information could 
    // carry inside the context structure.
    // NOTE(timo): We should check that the function actually returned something,
    // but that check is made in resolve_function_declaration()
    type->function.return_type = resolver->context.return_type;

    // End scope
    leave_scope(resolver);

    // Reset the context
    resolver->context.not_in_function = true;
    resolver->context.return_type = NULL;
    resolver->context.not_returned = true;

    expression->type = type;

    return type;
}


// Resolves type of a call expression. The resolved type will be the type of
// the value returned by the called function/callable.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      Pointer to resolved type of the expression.
static Type* resolve_call_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_CALL);

    Type* type = resolve_expression(resolver, expression->call.variable);
    
    // TODO(timo): Refactor this to be in line with other functions and 
    // have only one return statement at the end.
    // Make sure the called variable is actually callable - a function in our case
    if (type_is_function(type))
    {
        array* arguments = expression->call.arguments;
        Symbol* symbol = scope_lookup(resolver->local, expression->call.variable->identifier->lexeme);

        // Number of arguments == arity of the called function
        if (symbol->type->function.arity != arguments->length)
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, expression->position,
                ":RESOLVER - TypeError: Function '%s' expected %d arguments, but %d was given\n", 
                symbol->identifier, symbol->type->function.arity, arguments->length);
            array_push(resolver->diagnostics, _diagnostic);
        }
        
        // Types of the arguments == types of the parameters
        for (int i = 0; i < arguments->length; i++)
        {
            AST_Expression* argument = (AST_Expression*)arguments->items[i];
            Symbol* parameter = symbol->type->function.parameters->items[i];

            Type* argument_type = resolve_expression(resolver, argument);
            Type* parameter_type = parameter->type;

            if (types_not_equal(argument_type, parameter_type))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Parameter '%s' is of type '%s', but argument of type '%s' was given.", 
                    parameter->identifier, type_as_string(parameter_type->kind), type_as_string(argument_type->kind));
                array_push(resolver->diagnostics, _diagnostic);
            }
        }
        
        // TODO(timo): Is this the correct type?
        expression->type = type->function.return_type;

        // return the return type of the called function
        return type->function.return_type;
    }
    else
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: '%s' is not callable.",
            expression->call.variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);

        return hashtable_get(resolver->type_table, "none");
    }
}


Type* resolve_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;

    // TODO(timo): At this point we could check if the expression is being
    // declared in global scope. If it is => error. Only declarations
    // allowed in the global scope.

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

            type = hashtable_get(resolver->type_table, "none");
            break;
        }
    }
    
    // TODO(timo): Could we just set the expression type in here? We probably
    // should, so we don't have to set the type anywhere else.
    // TODO(timo): Also if we used the Operand structure we created before,
    // we could just return the operand and take the values of the operand
    // and set it's type and value as the type and value of the expression.
    // TODO(timo): Third option could be just to make each function type of
    // void, since we are just decorating the expressions, we don't actually
    // have to return anything.

    return type;
}


// Resolves each statement in a block statement.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
static void resolve_block_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_BLOCK);

    // TODO(timo): enter the scope of the block

    array* statements = statement->block.statements;
    
    for (int i = 0; i < statements->length; i++)
        resolve_statement(resolver, statements->items[i]); 

    // TODO(timo): leave the scope of the block
}


// Resolves a if statement.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
static void resolve_if_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_IF);

    Type* condition = resolve_expression(resolver, statement->_if.condition);

    if (type_is_not_boolean(condition))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->_if.condition->position,
            ":RESOLVER - TypeError: Conditional expression must produce boolean value");
        array_push(resolver->diagnostics, _diagnostic);
    }

    resolve_statement(resolver, statement->_if.then);

    if (statement->_if._else) 
        resolve_statement(resolver, statement->_if._else);
}


// Resolves a while statement.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
static void resolve_while_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_WHILE);

    Type* condition = resolve_expression(resolver, statement->_while.condition);

    if (type_is_not_boolean(condition))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->_while.condition->position,
            ":RESOLVER - TypeError: Conditional expression must produce boolean value");
        array_push(resolver->diagnostics, _diagnostic);
    }

    resolver->context.not_in_loop = false;
    resolve_statement(resolver, statement->_while.body);
    resolver->context.not_in_loop = true;
}


// Resolves a return statement.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
static void resolve_return_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_RETURN);

    if (resolver->context.not_in_function)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->position,
            ":RESOLVER - SyntaxError: Can't return outside of functions");
        array_push(resolver->diagnostics, _diagnostic);
    }
    
    if (! resolver->context.not_returned)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->position,
            ":RESOLVER - SyntaxError: You can only return once from function");
        array_push(resolver->diagnostics, _diagnostic);
    }
    
    // NOTE(timo): Since we are in return statement, we have something to return
    // and for now explicit returning without value is not possible.
    // TODO(timo): There must be a better way
    // Returns statement must return a value
    if (statement->_return.value && statement->_return.value->kind != EXPRESSION_NONE)
    {
        resolver->context.return_type = resolve_expression(resolver, statement->_return.value);
        resolver->context.not_returned = false;
    }
    else
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->position,
            ":RESOLVER - SyntaxError: Return statement must return a value.");
        array_push(resolver->diagnostics, _diagnostic);

        resolver->context.return_type = hashtable_get(resolver->type_table, "none");
    }
}


// Resolves a break statement.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
static void resolve_break_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_BREAK);

    if (resolver->context.not_in_loop)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->position,
            ":RESOLVER - SyntaxError: Cant't break outside of loops.");
        array_push(resolver->diagnostics, _diagnostic);
    }
}


// Resolves a continue statement.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
static void resolve_continue_statement(Resolver* resolver, AST_Statement* statement)
{
    assert(statement->kind == STATEMENT_CONTINUE);

    if (resolver->context.not_in_loop)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->position,
            ":RESOLVER - SyntaxError: Cant't continue outside of loops.");
        array_push(resolver->diagnostics, _diagnostic);
    }
}


void resolve_statement(Resolver* resolver, AST_Statement* statement)
{
    // TODO(timo): At this point we could check if the statement is being
    // declared in global scope. If it is => error. Only declarations
    // allowed in the global scope.

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
            resolve_break_statement(resolver, statement);
            break;
        case STATEMENT_CONTINUE:
            resolve_continue_statement(resolver, statement);
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
    Type* type;

    switch (specifier)
    {
        case TYPE_SPECIFIER_INT:
        {
            type = hashtable_get(resolver->type_table, "int");
            break;
        }
        case TYPE_SPECIFIER_BOOL:
        {
            type = hashtable_get(resolver->type_table, "bool");
            break;
        }
        case TYPE_SPECIFIER_ARRAY_INT:
        {
            type = type_array(hashtable_get(resolver->type_table, "int"));
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, (Position) {0},
                ":RESOLVER - Unreachable: Invalid type specifier");
            array_push(resolver->diagnostics, _diagnostic);

            type = hashtable_get(resolver->type_table, "none");
            break;
        }
    }
    
    return type;
}


// Resolves a variable declaration by making sure the intended and actual
// actual resolved types matches. If the variable is global, it is also made
// sure the initializer is a compile-time constant. This is behaviour taken
// from C language and is being used for now. At the end, symbol will be
// declared into the current local scope.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      declaration: Declaration to be resolved.
static void resolve_variable_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    assert(declaration->kind == DECLARATION_VARIABLE);

    // Check if the identifier already exists in the local scope.
    if (scope_get(resolver->local, declaration->identifier->lexeme) != NULL)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, declaration->position,
            "RESOLVER - NameError: Redeclaration of identifier '%s' in '%s'",
            declaration->identifier->lexeme, resolver->local->name);
        array_push(resolver->diagnostics, _diagnostic);

        // We can just return since the identifier is already declared so in
        // future uses of the identifier won't mess things up.
        return;
    }

    Type* expected_type = resolve_type_specifier(resolver, declaration->specifier);
    Type* actual_type = resolve_expression(resolver, declaration->initializer);

    // The variables initializer has to be a constant literal. At this point we 
    // can check that by checking if the value is none or not after resolving it.
    Value initializer_value = declaration->initializer->value;

    if (resolver->local == resolver->global && value_is_none(initializer_value))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, declaration->position,
            ":RESOLVER - Error: Global variable initializer has to be compile-time constant.");
        array_push(resolver->diagnostics, _diagnostic);
    }

    if (types_not_equal(actual_type, expected_type))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, declaration->position,
            ":RESOLVER - TypeError: Conflicting types in variable declaration. Declaring type '%s' to '%s'",
            type_as_string(actual_type->kind), type_as_string(expected_type->kind));
        array_push(resolver->diagnostics, _diagnostic);
    }

    Symbol* symbol = symbol_variable(resolver->local, declaration->identifier->lexeme, actual_type);
    // We have at least a none value in every expression.
    symbol->value = declaration->initializer->value;
    scope_declare(resolver->local, symbol);
}


// Resolves a function declaration.
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      declaration: Declaration to be resolved.
static void resolve_function_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    assert(declaration->kind == DECLARATION_FUNCTION);

    // Check if the identifier already exists in the local scope.
    if (scope_get(resolver->local, declaration->identifier->lexeme) != NULL)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, declaration->position,
            "RESOLVER - NameError: Redeclaration of identifier '%s' in '%s'",
            declaration->identifier->lexeme, resolver->local->name);
        array_push(resolver->diagnostics, _diagnostic);

        // We can just return since the identifier is already declared so in
        // future uses of the identifier won't mess things up.
        return;
    }

    // Set the current context to the context of the function
    resolver->context.current_function = (char*)declaration->identifier->lexeme;

    Type* expected_type = resolve_type_specifier(resolver, declaration->specifier);
    Type* actual_type = resolve_expression(resolver, declaration->initializer);

    // The initializer has to be a function type
    if (type_is_not_function(actual_type))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, declaration->position,
            ":RESOLVER - TypeError: Expected a function expression in initializer of function declaration, got '%s'.",
            type_as_string(actual_type->kind));
        array_push(resolver->diagnostics, _diagnostic);
    }

    // Function has to return a value. If this is true, the function does not
    // have a return statement at all.
    if (! actual_type->function.return_type)
    {
        // NOTE(timo): The diagnostic is added in the resolve_function_expression
        // TODO(timo): Decide how to handle this in here.
    }
    // Check if the expected type and the actual type match
    else if (types_not_equal(actual_type->function.return_type, expected_type) && 
             type_is_not_none(actual_type->function.return_type))
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, declaration->position,
            ":RESOLVER - TypeError: Conflicting types in function declaration. Declaring type '%s' to '%s'",
            type_as_string(actual_type->function.return_type->kind), type_as_string(expected_type->kind));
        array_push(resolver->diagnostics, _diagnostic);
    }
    
    // Declare the symbol into the current scope
    Symbol* symbol = symbol_function(resolver->local, declaration->identifier->lexeme, actual_type);

    // TODO(timo): Should we take the responsibility of declaring errors of
    // already declared variables instead of doing it in the scope
    // TODO(timo): Check for redeclaration of the variable
    scope_declare(resolver->local, symbol);

    // Reset the context
    resolver->context.current_function = NULL;
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
    for (int i = 0; i < declarations->length; i++)
        resolve_declaration(resolver, declarations->items[i]);
}
