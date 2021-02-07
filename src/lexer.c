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
        // TODO(timo): Why this is not needed anymore when before it was needed?!?
        // All that has changed, is that the there is no union anymore for the
        // idenfitier specifically, since everything has a lexeme now.
        // NOTE(timo): Okay, so because we pass some things like operators as a value
        // to the token constructor, we don't explicitly allocate memory for them.
        // So now there is a problem with this. How do we know when to free the memory?
        // free((char*)(*it)->lexeme);
        // (*it)->lexeme = NULL;

        free((char*)(*it)->lexeme);
        (*it)->lexeme = NULL;

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
                const char* lexeme = lexer->stream;

                while (is_digit(*lexer->stream)) advance(lexer, 1);
                
                int length = lexer->position.column_end - lexer->position.column_start;

                // TODO(timo): The position goes one step too far when it comes to the
                // lexeme of the number, so this is quick and dirty way to fix it for now. 
                lexer->position.column_end -= 1;

                lexer_push_token(lexer, token(TOKEN_INTEGER_LITERAL, lexeme, length, lexer->position));

                lexer->position.column_end += 1;
                continue;
            }
            case '_':
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
            case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
            case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            {
                const char* lexeme = lexer->stream;

                while (is_alpha(*lexer->stream) || is_digit(*lexer->stream)) advance(lexer, 1);

                int length = lexer->position.column_end - lexer->position.column_start;

                Token_Kind kind = TOKEN_IDENTIFIER;

                switch (*lexeme)
                {
                    case 'a':
                        if (lexeme[1] == 'n' && lexeme[2] == 'd' && !is_alpha(lexeme[3]))
                            kind = TOKEN_AND;
                        break;
                    case 'b':
                        switch (lexeme[1])
                        {
                            case 'o':
                                if (lexeme[2] == 'o' && lexeme[3] == 'l' && !is_alpha(lexeme[4]))
                                    kind = TOKEN_BOOL;
                                break;
                            case 'r': 
                                if (lexeme[2] == 'e' && lexeme[3] == 'a' && lexeme[4] == 'k' && !is_alpha(lexeme[5]))
                                    kind = TOKEN_BREAK;
                                break;
                        }
                        break;
                    case 'c':
                        if (lexeme[1] == 'o' && lexeme[2] == 'n' && lexeme[3] == 't' && lexeme[4] == 'i' && 
                            lexeme[5] == 'n' && lexeme[6] == 'u' &&lexeme[7] == 'e' && !is_alpha(lexeme[8]))
                                kind = TOKEN_CONTINUE;
                        break;
                    case 'd':
                        if (lexeme[1] == 'o' && !is_alpha(lexeme[2]))
                            kind = TOKEN_DO;
                        break;
                    case 'e': 
                        if (lexeme[1] == 'l' && lexeme[2] == 's' && lexeme[3] == 'e' && !is_alpha(lexeme[4]))
                            kind = TOKEN_ELSE;
                        break;
                    case 'f': 
                        if (lexeme[1] == 'a' && lexeme[2] == 'l' && lexeme[3] == 's' && lexeme[4] == 'e' && !is_alpha(lexeme[5]))
                            kind = TOKEN_BOOLEAN_LITERAL;
                        break;
                    case 'i':
                        switch (lexeme[1])
                        {
                            case 'f':
                                if (!is_alpha(lexeme[2]))
                                    kind = TOKEN_IF;
                                break;
                            case 'n':
                                if (lexeme[2] == 't' && !is_alpha(lexeme[3]))
                                    kind = TOKEN_INT;
                                break;
                        }
                        break;
                    case 'n':
                        if (lexeme[1] == 'o' && lexeme[2] == 't' && !is_alpha(lexeme[3]))
                            kind = TOKEN_NOT;
                        break;
                    case 'o': 
                        if (lexeme[1] == 'r' && !is_alpha(lexeme[2]))
                            kind = TOKEN_OR;
                        break;
                    case 'r': 
                        if (lexeme[1] == 'e' && lexeme[2] == 't' && lexeme[3] == 'u' && lexeme[4] == 'r' && lexeme[5] == 'n' && !is_alpha(lexeme[6]))
                            kind = TOKEN_RETURN;
                        break;
                    case 't':
                        switch (lexeme[1])
                        {
                            case 'h': 
                                if (lexeme[2] == 'e' && lexeme[3] == 'n' && !is_alpha(lexeme[4]))
                                    kind = TOKEN_THEN;
                                break;
                            case 'r': 
                                if (lexeme[2] == 'u' && lexeme[3] == 'e' && !is_alpha(lexeme[4]))
                                    kind = TOKEN_BOOLEAN_LITERAL;
                                break;
                        }
                        break;
                    case 'w': 
                        if (lexeme[1] == 'h' && lexeme[2] == 'i' && lexeme[3] == 'l' && lexeme[4] == 'e' && !is_alpha(lexeme[5]))
                            kind = TOKEN_WHILE;
                        break;
                }

                // TODO(timo): The position goes one step too far when it comes to the
                // lexeme of the identifier, so this is quick and dirty way to fix it for now. 
                lexer->position.column_end -= 1;

                lexer_push_token(lexer, token(kind, lexeme, length, lexer->position));

                lexer->position.column_end += 1;
                continue;
            }
            // TODO(timo): I could just group these operator things to a single block
            // like I did with the identifiers
            case '+':
                lexer_push_token(lexer, token(TOKEN_PLUS, "+", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '-':
                lexer_push_token(lexer, token(TOKEN_MINUS, "-", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '*':
                lexer_push_token(lexer, token(TOKEN_MULTIPLY, "*", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '/':
                lexer_push_token(lexer, token(TOKEN_DIVIDE, "/", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '(':
                lexer_push_token(lexer, token(TOKEN_LEFT_PARENTHESIS, "(", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ')':
                lexer_push_token(lexer, token(TOKEN_RIGHT_PARENTHESIS, ")", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '[':
                lexer_push_token(lexer, token(TOKEN_LEFT_BRACKET, "[", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ']':
                lexer_push_token(lexer, token(TOKEN_RIGHT_BRACKET, "]", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '{':
                lexer_push_token(lexer, token(TOKEN_LEFT_CURLYBRACE, "{", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '}':
                lexer_push_token(lexer, token(TOKEN_RIGHT_CURLYBRACE, "}", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ',':
                lexer_push_token(lexer, token(TOKEN_COMMA, ",", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ';':
                lexer_push_token(lexer, token(TOKEN_SEMICOLON, ";", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ':':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token(TOKEN_COLON_ASSIGN, ":=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token(TOKEN_COLON, ":", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '=':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token(TOKEN_IS_EQUAL, "==", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                if (*(lexer->stream + 1) == '>')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token(TOKEN_ARROW, "=>", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token(TOKEN_EQUAL, "=", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '!':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token(TOKEN_NOT_EQUAL, "!=", 2, lexer->position));
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
                    lexer_push_token(lexer, token(TOKEN_LESS_THAN_EQUAL, "<=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token(TOKEN_LESS_THAN, "<", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '>':
                if (*(lexer->stream + 1) == '=')
                {
                    advance(lexer, 1);
                    lexer_push_token(lexer, token(TOKEN_GREATER_THAN_EQUAL, ">=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                lexer_push_token(lexer, token(TOKEN_GREATER_THAN, ">", 1, lexer->position));
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
    lexer_push_token(lexer, token(TOKEN_EOF, "<EoF>", 5, lexer->position));    
}
