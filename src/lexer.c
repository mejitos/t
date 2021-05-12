// Lexer is responsible for creating stream of tokens from the source file.
//
// At the moment the operators doesn't really implement the maximal munching
// rule and therefore they should probably be separated as their own group
// in the same way numbers and names are separated. Then it is actually 
// possible to scan the maximum length operators.
//
// This has to be done because we really can't check characters after the 
// operators e.g. '+ ', because '1+2' is valid syntax. Therefore the operators
// has to be scanned as their own group.
//
// TODO(timo): Are the scanned lexemes last characters inclusive of exclusive?
// This is decision we have to make, to make better scanning.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


void lexer_init(Lexer* lexer, const char* source)
{
    *lexer = (Lexer){ .stream = source, 
                      .position.line_end = 1, 
                      .position.column_end = 1,
                      .diagnostics = array_init(sizeof (Diagnostic*)),
                      .tokens = array_init(sizeof (Token*)) };
}


void lexer_free(Lexer* lexer)
{
    for (int i = 0; i < lexer->diagnostics->length; i++)
    {
        Diagnostic* diagnostic = lexer->diagnostics->items[i];

        free((char*)diagnostic->message);
        diagnostic->message = NULL;

        free(diagnostic);
        diagnostic = NULL;
    }

    // NOTE(timo): This function will set the array to NULL after freeing
    array_free(lexer->diagnostics);

    // NOTE(timo): It is the callers responsibility to "unwind" the array
    // to the beginning so this iteration can be done.
    for (int i = 0; i < lexer->tokens->length; i++)
    {
        Token* token = lexer->tokens->items[i];
        free((char*)token->lexeme);
        token->lexeme = NULL;

        free(token);
        token = NULL;
    }
    
    // NOTE(timo): This function will set the array to NULL after freeing
    array_free(lexer->tokens);

    // NOTE(timo): The lexer itself is not freed, since it is being initialized 
    // in the stack at the top level function.
}


// Advances the current position in the source stream and updates the position
// of the current lexeme.
//
// Arguments
//      lexer: Pointer to initialized Lexer.
//      n: How many characters/steps is being advanced.
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


// Checks the character in the stream without advancing the stream. Character
// will be checked from a position in relation to the current position in the
// character stream.
//
// Arguments
//      lexer: Pointer to initialized Lexer.
//      n: Position in relation to the current position.
// Returns
//      The character in the position.
static inline const char peek(Lexer* lexer, int n)
{
    return *(lexer->stream + n);
}


// Checks if the character is whitespace.
//
// Arguments
//      ch: Character to be checked.
// Returns
//      Value true if character is whitespace, otherwise false.
static inline const bool is_whitespace(const char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}


// Checks if the character is a digit.
//
// Arguments
//      ch: Character to be checked.
// Returns
//      Value true if character is digit, otherwise false.
static inline const bool is_digit(const char ch)
{
    return '0' <= ch && ch <= '9';
}


// Checks if the character is in allowed alphabet.
//
// Arguments
//      ch: Character to be checked.
// Returns
//      Value true if character is in the allowed alphabet, otherwise false.
static inline const bool is_alpha(const char ch)
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

                // TODO(timo): The position goes one step too far when it comes 
                // to the lexeme of the number, so this is quick and dirty way 
                // to fix it for now. 
                lexer->position.column_end -= 1;

                array_push(lexer->tokens, token(TOKEN_INTEGER_LITERAL, lexeme, length, lexer->position));

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

                const int length = lexer->position.column_end - lexer->position.column_start;

                Token_Kind kind = TOKEN_IDENTIFIER;

                // Check if the scanned lexeme is one of the keywords/reserved 
                // words. If it is not, then it must be a identifier/name. The 
                // implementation is a bit dirty trie structure, but it is clear 
                // and simple to understand.
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

                // TODO(timo): The position goes one step too far when it comes 
                // to the lexeme of the identifier, so this is quick and dirty 
                // way to fix it for now. 
                lexer->position.column_end -= 1;

                array_push(lexer->tokens, token(kind, lexeme, length, lexer->position));

                lexer->position.column_end += 1;
                continue;
            }
            // TODO(timo): There must be a better way to handle these operators, 
            // punctuation etc. lexemes which are only couple of characters long.
            // TODO(timo): Therefore there could be possibility to give better
            // error messages in case of invalid operators etc. to give better
            // hints and possible solutions to the users.
            case '+':
                array_push(lexer->tokens, token(TOKEN_PLUS, "+", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '-':
                array_push(lexer->tokens, token(TOKEN_MINUS, "-", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '*':
                array_push(lexer->tokens, token(TOKEN_MULTIPLY, "*", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '/':
                array_push(lexer->tokens, token(TOKEN_DIVIDE, "/", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '(':
                array_push(lexer->tokens, token(TOKEN_LEFT_PARENTHESIS, "(", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ')':
                array_push(lexer->tokens, token(TOKEN_RIGHT_PARENTHESIS, ")", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '[':
                array_push(lexer->tokens, token(TOKEN_LEFT_BRACKET, "[", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ']':
                array_push(lexer->tokens, token(TOKEN_RIGHT_BRACKET, "]", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '{':
                array_push(lexer->tokens, token(TOKEN_LEFT_CURLYBRACE, "{", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '}':
                array_push(lexer->tokens, token(TOKEN_RIGHT_CURLYBRACE, "}", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ',':
                array_push(lexer->tokens, token(TOKEN_COMMA, ",", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ';':
                array_push(lexer->tokens, token(TOKEN_SEMICOLON, ";", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case ':':
                if (peek(lexer, 1) == '=')
                {
                    advance(lexer, 1);
                    array_push(lexer->tokens, token(TOKEN_COLON_ASSIGN, ":=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                array_push(lexer->tokens, token(TOKEN_COLON, ":", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '=':
                if (peek(lexer, 1) == '=')
                {
                    advance(lexer, 1);
                    array_push(lexer->tokens, token(TOKEN_IS_EQUAL, "==", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                if (peek(lexer, 1) == '>')
                {
                    advance(lexer, 1);
                    array_push(lexer->tokens, token(TOKEN_ARROW, "=>", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                array_push(lexer->tokens, token(TOKEN_EQUAL, "=", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '<':
                if (peek(lexer, 1) == '=')
                {
                    advance(lexer, 1);
                    array_push(lexer->tokens, token(TOKEN_LESS_THAN_EQUAL, "<=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                array_push(lexer->tokens, token(TOKEN_LESS_THAN, "<", 1, lexer->position));
                advance(lexer, 1);
                continue;
            case '>':
                if (peek(lexer, 1) == '=')
                {
                    advance(lexer, 1);
                    array_push(lexer->tokens, token(TOKEN_GREATER_THAN_EQUAL, ">=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                array_push(lexer->tokens, token(TOKEN_GREATER_THAN, ">", 1, lexer->position));
                advance(lexer, 1);
                continue;
            // NOTE(timo): This case has to be last, because there is only one
            // kind of operator allowed with the exclamation point and if it is
            // not the correct one, it will give a general error.
            case '!':
                if (peek(lexer, 1) == '=')
                {
                    advance(lexer, 1);
                    array_push(lexer->tokens, token(TOKEN_NOT_EQUAL, "!=", 2, lexer->position));
                    advance(lexer, 1);
                    continue;
                }
                advance(lexer, 1);
            default:
            {
                Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, lexer->position, 
                    ":LEXER - SyntaxError: Invalid character '%c'", *lexer->stream);
                array_push(lexer->diagnostics, _diagnostic);
                advance(lexer, 1);
                continue;
            }
        }
    }
    
    // NOTE(timo): This have to be here to take into account the end of file situation
    lexer->position.line_start = lexer->position.line_end;
    lexer->position.column_start = lexer->position.column_end;

    // Add the end of file token
    array_push(lexer->tokens, token(TOKEN_EOF, "<EoF>", 5, lexer->position));    
}
