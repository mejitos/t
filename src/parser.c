#include "t.h"


static Type_Specifier parse_type_specifier(Parser* parser);
static AST_Declaration* parse_declaration(Parser* parser);
static AST_Statement* parse_statement(Parser* parser);
static AST_Statement* parse_block_statement(Parser* parser);
static AST_Statement* parse_return_statement(Parser* parser);
static AST_Expression* parse_expression(Parser* parser);


void parser_init(Parser* parser, Token** tokens)
{
    *parser = (Parser){ .tokens = tokens };
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


static void parser_push_declaration(Parser* parser, AST_Declaration* declaration)
{
    printf("Pushed declaration to list\n");
}


static Type_Specifier parse_type_specifier(Parser* parser)
{
    switch ((*parser->tokens)->kind)
    {
        case TOKEN_INT:
            parser->tokens++;
            return TYPE_SPECIFIER_INT;
        case TOKEN_BOOL:
            printf("Got type specifier\n");
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

    printf("Parsed return statement\n");

    return NULL;
}


static AST_Statement* parse_block_statement(Parser* parser)
{
    parser->tokens++;

    // TODO(timo): Statement list
    while ((*parser->tokens)->kind != TOKEN_RIGHT_CURLYBRACE && (*parser->tokens)->kind != TOKEN_EOF)
    {
        parse_statement(parser);
        printf("Pushing statement to list\n");
        // TODO(timo): Create statement and push to list
    }

    expect_token((*parser->tokens), TOKEN_RIGHT_CURLYBRACE);
    parser->tokens++;

    return NULL;
}


static AST_Statement* parse_expression_statement(Parser* parser)
{
    return NULL;
}


static AST_Statement* parse_statement(Parser* parser)
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
            printf("found integer literal %d\n", (*parser->tokens)->integer_value);
            parser->tokens++;
            return NULL;
        case TOKEN_LEFT_PARENTHESIS:
            parser->tokens++;
            if ((*parser->tokens)->kind == TOKEN_IDENTIFIER)
            {
                // TODO(timo): Parameter list
                do {
                    if ((*parser->tokens)->kind == TOKEN_COMMA) parser->tokens++;
                    
                    Token* identifier = *parser->tokens;
                    parser->tokens++;
                    expect_token((*parser->tokens), TOKEN_COLON);
                    parser->tokens++;
                    Type_Specifier specifier = parse_type_specifier(parser);

                    // TODO(timo): Create parameter and push to parameter list
                    printf("Added parameter '%s'\n", identifier->identifier);
                } while ((*parser->tokens)->kind == TOKEN_COMMA);
                
                expect_token((*parser->tokens), TOKEN_RIGHT_PARENTHESIS);
                parser->tokens++;
                expect_token((*parser->tokens), TOKEN_ARROW);
                parser->tokens++;

                AST_Statement* body = parse_statement(parser);
                // NOTE(timo): The closing semicolon is handled at declaration
            }
            else
            {
                printf("Parsing parenthesized expression\n");
            }
            return NULL;
        default:
            printf("Invalid token in primary()\n");
            exit(1);
    }
}


static AST_Expression* unary(Parser* parser)
{
    AST_Expression* expression = primary(parser);

    return expression;
}


static AST_Expression* factor(Parser* parser)
{
    AST_Expression* expression = unary(parser);

    return expression;
}


static AST_Expression* term(Parser* parser)
{
    AST_Expression* expression = factor(parser);

    return expression;
}


static AST_Expression* ordering(Parser* parser)
{
    AST_Expression* expression = term(parser);

    return expression;
}


static AST_Expression* equality(Parser* parser)
{
    AST_Expression* expression = ordering(parser);

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


static AST_Expression* parse_expression(Parser* parser)
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

    AST_Declaration* declaration;
    
    /* TODO(timo): We are not returning anything yet
    if (initializer->kind == EXPRESSION_FUNCTION)
        printf("Created function declaration\n");
    else
        printf("Created variable declaration\n");
    */
    
    // TODO(timo): Create declaration and return it
    return NULL;
}


void parse(Parser* parser)
{

    while ((*parser->tokens)->kind != TOKEN_EOF)
    {
        parser_push_declaration(parser, parse_declaration(parser));
        // printf("%d\n", (*parser->tokens)->kind);
        // parser->tokens++;
    }
}
