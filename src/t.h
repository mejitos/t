#ifndef t_header
#define t_header

#include <stdio.h>      // for printing and stuff
#include <stdlib.h>     // for allocs, exit
#include <assert.h>     // for assertions
#include <stdbool.h>    // for bool data type
#include <limits.h>     // for integer overflow check


/*
 *  Memory allocators à la K & R. Therefore code is borrowed from the book The C Programming
 *  Language by Brian Kernighan and Dennis Ritchie. General opinion though is that this kind
 *  of allocation of memory is pretty bad practice in prodcution, but luckily we ain't in 
 *  production...at least yet.
 *
 *  The memory is being alllocated like it normally would with these C functions, but the
 *  success of this allocation will be checked and if it failed -> program will exit.
 *
 *  File: memory.c
 */
void* xmalloc(size_t bytes);
void* xcalloc(size_t length, size_t size);
void* xrealloc(void* pointer, size_t bytes);


/*
 *  The main routine of the compiler called from the entry point main.c
 *
 *  File: t.c
 */
void compile(const char* source);


/*
 *  Token related stuff
 */
typedef enum Token_Kind
{
    TOKEN_EOF,

    // MISC
    TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_ARROW, 

    // Literals
    TOKEN_INTEGER_LITERAL, TOKEN_BOOLEAN_LITERAL,

    // Grouping / separators / punctuation
    TOKEN_LEFT_PARENTHESIS, TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_CURLYBRACE, TOKEN_RIGHT_CURLYBRACE,
    TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_COMMA,

    // Assignment
    TOKEN_COLON_ASSIGN, 

    // Equality and comparison operators
    TOKEN_IS_EQUAL, TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN, TOKEN_LESS_THAN_EQUAL,
    TOKEN_GREATER_THAN, TOKEN_GREATER_THAN_EQUAL,

    // Arithmetic operators
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


/*
 *  Allocator for basic token
 *
 *  File: token.c
 */
Token* token_base(Token_Kind kind);
Token* token_integer(int value);


/*
 *  Lexer stuff
 */
typedef struct Lexer
{
    const char* stream;

    Token** tokens;
    int tokens_length;
    int tokens_capacity;
} Lexer;


/*
 *  File: lexer.c
 */
void lexer_init(Lexer* lexer, const char* source);
void lex(Lexer* lexer);



#endif
