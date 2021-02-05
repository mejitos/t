#include "t.h"


// TODO(timo): Are the scanned lexemes last characters inclusive of exclusive?
// This is decision we have to make, to make better scanning.


void lexer_init(Lexer* lexer, const char* source)
{
    *lexer = (Lexer){ .stream = source, 
                      .position.line_end = 1, 
                      .position.column_end = 1 };
}


void lexer_free(Lexer* lexer)
{
    // TODO(timo): We should take into account the "unwindind" of the tokens
    // so we can actually iterate them from the beginning. As of now, this
    // has to be handled by the caller before calling, but this is something
    // that should be thinked through
    for (Token** it = lexer->tokens; it != lexer->tokens + lexer->tokens_length; it++)
    {
        if ((*it)->kind == TOKEN_IDENTIFIER) 
        {
            free((char*)(*it)->identifier);
            (*it)->identifier = NULL;
        }

        free(*it);
        *it = NULL;
    }

    free(lexer->tokens);
    lexer->tokens = NULL;

    // NOTE(timo); The lexer itself is not freed, since it being 
    // initialized in the stack at the top level function.
}


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
    if (*lexer->stream == '\n')
    {
        lexer->position.line_end++;
        lexer->position.column_end = 1;
        lexer->stream += n;
    }
    else
    {
        lexer->position.column_end += n;
        lexer->stream += n;
    }
}


static inline bool is_whitespace(const char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}


static inline bool is_digit(const char ch)
{
    return '0' <= ch && ch <= '9';
}


static inline bool is_alpha(const char ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}


void lex(Lexer* lexer)
{
    while (*lexer->stream != '\0')
    {
        lexer->position.line_start = lexer->position.line_end;
        lexer->position.column_start = lexer->position.column_end;

        switch (*lexer->stream)
        {
            case ' ':
            case '\t':
            case '\n':
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
                        error(lexer->position, "[LEXER] - Error: Integer overflow in integer literal\n");
                        // TODO(timo): Maybe just iterate through the literal, dont add token
                        // add continue the lexing normally from there
                        exit(1);
                    }

                    value *= 10; 
                    value += digit; 
                    advance(lexer, 1);
                }   
                
                // TODO(timo): The position goes one step too far when it comes to the
                // lexeme of the number, so this is quick and dirty way to fix it for now. 
                lexer->position.column_end -= 1;

                lexer_push_token(lexer, token_integer(value, lexer->position));

                lexer->position.column_end += 1;
                continue;
            }
            case '_':
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
            case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
            case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            {
                while (is_alpha(*lexer->stream) || is_digit(*lexer->stream)) advance(lexer, 1);

                int length = lexer->position.column_end - lexer->position.column_start;
                char* lexeme = xcalloc(length + 1, sizeof (char));
                memcpy(lexeme, lexer->stream - length, length);

                Token_Kind kind = TOKEN_IDENTIFIER;

                switch (*lexeme)
                {
                    case 'a':
                        if (lexeme[1] == 'n' && lexeme[2] == 'd' && lexeme[3] == 0)
                            kind = TOKEN_AND;
                        break;
                    case 'b':
                        switch (lexeme[1])
                        {
                            case 'o':
                                if (lexeme[2] == 'o' && lexeme[3] == 'l' && lexeme[4] == 0)
                                    kind = TOKEN_BOOL;
                                break;
                            case 'r': 
                                if (lexeme[2] == 'e' && lexeme[3] == 'a' && lexeme[4] == 'k' && lexeme[5] == 0)
                                    kind = TOKEN_BREAK;
                                break;
                        }
                        break;
                    case 'c':
                        if (lexeme[1] == 'o' && lexeme[2] == 'n' && lexeme[3] == 't' && lexeme[4] == 'i' && 
                            lexeme[5] == 'n' && lexeme[6] == 'u' &&lexeme[7] == 'e' && lexeme[8] == 0)
                                kind = TOKEN_CONTINUE;
                        break;
                    case 'd':
                        if (lexeme[1] == 'o' && lexeme[2] == 0)
                            kind = TOKEN_DO;
                        break;
                    case 'e': 
                        if (lexeme[1] == 'l' && lexeme[2] == 's' && lexeme[3] == 'e' && lexeme[4] == 0)
                            kind = TOKEN_ELSE;
                        break;
                    case 'f': 
                        if (lexeme[1] == 'a' && lexeme[2] == 'l' && lexeme[3] == 's' && lexeme[4] == 'e' && lexeme[5] == 0)
                            kind = TOKEN_FALSE;
                        break;
                    case 'i':
                    {
                        switch (lexeme[1])
                        {
                            case 'f':
                                if (lexeme[2] == 0)
                                    kind = TOKEN_IF;
                                break;
                            case 'n':
                                if (lexeme[2] == 't' && lexeme[3] == 0)
                                    kind = TOKEN_INT;
                                break;
                        }
                        break;
                    }
                    case 'n':
                        if (lexeme[1] == 'o' && lexeme[2] == 't' && lexeme[3] == 0)
                            kind = TOKEN_NOT;
                        break;
                    case 'o': 
                        if (lexeme[1] == 'r' && lexeme[2] == 0)
                            kind = TOKEN_OR;
                        break;
                    case 'r': 
                        if (lexeme[1] == 'e' && lexeme[2] == 't' && lexeme[3] == 'u' && lexeme[4] == 'r' && lexeme[5] == 'n' && lexeme[6] == 0)
                            kind = TOKEN_RETURN;
                        break;
                    case 't':
                        switch (lexeme[1])
                        {
                            case 'h': 
                                if (lexeme[2] == 'e' && lexeme[3] == 'n' && lexeme[4] == 0)
                                    kind = TOKEN_THEN;
                                break;
                            case 'r': 
                                if (lexeme[2] == 'u' && lexeme[3] == 'e' && lexeme[4] == 0)
                                    kind = TOKEN_TRUE;
                                break;
                        }
                        break;
                    case 'w': 
                        if (lexeme[1] == 'h' && lexeme[2] == 'i' && lexeme[3] == 'l' && lexeme[4] == 'e' && lexeme[5] == 0)
                            kind = TOKEN_WHILE;
                        break;
                }
              
                
                // TODO(timo): The position goes one step too far when it comes to the
                // lexeme of the identifier, so this is quick and dirty way to fix it for now. 
                lexer->position.column_end -= 1;

                if (kind == TOKEN_IDENTIFIER) 
                    lexer_push_token(lexer, token_identifier(lexeme, lexer->position));
                else if (kind == TOKEN_TRUE || kind == TOKEN_FALSE)
                {
                    // NOTE(timo): Since we don't use the lexeme for the keywords
                    // we will have to free the memory allocated for the lexeme
                    free(lexeme);
                    lexer_push_token(lexer, token_boolean((bool)(kind == TOKEN_TRUE), lexer->position));
                }
                else 
                {
                    // NOTE(timo): Since we don't use the lexeme for the keywords
                    // we will have to free the memory allocated for the lexeme
                    free(lexeme);
                    lexer_push_token(lexer, token_base(kind, lexer->position));
                }

                lexer->position.column_end += 1;
                continue;
            }
            case '+':
                lexer_push_token(lexer, token_base(TOKEN_PLUS, lexer->position));
                advance(lexer, 1);
                continue;
            case '-':
                lexer_push_token(lexer, token_base(TOKEN_MINUS, lexer->position));
                advance(lexer, 1);
                continue;
            case '*':
                lexer_push_token(lexer, token_base(TOKEN_MULTIPLY, lexer->position));
                advance(lexer, 1);
                continue;
            case '/':
                lexer_push_token(lexer, token_base(TOKEN_DIVIDE, lexer->position));
                advance(lexer, 1);
                continue;
            case '(':
                lexer_push_token(lexer, token_base(TOKEN_LEFT_PARENTHESIS, lexer->position));
                advance(lexer, 1);
                continue;
            case ')':
                lexer_push_token(lexer, token_base(TOKEN_RIGHT_PARENTHESIS, lexer->position));
                advance(lexer, 1);
                continue;
            case '[':
                lexer_push_token(lexer, token_base(TOKEN_LEFT_BRACKET, lexer->position));
                advance(lexer, 1);
                continue;
            case ']':
                lexer_push_token(lexer, token_base(TOKEN_RIGHT_BRACKET, lexer->position));
                advance(lexer, 1);
                continue;
            case '{':
                lexer_push_token(lexer, token_base(TOKEN_LEFT_CURLYBRACE, lexer->position));
                advance(lexer, 1);
                continue;
            case '}':
                lexer_push_token(lexer, token_base(TOKEN_RIGHT_CURLYBRACE, lexer->position));
                advance(lexer, 1);
                continue;
            case ',':
                lexer_push_token(lexer, token_base(TOKEN_COMMA, lexer->position));
                advance(lexer, 1);
                continue;
            case ';':
                lexer_push_token(lexer, token_base(TOKEN_SEMICOLON, lexer->position));
                advance(lexer, 1);
                continue;
            case ':':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token_base(TOKEN_COLON_ASSIGN, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token_base(TOKEN_COLON, lexer->position));
                advance(lexer, 1);
                continue;
            case '=':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token_base(TOKEN_IS_EQUAL, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                if (*(lexer->stream + 1) == '>')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token_base(TOKEN_ARROW, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token_base(TOKEN_EQUAL, lexer->position));
                advance(lexer, 1);
                continue;
            case '!':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token_base(TOKEN_NOT_EQUAL, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                // NOTE(timo): If there is no equal symbol, let it go down to default and error
                // TODO(timo): We should actually notify the user about this one and not
                // let it flush down to the default.
            case '<':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token_base(TOKEN_LESS_THAN_EQUAL, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token_base(TOKEN_LESS_THAN, lexer->position));
                advance(lexer, 1);
                continue;
            case '>':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token_base(TOKEN_GREATER_THAN_EQUAL, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token_base(TOKEN_GREATER_THAN, lexer->position));
                advance(lexer, 1);
                continue;
            default:
                error(lexer->position, "[LEXER] - SyntaxError: Invalid character '%c'", *lexer->stream);
                // TODO(timo): Maybe exit is too harsh, so create solution where we can
                // actually lex the whole file. Not absolutely necessary though
                exit(1);
        }
    }
    
    // NOTE(timo): This have to be here to take into account the end of file situation
    lexer->position.line_start = lexer->position.line_end;
    lexer->position.column_start = lexer->position.column_end;

    // Add the end of file token
    lexer_push_token(lexer, token_base(TOKEN_EOF, lexer->position));    
}
