// Implementation of parser which creates an abstract syntax tree from the 
// token stream created by the lexer.
//
// At the moment there is a big problem with functions and their implementation
// as expressions. The parsing without context became a huge obstacle because
// the parenthesized expression can be pretty much identical to the parameter
// list of the function. The separation between the two and especially the
// error handling between the two became big headache. Wasted 4 days for just
// that thing alone without any decent solutions.
//
// Also the error recovery is pretty bad because it is only done at statement
// and declaration level. This created big problems especially when parsing
// the current function expressions.
//
// TODO(timo): The positions for all of the AST nodes should be set properly
// because now error messages are faulty when it comes to positions of some
// statements.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


static AST_Statement* parse_block_statement(Parser* parser);
static AST_Statement* parse_return_statement(Parser* parser);
static inline void advance(Parser* parser);


void parser_init(Parser* parser, array* tokens)
{
    *parser = (Parser){ .tokens = tokens,
                        .index = 0,
                        .position.line_end = 1,
                        .position.column_end = 1,
                        .diagnostics = array_init(sizeof (Diagnostic*)),
                        .declarations = array_init(sizeof (AST_Declaration*)) };

    // NOTE(timo): This sets the initial first token as current token 
    // and advances the pointer for the peek function
    advance(parser);
}


void parser_free(Parser* parser)
{
    for (int i = 0; i < parser->diagnostics->length; i++)
    {
        Diagnostic* diagnostic = parser->diagnostics->items[i];

        free((char*)diagnostic->message);
        diagnostic->message = NULL;

        free(diagnostic);
        diagnostic = NULL;
    }

    // NOTE(timo): The array itself is set to NULL in the function
    array_free(parser->diagnostics);

    for (int i = 0; i < parser->declarations->length; i++)
    {
        declaration_free(parser->declarations->items[i]);
        parser->declarations->items[i] = NULL;
    }
    
    // NOTE(timo): The array itself is set to NULL in the function
    array_free(parser->declarations);

    // NOTE(timo): Tokens are being freed by the lexer and we dont 
    // free the parser since it is being initialized to the stack
    // at the top level function
}


// Error recovery for parser if errors are found. The recovery is very basic
// algorithm where we just skip to next statement or declaration from the point
// the error was spotted from. Panic mode ends when a token for starting new 
// statement or declaration is being found and parsing continues from that point on.
//
// TODO(timo): At the moment this doesn't actually even work the way I intended
// it to work, since the recovery is always done after the faulty declaration or
// statement is parsed all the way through. Therefore the recovery will happen
// too late.
//
// Arguments
//      parser: Pointer to initialized Parser.
static void panic_mode(Parser* parser)
{
    Token_Kind kind = parser->current_token->kind;

    do {
        if (kind == TOKEN_SEMICOLON)
            advance(parser);
        if (kind == TOKEN_IDENTIFIER || kind == TOKEN_IF    || kind == TOKEN_WHILE ||
            kind == TOKEN_RETURN     || kind == TOKEN_BREAK || kind == TOKEN_CONTINUE ||
            kind == TOKEN_LEFT_CURLYBRACE || TOKEN_EOF)
            break;
        else
            advance(parser);
    } while ((kind = parser->current_token->kind) != TOKEN_EOF);

    parser->panic = false;
}


// Checks if the current token is intended and creates a diagnostic if it is not.
//
// Arguments
//      parser: Initialized parser.
//      kind: Expected token kind.
//      lexeme: Lexeme of the expected token kind.
//      panic: If the error recovery should be done or not.
static void expect_token(Parser* parser, const Token_Kind kind, const char* lexeme, const bool panic)
{
    // NOTE(timo): The boolean 'panic' tells whether it is time to panic or not.
    // There is a lot of situations where that isn't actually necessary e.g.
    // when parsing while statement and the 'do' keyword is missing. That is
    // not so critical mistake, that actual error recovery is needed.
    // TODO(timo): Create better solution or at least go through with care all
    // the situations where the error recovery is really not needed.
    if (parser->current_token->kind != kind)
    {
        Diagnostic* _diagnostic = diagnostic(
            DIAGNOSTIC_ERROR, parser->current_token->position, 
            ":PARSER - SyntaxError: Invalid token '%s', expected '%s'", 
            parser->current_token->lexeme, lexeme);
        array_push(parser->diagnostics, _diagnostic); 
        parser->panic = panic;
    }
    else
        advance(parser);
}


// Checks the next token without advancing the pointer.
//
// Arguments
//      parser: Pointer to a initialized Parser.
// Returns
//      Pointer to a next token in line.
static inline const Token* peek(const Parser* parser)
{
    // NOTE(timo): Since the current token is always advanced after assigning it,
    // the peek needs to return the current pointer and not current + 1.
    return (Token*)parser->tokens->items[parser->index];
}


// Advances the token stream by setting the current token as the next token
// in line and by advancing the index.
//
// Arguments
//      parser: Pointer to a initialized Parser.
static inline void advance(Parser* parser)
{
    // NOTE(timo): This check is needed to make sure there actually is a 
    // current token all the time, at least the EoF token
    if (parser->index < parser->tokens->length)
        parser->current_token = (Token*)parser->tokens->items[parser->index++];
}


Type_Specifier parse_type_specifier(Parser* parser)
{
    Type_Specifier specifier;

    switch (parser->current_token->kind)
    {
        case TOKEN_INT:
        {
            advance(parser);
            specifier = TYPE_SPECIFIER_INT;
            break;
        }
        case TOKEN_BOOL:
        {
            advance(parser);
            specifier = TYPE_SPECIFIER_BOOL;
            break;
        }
        case TOKEN_LEFT_BRACKET:
        {
            advance(parser);
            // NOTE(timo): Since we will only have arrays of int at this point
            // of time, we will force it here at the parsing stage already
            // TODO(timo): This needs more descriptive error message though
            expect_token(parser, TOKEN_INT, "int", true);
            expect_token(parser, TOKEN_RIGHT_BRACKET, "]", true);

            specifier = TYPE_SPECIFIER_ARRAY_INT;
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, parser->current_token->position, 
                ":PARSER - SyntaxError: Expected type specifier, got '%s'",
                parser->current_token->lexeme);
            array_push(parser->diagnostics, _diagnostic); 
            parser->panic = true;
            
            advance(parser);
            specifier = TYPE_SPECIFIER_NONE;
        }
    }

    return specifier;
}


// Parses a return statement based on the grammar.
//
// EBNF grammar:
//      return_statement         = 'return' expression ';' ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_return_statement(Parser* parser)
{
    advance(parser); // skip the keyword
    AST_Expression* value = parse_expression(parser);
    expect_token(parser, TOKEN_SEMICOLON, ";", false);

    return return_statement(value);
}


// Parses a block statement based on the grammar.
//
// EBNF grammar:
//      block_statement          = '{' statement* '}' ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_block_statement(Parser* parser)
{
    // NOTE(timo): This expect is important later when we can just assign
    // any statement to if and while statements and to function expressions
    expect_token(parser, TOKEN_LEFT_CURLYBRACE, "{", false);

    array* statements = array_init(sizeof (AST_Statement*));

    while (parser->current_token->kind != TOKEN_RIGHT_CURLYBRACE && 
           parser->current_token->kind != TOKEN_EOF)
    {
        array_push(statements, parse_statement(parser));

        if (parser->panic) 
            panic_mode(parser);
    }

    expect_token(parser, TOKEN_RIGHT_CURLYBRACE, "}", true);

    return block_statement(statements, statements->length);
}


// Parses a expression statement based on the grammar.
//
// EBNF grammar:
//      expression_statement     = expression ';' ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_expression_statement(Parser* parser)
{
    AST_Expression* expression = parse_expression(parser);
    expect_token(parser, TOKEN_SEMICOLON, ";", true);

    return expression_statement(expression);
}


// Parses a declaration statement based on the grammar.
//
// EBNF grammar:
//      declaration_statement    = declaration ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_declaration_statement(Parser* parser)
{
    AST_Declaration* declaration = parse_declaration(parser);
    // NOTE(timo): The closing semicolon is handled while parsing the declaration
    
    return declaration_statement(declaration);
}


// Parses a if statement based on the grammar.
//
// EBNF grammar:
//      if_statement        = 'if' expression 'then' statement ('else' statement)? ;
//
// TODO(timo): We could parse all the else-if's into its own array.
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_if_statement(Parser* parser)
{
    advance(parser); // skip the keyword
    AST_Expression* condition = parse_expression(parser);
    expect_token(parser, TOKEN_THEN, "then", false);
    AST_Statement* then = parse_statement(parser);
    AST_Statement* _else = NULL;

    if (parser->current_token->kind == TOKEN_ELSE)
    {
        advance(parser);
        _else = parse_statement(parser);
    }

    return if_statement(condition, then, _else);
}


// Parses a while statement based on the grammar.
//
// EBNF grammar:
//      while_statement          = 'while' expression 'do' statement ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_while_statement(Parser* parser)
{
    advance(parser); // skip the keyword
    AST_Expression* condition = parse_expression(parser);
    expect_token(parser, TOKEN_DO, "do", false);
    AST_Statement* body = parse_statement(parser);

    return while_statement(condition, body);
}


// Parses a break statement based on the grammar.
//
// EBNF grammar:
//      break_statement          = 'break' ';' ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_break_statement(Parser* parser)
{
    advance(parser); // skip the keyword
    expect_token(parser, TOKEN_SEMICOLON, ";", false);

    return break_statement();
}


// Parses a continue statement based on the grammar.
//
// EBNF grammar:
//      continue_statement       = 'continue' ';' ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created statement.
static AST_Statement* parse_continue_statement(Parser* parser)
{
    advance(parser); // skip the keyword
    expect_token(parser, TOKEN_SEMICOLON, ";", false);

    return continue_statement();
}


AST_Statement* parse_statement(Parser* parser)
{
    switch(parser->current_token->kind)
    {
        case TOKEN_LEFT_CURLYBRACE:
            return parse_block_statement(parser);
        case TOKEN_IF:
            return parse_if_statement(parser);
        case TOKEN_WHILE:
            return parse_while_statement(parser);
        case TOKEN_BREAK:
            return parse_break_statement(parser);
        case TOKEN_CONTINUE:
            return parse_continue_statement(parser);
        case TOKEN_RETURN:
            return parse_return_statement(parser);
        case TOKEN_IDENTIFIER:
            // NOTE(timo): This check is needed to distinguish between declaration
            // and expression statements since we don't have keywords for declaration.
            if (peek(parser)->kind == TOKEN_COLON)
                return parse_declaration_statement(parser);
        default:
            return parse_expression_statement(parser);
    }

    Diagnostic* _diagnostic = diagnostic(
        DIAGNOSTIC_ERROR, parser->current_token->position, 
        ":PARSER - Unreachable: Expected statement starter, got '%s'",
        parser->current_token->lexeme);
    array_push(parser->diagnostics, _diagnostic); 
    parser->panic = true;

    return NULL;
}


// Parses a parameter list for a function expression.
//
// Arguments
//      parser: Initialized parser.
// Returns
//      Pointer to array of parameters.
static array* parse_parameter_list(Parser* parser)
{
    array* parameters = array_init(sizeof (Parameter*));
    
    while (parser->current_token->kind != TOKEN_RIGHT_PARENTHESIS &&
           parser->current_token->kind != TOKEN_EOF)
    {
        Token* identifier = parser->current_token;
        advance(parser);

        expect_token(parser, TOKEN_COLON, ":", false);

        Type_Specifier specifier = parse_type_specifier(parser);
        array_push(parameters, function_parameter(identifier, specifier));

        // Comma is kept at the end, so we don't have to care about trailing commas.
        // TODO(timo): But what if there is multiple of them? Then this doesn't
        // work as expected.
        if (parser->current_token->kind == TOKEN_COMMA && peek(parser)->kind != TOKEN_RIGHT_PARENTHESIS)
            advance(parser);
    }

    return parameters;
}


// Parses a primary expression based on the grammar.
//
// EBNF grammar:
//      primary         = '(' expression ')'
//                      | function
//                      | literal ;
//      function        = '(' parameter_list? ')' '=>' statement ';'
//      parameter_list  = IDENTIFIER ':' type_specifier ( ',' IDENTIFIER ':' type_specifier )* ;
//      type_specifier  = 'int' | 'bool' ;
//      literal         = IDENTIFIER
//                      | INTEGER
//                      | BOOLEAN ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* primary(Parser* parser)
{
    AST_Expression* expression;

    switch (parser->current_token->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_BOOLEAN_LITERAL:
        {
            expression = literal_expression(parser->current_token);
            advance(parser);
            break;
        }
        case TOKEN_IDENTIFIER:
        {
            expression = variable_expression(parser->current_token);
            advance(parser);
            break;
        }
        case TOKEN_LEFT_PARENTHESIS:
        {
            advance(parser);

            // Function expression
            if ((parser->current_token->kind == TOKEN_IDENTIFIER && peek(parser)->kind == TOKEN_COLON) ||
                 parser->current_token->kind == TOKEN_RIGHT_PARENTHESIS)
            {
                array* parameters = parse_parameter_list(parser);
                
                expect_token(parser, TOKEN_RIGHT_PARENTHESIS, ")", true);
                expect_token(parser, TOKEN_ARROW, "=>", false);

                AST_Statement* body = parse_statement(parser);
                expression = function_expression(parameters, parameters->length, body);
            }
            else // Ordinary parenthesized expression
            {
                expression = parse_expression(parser);
                expect_token(parser, TOKEN_RIGHT_PARENTHESIS, ")", true);
            }
            
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, parser->current_token->position, 
                ":PARSER - SyntaxError: Invalid token '%s' in primary expression",
                // ":PARSER - SyntaxError: Expected expression.",
                parser->current_token->lexeme);
            array_push(parser->diagnostics, _diagnostic); 
            parser->panic = true;

            expression = error_expression();
            advance(parser);
        }
    }

    return expression;
}


// Parses a call and subscript expressions based on the grammar.
//
// EBNF grammar:
//      call            = primary '(' arguments? ')' ;
//      index           = primary '[' expression ']' ;
//      arguments       = expression ( ',' expression )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* call(Parser* parser)
{
    AST_Expression* expression = primary(parser);

    if (parser->current_token->kind == TOKEN_LEFT_BRACKET)
    {
        advance(parser);
        AST_Expression* index = parse_expression(parser);
        expect_token(parser, TOKEN_RIGHT_BRACKET, "]", true);
        expression = index_expression(expression, index);
    }
    else if (parser->current_token->kind == TOKEN_LEFT_PARENTHESIS)
    {
        advance(parser);
        array* arguments = array_init(sizeof (AST_Expression*));
        // TODO(timo): Should probably check for end of file too
        if (parser->current_token->kind != TOKEN_RIGHT_PARENTHESIS)
        {
            array_push(arguments, parse_expression(parser));

            while (parser->current_token->kind == TOKEN_COMMA)
            {
                advance(parser);
                array_push(arguments, parse_expression(parser));
            }
        }
        
        expect_token(parser, TOKEN_RIGHT_PARENTHESIS, ")", true);
        expression = call_expression(expression, arguments);
    }

    return expression;
}


// Parses a unary expression based on the grammar.
//
// EBNF grammar:
//      unary           = ( 'not' | '-' | '+' ) unary
//                      | call ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* unary(Parser* parser)
{
    if (parser->current_token->kind == TOKEN_MINUS || 
        parser->current_token->kind == TOKEN_PLUS ||
        parser->current_token->kind == TOKEN_NOT)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* operand = unary(parser);

        return unary_expression(_operator, operand);
    }

    return call(parser);
}


// Parses a binary expression based on the grammar.
//
// EBNF grammar:
//      factor          = unary ( ( '/' | '*' ) unary )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* factor(Parser* parser)
{
    AST_Expression* expression = unary(parser);

    while (parser->current_token->kind == TOKEN_MULTIPLY || 
           parser->current_token->kind == TOKEN_DIVIDE)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = unary(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


// Parses a binary expression based on the grammar.
//
// EBNF grammar:
//      term            = factor ( ( '+' | '-' ) factor )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* term(Parser* parser)
{
    AST_Expression* expression = factor(parser);

    while (parser->current_token->kind == TOKEN_PLUS || 
           parser->current_token->kind == TOKEN_MINUS)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = factor(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


// Parses a binary expression based on the grammar.
//
// EBNF grammar:
//      relation        = term ( ( '<' | '<=' | '>' | '>=' ) term )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* relation(Parser* parser)
{
    AST_Expression* expression = term(parser);

    while (parser->current_token->kind == TOKEN_LESS_THAN || 
           parser->current_token->kind == TOKEN_LESS_THAN_EQUAL ||
           parser->current_token->kind == TOKEN_GREATER_THAN || 
           parser->current_token->kind == TOKEN_GREATER_THAN_EQUAL)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = term(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


// Parses a binary expression based on the grammar.
//
// EBNF grammar:
//      equality        = relation ( ( '==' | '!=' ) relation )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* equality(Parser* parser)
{
    AST_Expression* expression = relation(parser);

    while (parser->current_token->kind == TOKEN_IS_EQUAL || 
           parser->current_token->kind == TOKEN_NOT_EQUAL)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = relation(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


// Parses a binary expression based on the grammar.
//
// EBNF grammar:
//      and             = equality ( 'and' equality )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* and(Parser* parser)
{
    AST_Expression* expression = equality(parser);

    while (parser->current_token->kind == TOKEN_AND)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = equality(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


// Parses a binary expression based on the grammar.
//
// EBNF grammar:
//      or              = and ( 'or' and )* ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* or(Parser* parser)
{
    AST_Expression* expression = and(parser);

    while (parser->current_token->kind == TOKEN_OR)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = and(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


// Parses an assignment expression based on the grammar.
//
// EBNF grammar:
//      assignment      = IDENTIFIER ':=' assignment
//                      | or ;
//
// Arguments
//      parser: Pointer to initialized Parser.
// Returns
//      Pointer to the newly created expression.
static AST_Expression* assignment(Parser* parser)
{
    AST_Expression* expression = or(parser);

    if (parser->current_token->kind == TOKEN_COLON_ASSIGN)
    {
        advance(parser);
        AST_Expression* value = assignment(parser);

        if (expression->kind != EXPRESSION_VARIABLE)
        {
            Diagnostic* _diagnostic = diagnostic(
                DIAGNOSTIC_ERROR, expression->position, 
                ":PARSER - SyntaxError: Invalid assignment target, expected a variable.");
            array_push(parser->diagnostics, _diagnostic); 
            // NOTE(timo): In case of invalid assignment target there is really no need
            // for error recovery since we are already at the end of the expression.
        }

        return assignment_expression(expression, value);
    }

    return expression;
}


AST_Expression* parse_expression(Parser* parser)
{
    return assignment(parser);
}


AST_Declaration* parse_declaration(Parser* parser)
{
    Token* identifier = parser->current_token;

    expect_token(parser, TOKEN_IDENTIFIER, "identifier", true);
    expect_token(parser, TOKEN_COLON, ":", true);

    Type_Specifier specifier = parse_type_specifier(parser);

    expect_token(parser, TOKEN_EQUAL, "=", true);
    
    AST_Expression* initializer = parse_expression(parser);

    expect_token(parser, TOKEN_SEMICOLON, ";", true);

    if (initializer->kind == EXPRESSION_FUNCTION)
        return function_declaration(identifier, specifier, initializer);
    else
        return variable_declaration(identifier, specifier, initializer);
}


void parse(Parser* parser)
{
    while (parser->current_token->kind != TOKEN_EOF)
    {
        array_push(parser->declarations, parse_declaration(parser));
        
        if (parser->panic) 
            panic_mode(parser);
    }

    assert(parser->current_token->kind == TOKEN_EOF);
}
