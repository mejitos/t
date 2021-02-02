#include "t.h"


static void lexer_push_token(Lexer* lexer, Token* token)
{
    if (! lexer->tokens)
    {
        lexer->tokens = xcalloc(8, sizeof (Token*));
        lexer->tokens_capacity = 8;
        lexer->tokens_length = 0;
    }

    if (lexer->tokens_capacity == lexer->tokens_length)
    {
        lexer->tokens = xrealloc(lexer->tokens, sizeof (Token*) * lexer->tokens_capacity * 2);
        lexer->tokens_capacity *= 2;
        for (int i = lexer->tokens_length; i < lexer->tokens_capacity; i++) *(lexer->tokens + i) = NULL;
    }

    *(lexer->tokens + lexer->tokens_length) = token;
    lexer->tokens_length++;
}


static inline void advance(Lexer* lexer, int n)
{
    lexer->stream += n;
}


static inline bool is_whitespace(const char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

static inline bool is_digit(const char ch)
{
    return '0' <= ch && ch <= '9';
}


void lex(Lexer* lexer)
{
    while (*lexer->stream != '\0')
    {
        switch (*lexer->stream)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                while (is_whitespace(*lexer->stream) && *lexer->stream != '\0')
                    advance(lexer, 1);
                continue;
            case '#':
                while (*lexer->stream != '\n' && *lexer->stream != '\0')
                    advance(lexer, 1);
                continue;
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9':
            {
                int value = 0;

                while (is_digit(*lexer->stream)) 
                {   
                    // Converts ascii digit to corresponding number
                    int digit = *lexer->stream - '0';

                    // Check for integer overflow in integer literal
                    if (value > (INT_MAX - digit) / 10) 
                    {   
                        printf("[LEXER] - Error: Integer overflow in integer literal\n");
                        // TODO(timo): Maybe just iterate through the literal, dont add token
                        // add continue the lexing normally from there
                        exit(1);
                    }

                    value *= 10; 
                    value += digit; 
                    advance(lexer, 1);
                }   
                    
                lexer_push_token(lexer, token_integer(value));
                continue;
            }
            case '+':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_PLUS));
                continue;
            case '-':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_MINUS));
                continue;
            case '*':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_MULTIPLY));
                continue;
            case '/':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_DIVIDE));
                continue;
            case '(':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_LEFT_PARENTHESIS));
                continue;
            case ')':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_RIGHT_PARENTHESIS));
                continue;
            case '[':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_LEFT_BRACKET));
                continue;
            case ']':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_RIGHT_BRACKET));
                continue;
            case '{':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_LEFT_CURLYBRACE));
                continue;
            case '}':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_RIGHT_CURLYBRACE));
                continue;
            case ',':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_COMMA));
                continue;
            case ';':
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_SEMICOLON));
                continue;
            case ':':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 2);
                    lexer_push_token(lexer, token_base(TOKEN_COLON_ASSIGN));
                    continue;
                }
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_COLON));
                continue;
            case '=':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 2);
                    lexer_push_token(lexer, token_base(TOKEN_IS_EQUAL));
                    continue;
                }
                if (*(lexer->stream + 1) == '>')
                {
                    advance(lexer, 2);
                    lexer_push_token(lexer, token_base(TOKEN_ARROW));
                    continue;
                }
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_EQUAL));
                continue;
            case '!':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 2);
                    lexer_push_token(lexer, token_base(TOKEN_NOT_EQUAL));
                    continue;
                }
                // NOTE(timo): If there is no equal symbol, let it go down to default and error
                // TODO(timo): We should actually notify the user about this one and not
                // let it flush down to the default.
            case '<':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 2);
                    lexer_push_token(lexer, token_base(TOKEN_LESS_THAN_EQUAL));
                    continue;
                }
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_LESS_THAN));
                continue;
            case '>':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 2);
                    lexer_push_token(lexer, token_base(TOKEN_GREATER_THAN_EQUAL));
                    continue;
                }
                advance(lexer, 1);
                lexer_push_token(lexer, token_base(TOKEN_GREATER_THAN));
                continue;
            default:
                printf("[LEXER] - SyntaxError: Invalid character '%c'", *lexer->stream);
                // TODO(timo): Maybe exit is too harsh, so create solution where we can
                // actually lex the whole file. Not absolutely necessary though
                exit(1);
        }
    }
    
    // Add the end of file token
    lexer_push_token(lexer, token_base(TOKEN_EOF));    
}
