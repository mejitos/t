#ifndef t_header
#define t_header

#include <stdio.h>


void compile(const char* source);




/*
 *  Token related stuff
 */
typedef enum Token_Kind
{
    TOKEN_EOF,

    // MISC
    TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_ARROW, TOKEN_HASH,

    // Literals
    TOKEN_INTEGER_LITERAL, TOKEN_BOOLEAN_LITERAL,

    // Grouping / separators / punctuation
    TOKEN_LEFT_PARENTHESIS, TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_CURLYBRACE, TOKEN_RIGHT_CURLYBRACE,
    TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_COMMA,

    // Operators
    TOKEN_COLON_ASSIGN, 
    TOKEN_IS_EQUAL, TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN, TOKEN_LESS_THAN_EQUAL,
    TOKEN_GREATER_THAN, TOKEN_GREATER_THAN_EQUAL,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,

    // Keywords / reserved words
    TOKEN_IF, TOKEN_THEN, TOKEN_ELSE, 
    TOKEN_WHILE, TOKEN_DO,
    TOKEN_AND, TOKEN_OR, TOKEN_NOT, 
    TOKEN_RETURN, TOKEN_BREAK, TOKEN_CONTINUE,
    TOKEN_TRUE, TOKEN_FALSE, // TODO(timo): Do I want booleans as literals or just as these reserved words since they are both

    // Type specifiers
    TOKEN_INT, TOKEN_BOOL,
} Token_Kind;


/*
 * Represents the analyzed lexeme and its attributes from the source file
 *
 */
typedef struct Token 
{
    Token_Kind kind;
    union {
        const char* identifier;
        int integer_value;
        bool boolean_value;
    };  
} Token;


#endif
