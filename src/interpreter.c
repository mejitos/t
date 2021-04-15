#include "t.h"


// void interpreter_init(Interpreter* interpreter)
void interpreter_init(Interpreter* interpreter, Scope* global)
{
    // *interpreter = (Interpreter) { .global = scope_init(NULL, "global") };
    *interpreter = (Interpreter) { .global = global };

    interpreter->local = interpreter->global;
}


static Value evaluate_literal_expression(AST_Expression* expression)
{
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
    interpreter->return_value = evaluate_expression(interpreter, statement->_return.value);
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
    // TODO(timo): Since we are interested in different things here in interpreter,
    // we can't just create the symbols in the same way as in resolver
    // Symbol* symbol = symbol_variable(declaration);
    Symbol* symbol = symbol_variable(declaration->identifier->lexeme, NULL);
    symbol->value = evaluate_expression(interpreter, declaration->initializer);

    scope_declare(interpreter->global, symbol);
}


static void evaluate_function_declaration(Interpreter* interpreter, AST_Declaration* declaration)
{
    assert(declaration->kind == DECLARATION_FUNCTION);

    // Declare the identifier in the current scope
    // Name collisions should've been handled by the resolver
    // NOTE(timo): The function is already defined

    // Begin a new scope for the function
    // NOTE(timo): Switch scope

    // Add the variables into the scope
    // NOTE(timo): Parameters and variables are already in the scope

    // Evaluate the body
    AST_Statement* body = declaration->initializer->function.body;
    for (int i = 0; i < body->block.statements->length; i++)
        evaluate_statement(interpreter, body->block.statements->items[i]);
}


// TODO(timo): Since we have already gone through the declarations
// there might not even be a need for evaluating declarations
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


// TODO(timo): Take arguments and options
// void interpret(const char* source)
Value interpret(const char* source)
{
    // Setup
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    Interpreter interpreter;
    
    // Lexing
    lexer_init(&lexer, source);
    lex(&lexer);

    if (lexer.diagnostics->length > 0)
    {
        print_diagnostics(lexer.diagnostics);
        goto teardown_lexer;
    }

    // Parsing
    parser_init(&parser, lexer.tokens);
    parse(&parser);

    if (parser.diagnostics->length > 0)
    {
        print_diagnostics(parser.diagnostics);
        goto teardown_parser;
    }

    // Resolving
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    if (resolver.diagnostics->length > 0)
    {
        print_diagnostics(resolver.diagnostics);
        goto teardown;
    }
    
    // Evaluate
    interpreter_init(&interpreter, resolver.global);
    // TODO(timo): Create a program struct and evaluate it?
    // NOTE(timo): At this point we should handle the arguments and options
    // Then we should just evaluate the body of the program
    // evaluate_declaration(&interpreter, program);
    Symbol* main = scope_lookup(resolver.global, "main");
    interpreter.local = main->type->function.scope;

    AST_Declaration* program = parser.declarations->items[parser.declarations->length - 1];
    AST_Statement* body = program->initializer->function.body;

    for (int i = 0; i < body->block.statements->length; i++)
        evaluate_statement(&interpreter, body);

    // TODO(timo): Get and print the return value of the program
    Value return_value = interpreter.return_value;

    /*
    if (return_value.type == VALUE_INTEGER)
        printf("Program exited with the value %d\n", return_value.integer);
    if (return_value.type == VALUE_BOOLEAN)
        printf("Program exited with the value %s\n", return_value.boolean ? "true" : "false");
    */
   
    // Teardown
teardown:
    resolver_free(&resolver);
    type_table_free(type_table);
teardown_parser:
    parser_free(&parser);
teardown_lexer:
    lexer_free(&lexer);

    return return_value;
}


// TODO(timo): Take arguments and options
// void interpret_from_file(const char* path)
Value interpret_from_file(const char* path)
{
    FILE* file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Could not open file '%s'\n", path);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char* buffer = malloc(file_size * sizeof (char) + 1);
    
    if (buffer == NULL)
    {
        printf("Malloc failed. Not enough memory to read file '%s'\n", path);
        exit(1);
    }

    size_t bytes_read = fread(buffer, sizeof (char), file_size, file);

    if (bytes_read < file_size)
    {
        printf("Could not read file '%s'\n", path);
        exit(1);
    }

    buffer[bytes_read] = 0;
    fclose(file);
    
    Value return_value = interpret(buffer);

    free(buffer);

    return return_value;
}
