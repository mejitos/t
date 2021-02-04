#include "t.h"


static Type_Specifier parse_type_specifier(Parser* parser);
static AST_Declaration* parse_declaration(Parser* parser);
// static AST_Statement* parse_statement(Parser* parser);
static AST_Statement* parse_block_statement(Parser* parser);
static AST_Statement* parse_return_statement(Parser* parser);
// static AST_Expression* parse_expression(Parser* parser);


void parser_init(Parser* parser, Token** tokens)
{
    *parser = (Parser){ .tokens = tokens,
                        .declarations = array_init(sizeof (AST_Declaration*)) };
}


void expect_token(Token* token, Token_Kind kind)
{
    if (token->kind == kind) return;
    else
    {
        printf("Invalid token kind '%d', expected '%d'\n", token->kind, kind);
        exit(1);
    }
}


static Type_Specifier parse_type_specifier(Parser* parser)
{
    switch ((*parser->tokens)->kind)
    {
        case TOKEN_INT:
            parser->tokens++;
            return TYPE_SPECIFIER_INT;
        case TOKEN_BOOL:
            parser->tokens++;
            return TYPE_SPECIFIER_BOOL;
        default:
            printf("Expected type specifier, got something else '%d'\n", (*parser->tokens)->kind);
            exit(1);
    }
}


static AST_Statement* parse_return_statement(Parser* parser)
{
    parser->tokens++;

    AST_Expression* value = parse_expression(parser);

    expect_token((*parser->tokens), TOKEN_SEMICOLON);
    parser->tokens++;

    return return_statement(value);
}


static AST_Statement* parse_block_statement(Parser* parser)
{
    parser->tokens++;

    array* statements = array_init(sizeof (AST_Statement*));

    while ((*parser->tokens)->kind != TOKEN_RIGHT_CURLYBRACE && (*parser->tokens)->kind != TOKEN_EOF)
    {
        array_push(statements, parse_statement(parser));
    }

    expect_token((*parser->tokens), TOKEN_RIGHT_CURLYBRACE);
    parser->tokens++;

    return NULL;
}


static AST_Statement* parse_expression_statement(Parser* parser)
{
    return NULL;
}


AST_Statement* parse_statement(Parser* parser)
{
    switch((*parser->tokens)->kind)
    {
        case TOKEN_LEFT_CURLYBRACE:
            return parse_block_statement(parser);
        case TOKEN_RETURN:
            return parse_return_statement(parser);
        default:
            return parse_expression_statement(parser);
    }

    printf("Invalid token in parse_statement\n");
    exit(1);
}


static AST_Expression* primary(Parser* parser)
{
    switch ((*parser->tokens)->kind)
    {
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_BOOLEAN_LITERAL:
        {
            AST_Expression* expression = literal_expression(*parser->tokens);
            parser->tokens++;
            return expression;
        }
        case TOKEN_LEFT_PARENTHESIS:
            parser->tokens++;
            if ((*parser->tokens)->kind == TOKEN_IDENTIFIER)
            {
                array* parameters = array_init(sizeof (Parameter*));

                do {
                    if ((*parser->tokens)->kind == TOKEN_COMMA) parser->tokens++;
                    
                    Token* identifier = *parser->tokens;
                    parser->tokens++;
                    expect_token((*parser->tokens), TOKEN_COLON);
                    parser->tokens++;
                    Type_Specifier specifier = parse_type_specifier(parser);

                    array_push(parameters, function_parameter(identifier, specifier));
                } while ((*parser->tokens)->kind == TOKEN_COMMA);
                
                expect_token((*parser->tokens), TOKEN_RIGHT_PARENTHESIS);
                parser->tokens++;
                expect_token((*parser->tokens), TOKEN_ARROW);
                parser->tokens++;

                AST_Statement* body = parse_statement(parser);

                return function_expression(parameters, parameters->length, body);
            }
            else
            {
                AST_Expression* expression = parse_expression(parser);

                expect_token(*parser->tokens, TOKEN_RIGHT_PARENTHESIS);
                parser->tokens++;

                return expression;
            }
        default:
            printf("Invalid token in primary()\n");
            exit(1);
    }
}


static AST_Expression* unary(Parser* parser)
{
    // TODO(timo): Add the logical and
    if ((*parser->tokens)->kind == TOKEN_MINUS || (*parser->tokens)->kind == TOKEN_PLUS)
    {
        Token* _operator = *parser->tokens;
        parser->tokens++;
        AST_Expression* operand = unary(parser);
        return unary_expression(_operator, operand);
    }

    return primary(parser);
}


static AST_Expression* factor(Parser* parser)
{
    AST_Expression* expression = unary(parser);

    while ((*parser->tokens)->kind == TOKEN_MULTIPLY || (*parser->tokens)->kind == TOKEN_DIVIDE)
    {
        Token* _operator = *parser->tokens;
        parser->tokens++;
        AST_Expression* right = unary(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* term(Parser* parser)
{
    AST_Expression* expression = factor(parser);

    while ((*parser->tokens)->kind == TOKEN_PLUS || (*parser->tokens)->kind == TOKEN_MINUS)
    {
        Token* _operator = *parser->tokens;
        parser->tokens++;
        AST_Expression* right = factor(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* ordering(Parser* parser)
{
    AST_Expression* expression = term(parser);

    while ((*parser->tokens)->kind == TOKEN_LESS_THAN || (*parser->tokens)->kind == TOKEN_LESS_THAN_EQUAL ||
           (*parser->tokens)->kind == TOKEN_GREATER_THAN || (*parser->tokens)->kind == TOKEN_GREATER_THAN_EQUAL)
    {
        Token* _operator = *parser->tokens;
        parser->tokens++;
        AST_Expression* right = term(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* equality(Parser* parser)
{
    AST_Expression* expression = ordering(parser);

    while ((*parser->tokens)->kind == TOKEN_IS_EQUAL || (*parser->tokens)->kind == TOKEN_NOT_EQUAL)
    {
        Token* _operator = *parser->tokens;
        parser->tokens++;
        AST_Expression* right = ordering(parser);
        expression = binary_expression(expression, _operator, right);
    }

    return expression;
}


static AST_Expression* and(Parser* parser)
{
    AST_Expression* expression = equality(parser);

    return expression;
}


static AST_Expression* or(Parser* parser)
{
    AST_Expression* expression = and(parser);

    return expression;
}


static AST_Expression* assignment(Parser* parser)
{
    AST_Expression* expression = or(parser);

    return expression;
}


AST_Expression* parse_expression(Parser* parser)
{
    return assignment(parser);
}


static AST_Declaration* parse_declaration(Parser* parser)
{
    Token* identifier = *parser->tokens;

    expect_token((*parser->tokens), TOKEN_IDENTIFIER);
    parser->tokens++;

    expect_token((*parser->tokens), TOKEN_COLON);
    parser->tokens++;

    Type_Specifier specifier = parse_type_specifier(parser);

    expect_token((*parser->tokens), TOKEN_EQUAL);
    parser->tokens++;

    AST_Expression* initializer = parse_expression(parser);
    expect_token((*parser->tokens), TOKEN_SEMICOLON);
    parser->tokens++;

    if (initializer->kind == EXPRESSION_FUNCTION)
        return function_declaration(identifier, specifier, initializer);
    else
        printf("Created variable declaration\n");
    
    // TODO(timo): Create declaration and return it
    return NULL;
}


void parse(Parser* parser)
{
    while ((*parser->tokens)->kind != TOKEN_EOF)
    {
        array_push(parser->declarations, parse_declaration(parser));
    }

    // assert(parser->declarations->length == 1);
}
