#include "t.h"


static AST_Statement* parse_block_statement(Parser* parser);
static AST_Statement* parse_return_statement(Parser* parser);
static inline void advance(Parser* parser);


void parser_init(Parser* parser, array* tokens)
{
    *parser = (Parser){ .tokens = tokens,
                        .index = 0,
                        .declarations = array_init(sizeof (AST_Declaration*)) };

    // NOTE(timo): This sets the initial first token and 
    // advances the pointer for the peek function
    advance(parser);
}


void parser_free(Parser* parser)
{
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


static void panic_mode(Parser* parser)
{
    advance(parser);

    Token_Kind kind;

    while ((kind = parser->current_token->kind) != TOKEN_EOF)
    {
        if (kind == TOKEN_SEMICOLON)
            advance(parser);
        // NOTE(timo): Panic mode ends when a token for starting new statement or
        // declaration is being found and parsing continues from that point on.
        if (kind == TOKEN_IDENTIFIER || kind == TOKEN_IF || kind == TOKEN_WHILE ||
            kind == TOKEN_RETURN || kind == TOKEN_BREAK)
            break;
        else
            advance(parser);
    }

    parser->panic = false;
}


static void expect_token(Token* token, Token_Kind kind, const char* lexeme)
{
    if (token->kind != kind)
    {
        error(token->position, "Invalid token '%s', expected '%s'\n", token->lexeme, lexeme);
        exit(1);
    }
}


static inline Token* peek(Parser* parser)
{
    // NOTE(timo): Since the current token is always advanced after assigning it,
    // the peek needs to return the current pointer and not current + 1
    return (Token*)parser->tokens->items[parser->index];
}


static inline void advance(Parser* parser)
{
    // NOTE(timo): This check is needed that there actually is a current token
    // all the time, at least the EoF token
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
        }
        break;
        case TOKEN_BOOL:
        {
            advance(parser);
            specifier = TYPE_SPECIFIER_BOOL;
        }
        break;
        case TOKEN_LEFT_BRACKET:
        {
            advance(parser);
            // NOTE(timo): Since we will only have arrays of int at this point
            // of time, we will force it here at the parsing stage already
            // TODO(timo): This needs more descriptive error message though
            expect_token(parser->current_token, TOKEN_INT, "int");
            advance(parser);
            expect_token(parser->current_token, TOKEN_RIGHT_BRACKET, "]");
            advance(parser);

            specifier = TYPE_SPECIFIER_ARRAY_INT;
        }
        break;
        default:
            printf("Expected type specifier, got something else '%s'\n", parser->current_token->lexeme);
            exit(1);
    }

    return specifier;
}


static AST_Statement* parse_return_statement(Parser* parser)
{
    advance(parser); // skip the keyword

    AST_Expression* value = parse_expression(parser);

    expect_token(parser->current_token, TOKEN_SEMICOLON, ";");
    advance(parser);

    return return_statement(value);
}


static AST_Statement* parse_block_statement(Parser* parser)
{
    // NOTE(timo): This expect is important later when we can just assign
    // any statement to if and while statements and to function expressions
    expect_token(parser->current_token, TOKEN_LEFT_CURLYBRACE, "{");
    advance(parser);

    array* statements = array_init(sizeof (AST_Statement*));

    while (parser->current_token->kind != TOKEN_RIGHT_CURLYBRACE && parser->current_token->kind != TOKEN_EOF)
        array_push(statements, parse_statement(parser));
        // TODO(timo): Check for errors and if found, panic mode

    expect_token(parser->current_token, TOKEN_RIGHT_CURLYBRACE, "}");
    advance(parser);

    return block_statement(statements, statements->length);
}


static AST_Statement* parse_expression_statement(Parser* parser)
{
    AST_Expression* expression = parse_expression(parser);
    expect_token(parser->current_token, TOKEN_SEMICOLON, ";");
    advance(parser);

    return expression_statement(expression);
}


static AST_Statement* parse_declaration_statement(Parser* parser)
{
    AST_Declaration* declaration = parse_declaration(parser);
    // NOTE(timo): The closing semicolon is handled while parsing the declaration
    
    return declaration_statement(declaration);
}


static AST_Statement* parse_if_statement(Parser* parser)
{
    advance(parser); // skip the keyword

    AST_Expression* condition = parse_expression(parser);

    expect_token(parser->current_token, TOKEN_THEN, "then");
    advance(parser);

    AST_Statement* then = parse_block_statement(parser);
    AST_Statement* _else;

    if (parser->current_token->kind == TOKEN_ELSE)
    {
        advance(parser);
        _else = parse_block_statement(parser);
    }

    return if_statement(condition, then, _else);
}


static AST_Statement* parse_while_statement(Parser* parser)
{
    advance(parser); // skip the keyword

    AST_Expression* condition = parse_expression(parser);

    expect_token(parser->current_token, TOKEN_DO, "do");
    advance(parser);

    AST_Statement* body = parse_block_statement(parser);

    return while_statement(condition, body);
}


static AST_Statement* parse_break_statement(Parser* parser)
{
    advance(parser); // skip the keyword
    
    expect_token(parser->current_token, TOKEN_SEMICOLON, ";");
    advance(parser);

    return break_statement();
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
        case TOKEN_RETURN:
            return parse_return_statement(parser);
        case TOKEN_IDENTIFIER:
            // NOTE(timo): This check is needed to distinguish between declaration 
            // and assignment since we don't have keywords for declaration.
            // Also the 2nd check is needed to make sure that 'foo;' expresison is
            // handled correctly as a variable expression.
            // Falls down to the expression statement if the condition is not met.
            if (peek(parser)->kind != TOKEN_COLON_ASSIGN && peek(parser)->kind != TOKEN_SEMICOLON)
                return parse_declaration_statement(parser);
        default:
            return parse_expression_statement(parser);
    }

    printf("Invalid token in parse_statement\n");
    exit(1);
}


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
            advance(parser);

            if (parser->current_token->kind == TOKEN_IDENTIFIER)
            {
                array* parameters = array_init(sizeof (Parameter*));
                
                do {
                    if (parser->current_token->kind == TOKEN_COMMA) advance(parser);
                    
                    Token* identifier = parser->current_token;
                    advance(parser);
                    expect_token(parser->current_token, TOKEN_COLON, ":");
                    advance(parser);
                    Type_Specifier specifier = parse_type_specifier(parser);

                    array_push(parameters, function_parameter(identifier, specifier));
                } while (parser->current_token->kind == TOKEN_COMMA);
                
                expect_token(parser->current_token, TOKEN_RIGHT_PARENTHESIS, ")");
                advance(parser);
                expect_token(parser->current_token, TOKEN_ARROW, "=>");
                advance(parser);

                AST_Statement* body = parse_statement(parser);

                expression = function_expression(parameters, parameters->length, body);
            }
            else if (parser->current_token->kind == TOKEN_RIGHT_PARENTHESIS)
            {
                advance(parser);     
                expect_token(parser->current_token, TOKEN_ARROW, "=>");
                advance(parser);

                AST_Statement* body = parse_statement(parser);

                expression = function_expression(NULL, 0, body);
            }
            else
            {
                expression = parse_expression(parser);

                expect_token(parser->current_token, TOKEN_RIGHT_PARENTHESIS, ")");
                advance(parser);
            }
            break;
        default:
            printf("Invalid token in primary()\n");
            exit(1);
    }

    return expression;
}


static AST_Expression* call(Parser* parser)
{
    AST_Expression* expression = primary(parser);

    if (parser->current_token->kind == TOKEN_LEFT_BRACKET)
    {
        advance(parser);
        // TODO(timo): Do we want to force the index to be integer literal
        // or do we leave that to the resolver? For now we pass it to resolver.
        AST_Expression* index = parse_expression(parser);

        expect_token(parser->current_token, TOKEN_RIGHT_BRACKET, "]");
        advance(parser);

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

        expect_token(parser->current_token, TOKEN_RIGHT_PARENTHESIS, ")");
        advance(parser);

        expression = call_expression(expression, arguments);
    }

    return expression;
}


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


static AST_Expression* factor(Parser* parser)
{
    AST_Expression* expression = unary(parser);

    while (parser->current_token->kind == TOKEN_MULTIPLY || parser->current_token->kind == TOKEN_DIVIDE)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = unary(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* term(Parser* parser)
{
    AST_Expression* expression = factor(parser);

    while (parser->current_token->kind == TOKEN_PLUS || parser->current_token->kind == TOKEN_MINUS)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = factor(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* ordering(Parser* parser)
{
    AST_Expression* expression = term(parser);

    while (parser->current_token->kind == TOKEN_LESS_THAN || parser->current_token->kind == TOKEN_LESS_THAN_EQUAL ||
           parser->current_token->kind == TOKEN_GREATER_THAN || parser->current_token->kind == TOKEN_GREATER_THAN_EQUAL)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = term(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* equality(Parser* parser)
{
    AST_Expression* expression = ordering(parser);

    while (parser->current_token->kind == TOKEN_IS_EQUAL || parser->current_token->kind == TOKEN_NOT_EQUAL)
    {
        Token* _operator = parser->current_token;
        advance(parser);
        AST_Expression* right = ordering(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


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


static AST_Expression* assignment(Parser* parser)
{
    AST_Expression* expression = or(parser);

    if (parser->current_token->kind == TOKEN_COLON_ASSIGN)
    {
        advance(parser);
        AST_Expression* value = assignment(parser);

        if (expression->kind == EXPRESSION_VARIABLE)
            // Advancement will be handled elsewhere
            return assignment_expression(expression, value);
        else
        {
            printf("Invalid assignment target, expected a variable.\n");
            exit(1);
        }
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

    expect_token(parser->current_token, TOKEN_IDENTIFIER, "identifier");
    advance(parser);

    expect_token(parser->current_token, TOKEN_COLON, ":");
    advance(parser);

    Type_Specifier specifier = parse_type_specifier(parser);

    expect_token(parser->current_token, TOKEN_EQUAL, "=");
    advance(parser);

    AST_Expression* initializer = parse_expression(parser);

    expect_token(parser->current_token, TOKEN_SEMICOLON, ";");
    advance(parser);

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
        
        if (parser->panic) panic_mode(parser);
    }

    assert(parser->current_token->kind == TOKEN_EOF);
}
