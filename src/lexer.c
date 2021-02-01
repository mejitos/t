#include "t.h"

/*
TODO(timo): Decide if we want to use this or not. There is probably
no need for this since we can just create the one Lexer at the main
compile function so there is no need for some general pointer type
void lexer_init(Lexer* lexer, const char* source)
{
    lexer = xcalloc(1, sizeof (Lexer));
    lexer->stream = source;
}
*/


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


static inline void advance(Lexer* lexer)
{
    lexer->stream++;
}


static inline bool is_whitespace(const char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
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
                    advance(lexer);
                continue;
            case '#':
                while (*lexer->stream != '\n' && *lexer->stream != '\0')
                    advance(lexer);
                continue;
            case '+':
                advance(lexer);
                lexer_push_token(lexer, token_base(TOKEN_PLUS));
                continue;
            case '-':
                advance(lexer);
                lexer_push_token(lexer, token_base(TOKEN_MINUS));
                continue;
            case '*':
                advance(lexer);
                lexer_push_token(lexer, token_base(TOKEN_MULTIPLY));
                continue;
            case '/':
                advance(lexer);
                lexer_push_token(lexer, token_base(TOKEN_DIVIDE));
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
