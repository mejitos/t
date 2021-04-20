#include "t.h"

#define INTEGER_MAX 2147483647;
#define INTEGER_MIN 2147483648; // absolute value


void resolver_init(Resolver* resolver, hashtable* type_table)
{
    *resolver = (Resolver){ .global = scope_init(NULL, "global"),
                            .diagnostics = array_init(sizeof (Diagnostic*)),
                            .type_table = type_table,
                            .context.not_in_loop = true,
                            .context.not_in_function = true, };

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


static inline void enter_scope(Resolver* resolver, const char* name)
{
    // NOTE(timo): local scope at this point should be the global scope
    assert(resolver->local == resolver->global);

    Scope* scope = scope_init(resolver->local, name);
    resolver->local = scope;
}


static inline void leave_scope(Resolver* resolver)
{
    // resolver->local = resolver->global;
    resolver->local = resolver->local->enclosing;
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

                    // TODO(timo): Return none type?
                    break;
                }

                integer_value += digit;
                lexeme++;
            }
            
            type = hashtable_get(resolver->type_table, "int");
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

            type = hashtable_get(resolver->type_table, "bool");
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

            // TODO(timo): Return none type?
            break;
        }
    }

    expression->type = type;
    expression->value = value;

    return type;
}


static Type* resolve_unary_expression(Resolver* resolver, AST_Expression* expression)
{
    assert(expression->kind == EXPRESSION_UNARY);

    AST_Expression* operand = expression->unary.operand;
    Token* _operator = expression->unary._operator;
    Type* operand_type = resolve_expression(resolver, operand);

    switch (_operator->kind)
    {
        case TOKEN_PLUS:
        {
            if (operand_type->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary '+'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);

                // TODO(timo): Return none type?
                break;
            }
            /*
            if (operand->kind == EXPRESSION_LITERAL && operand->value.integer > INTEGER_MAX)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - OverflowError: Integer overflow in integer literal. Maximum integer value is 2147483647");
                array_push(resolver->diagnostics, _diagnostic);
                break;
            }
            */
            // TODO(timo): Do we need values to every expression or do we just keep them in literal and unary?
            expression->value = operand->value;
            expression->value.integer = +expression->value.integer;
            break;
        }
        case TOKEN_MINUS:
        {
            if (operand_type->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary '-'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);

                // TODO(timo): Return none type?
                break;
            }
            /*
            if (operand->kind == EXPRESSION_LITERAL && operand->value.integer > INTEGER_MIN)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - OverflowError: Integer overflow in integer literal. Minimum integer value is 2147483648");
                array_push(resolver->diagnostics, _diagnostic);
                break;
            }
            */
            // TODO(timo): Do we need values to every expression or do we just keep them in literal and unary?
            expression->value = operand->value;
            expression->value.integer = -expression->value.integer;
            break;
        }
        case TOKEN_NOT:
        {
            if (operand_type->kind != TYPE_BOOLEAN)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand type '%s' for unary 'not'",
                    type_as_string(operand_type->kind));
                array_push(resolver->diagnostics, _diagnostic);

                // TODO(timo): Return none type?
            }
            // TODO(timo): Do we need values to every expression or do we just keep them in literal and unary?
            expression->value = operand->value;
            expression->value.integer = !expression->value.boolean;
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, _operator->position,
                ":RESOLVER - Unreachable: Invalid unary operator '%s'",
                _operator->lexeme);
            array_push(resolver->diagnostics, _diagnostic);

            // TODO(timo): Return none type?
            break;
        }
    }

    expression->type = operand_type;

    return operand_type;
}


static Type* resolve_binary_expression(Resolver* resolver, AST_Expression* expression)
{
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
            if (type_left->kind != TYPE_INTEGER || type_right->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            else
                type = type_left;

            break;
        }
        case TOKEN_IS_EQUAL:
        case TOKEN_NOT_EQUAL:
        {
            if ((type_left->kind == TYPE_INTEGER && type_right->kind != TYPE_INTEGER) || 
                (type_left->kind == TYPE_BOOLEAN && type_right->kind != TYPE_BOOLEAN))
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            else
                type = hashtable_get(resolver->type_table, "bool");

            break;
        }
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_EQUAL:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_EQUAL:
        {
            // TODO(timo): Operand/values has to be scalar types e.g. integers
            // in this case for the ordering expression
            if (type_left->kind != TYPE_INTEGER || type_right->kind != TYPE_INTEGER)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            else
                type = hashtable_get(resolver->type_table, "bool");

            break;
        }
        case TOKEN_AND:
        case TOKEN_OR:
        {
            if (type_left->kind != TYPE_BOOLEAN || type_right->kind != TYPE_BOOLEAN)
            {
                Diagnostic* _diagnostic = diagnostic(
                    DIAGNOSTIC_ERROR, expression->position,
                    ":RESOLVER - TypeError: Unsupported operand types '%s' and '%s' for binary '%s'",
                    type_as_string(type_left->kind), type_as_string(type_right->kind), _operator->lexeme);
                array_push(resolver->diagnostics, _diagnostic);

                type = hashtable_get(resolver->type_table, "none");
            }
            else
                type = type_left;

            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, _operator->position,
                ":RESOLVER - Unreachable: Invalid binary operator '%s'",
                _operator->lexeme);
            array_push(resolver->diagnostics, _diagnostic);

            type = hashtable_get(resolver->type_table, "none");
            break;
        }
    }

    expression->type = type;

    return type;
}


static Type* resolve_variable_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;
    Symbol* symbol = scope_lookup(resolver->local, expression->identifier->lexeme);
     
    if (symbol == NULL)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - SyntaxError: Referencing identifier '%s' before declaring it",
            expression->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);
        
        type = hashtable_get(resolver->type_table, "none");
    }
    else
        type = symbol->type;

    expression->type = type;

    return type;
}


static Type* resolve_assignment_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;
    Type* variable_type = resolve_expression(resolver, expression->assignment.variable);
    Type* value_type = resolve_expression(resolver, expression->assignment.value);
    
    if (variable_type->kind != value_type->kind)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Conflicting types in assignment expression. Assigning to '%s' from '%s'",
            type_as_string(variable_type->kind), type_as_string(value_type->kind));
        array_push(resolver->diagnostics, _diagnostic);

        type = hashtable_get(resolver->type_table, "none");
    }
    else
        type = value_type;

    expression->type = type;

    return type;
}


/*
 *  Returns the type of the element of the array that is being subscripted
 *  I think that is the correct value?
 */
static Type* resolve_index_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type;
    AST_Expression* variable = expression->index.variable;
    Type* variable_type = resolve_expression(resolver, variable);
    Symbol* symbol = scope_lookup(resolver->local, variable->identifier->lexeme);

    // Make sure the subscript target is an array type
    if (variable_type->kind != TYPE_ARRAY)
    {
        // NOTE(timo): There is possibility that the variable is a whole lot of other things
        // than just a variable. It can be literal, function etc. so we cannot print just the 
        // identifier because it might not have one so we just print the type
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: '%s' is not subscriptable.",
            type_as_string(variable_type->kind));
        array_push(resolver->diagnostics, _diagnostic);

        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }

    // TODO(timo): These checks for the argv and main are only for this stage of the 
    // compiler/language where we have arrays only for the argv of the main program
    if ((symbol != NULL && strcmp(symbol->identifier, "argv") != 0) || strcmp(resolver->local->name, "main") != 0)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - Error: You cannot use index expressions with other values than argv of the main function");
        array_push(resolver->diagnostics, _diagnostic);

        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }

    // Make sure that the type of the expression is integer
    Type* index_type = resolve_expression(resolver, expression->index.value);
    
    if (index_type->kind != TYPE_INTEGER)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, expression->position,
            ":RESOLVER - TypeError: Array subscripts must be integers.",
            variable->identifier->lexeme);
        array_push(resolver->diagnostics, _diagnostic);

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

        type = hashtable_get(resolver->type_table, "none");
        goto end;
    }
    

    // TODO(timo): The array size should be gotten from initializing the array but since
    // we have the arrays only for the argv, they are resolved at runtime
    // TODO(timo): We don't have a way to get the value of the argc for now
    /*
    if (index_value.integer > "array capacity - 1")
    {
        // TODO(timo): Error
        printf("Array subscript greater than array length\n");
        exit(1);
    }
    */
    
    // TODO(timo): We prbably should make sure that the elements of the array are integers
    // or the element types but that is not needed for now since we only have arrays for argv

    // Return the type of the element in the array which the subscript accesses
    type = variable_type->array.element_type;
end:
    expression->type = type;

    return type;
}


static Type* resolve_function_expression(Resolver* resolver, AST_Expression* expression)
{
    // TODO(timo): If we are in function at this point, error out.
    // if not in function -> resolve function -> return function type
    // else -> error -> return none type

    resolver->context.not_in_function = false;

    // Begin a new scope
    // TODO(timo): If anonymous functions are being supported, we
    // should check here whether the current function is NULL or not
    enter_scope(resolver, resolver->context.current_function);

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
            scope_declare(resolver->local, symbol);
            array_push(type->function.parameters, symbol);
        }

        type->function.arity = type->function.parameters->length;
    }

    // Resolve body and return type
    resolve_statement(resolver, expression->function.body);
    
    // Set the functions scope to created local scope
    type->function.scope = resolver->local;

    // End scope
    leave_scope(resolver);
    resolver->context.not_in_function = true;

    // NOTE(timo): Decided to force only single return statement per function, so this can actually 
    // return something. Even though this information could carry inside the context structure.
    type->function.return_type = resolver->context.return_type;

    expression->type = type;

    return type;
}


static Type* resolve_call_expression(Resolver* resolver, AST_Expression* expression)
{
    Type* type = resolve_expression(resolver, expression->call.variable);
    array* arguments = expression->call.arguments;
    Symbol* symbol = scope_lookup(resolver->local, expression->call.variable->identifier->lexeme);

    // Make sure the called variable is actually callable - a function in our case
    if (type->kind == TYPE_FUNCTION)
    {
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
            symbol->identifier);
        array_push(resolver->diagnostics, _diagnostic);
        // TODO(timo): return type none?
        return hashtable_get(resolver->type_table, "none");
    }
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
    
    // TODO(timo): Could we just set the expression type in here?

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
    resolver->context.return_type = resolve_expression(resolver, statement->_return.value);
}


void resolve_break_statement(Resolver* resolver, AST_Statement* statement)
{
    if (resolver->context.not_in_loop)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, statement->position,
            ":RESOLVER - SyntaxError: Cant't break outside of loops.");
        array_push(resolver->diagnostics, _diagnostic);
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
            resolve_break_statement(resolver, statement);
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


void resolve_variable_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    // NOTE(timo): So this type will be the type of the value assigned to the variable 
    // or if the declaration is function, this will be the type of the return value.
    Type* expected_type = resolve_type_specifier(resolver, declaration->specifier);
    Type* actual_type = resolve_expression(resolver, declaration->initializer);

    // TODO(timo): If the resolved type is a function, we have to compare the type
    // to the return type of the function
    if (expected_type->kind != actual_type->kind)
    {
        // TODO(timo): Error
        printf("Conflicting types when declaring variable '%s'\n", declaration->identifier->lexeme);
        exit(1);
    }

    // Declare the symbol into the current scope
    Symbol* symbol = symbol_variable(declaration->identifier->lexeme, actual_type);
    
    // TODO(timo): Set value for a global variable, but this solution kinda sucks
    // just a hacky solution to get things going for now
    if (strcmp(resolver->local->name, "global") == 0)
        symbol->value = declaration->initializer->value;
    
    // TODO(timo): Should we take the responsibility of declaring errors of
    // already diagnosed variables instead of doing it in the scope
    scope_declare(resolver->local, symbol);
}


void resolve_function_declaration(Resolver* resolver, AST_Declaration* declaration)
{
    // TODO(timo): This is the place where we create the routines which has
    // the name of the routine, its parameters, its scope etc.

    // TODO(timo): We should actually open and close the new scope in here considering our
    // implementation at this point
    
    // Set the current context to the context of the function
    resolver->context.current_function = (char*)declaration->identifier->lexeme;

    // NOTE(timo): So this type will be the type of the value assigned to the variable 
    // or if the declaration is function, this will be the type of the return value.
    Type* expected_type = resolve_type_specifier(resolver, declaration->specifier);

    // Declare the scope here so we can set the scope into the symbol
    Type* actual_type = resolve_expression(resolver, declaration->initializer);
   
    // The type has to be a function type
    if (actual_type->kind != TYPE_FUNCTION)
    {
        printf("Expected function type, got something else\n");
        exit(1);
    }

    // Check if the expected type and the actual type match
    // TODO(timo): Dont know yet if this is the right thing to do, since in principal
    // the function should return something and it should be the correct type so
    // this kind of disregarding the "none" type is probably not good. For now it will
    // be just a resolving-stage type only so this is somewhat justified
    if (expected_type->kind != actual_type->function.return_type->kind && actual_type->function.return_type->kind != TYPE_NONE)
    {
        // TODO(timo): Error
        printf("Conflicting types in function declaration\n");
        exit(1);
    }
    
    // Declare the symbol into the current scope
    Symbol* symbol = symbol_function(declaration->identifier->lexeme, actual_type);

    // TODO(timo): Should we take the responsibility of declaring errors of
    // already diagnosed variables instead of doing it in the scope
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
    // TODO(timo): We probably should create a abstraction for the program itself
    // The program is probably not needed, but we still should handle the program
    // arguments somehow and it is probably best done in here
    for (int i = 0; i < declarations->length; i++)
    {
        resolve_declaration(resolver, declarations->items[i]);
    }
}
