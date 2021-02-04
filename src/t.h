#ifndef t_header
#define t_header

#include <stdio.h>      // for printing and stuff
#include <stdlib.h>     // for allocs, exit
#include <assert.h>     // for assertions
#include <stdbool.h>    // for bool data type
#include <limits.h>     // for integer overflow check
#include <string.h>     // for memcpy, memcmp etc
#include <stdarg.h>    // for varargs


/*
 *  Type definitions
 */
typedef struct Position Position;
typedef struct AST_Declaration AST_Declaration;
typedef struct AST_Statement AST_Statement;
typedef struct AST_Expression AST_Expression;


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
 *  Errors used in the compiler
 *
 */

void error(Position position, const char* fmt, ...);


/*
 *  The main routine of the compiler called from the entry point main.c
 *
 *  File: t.c
 */
void compile(const char* source);


/*
 *  General position struct for everyone to use
 *
 *  Since for now we will only support compiling of single file, there is no need to add the
 *  file attribute for the position at this stage. Maybe later.
 */
struct Position
{
    int line_start;
    int column_start;
    int line_end;
    int column_end;
};


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
    Position position;
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
Token* token_base(Token_Kind kind, Position position);
Token* token_integer(int value, Position position);
Token* token_boolean(bool value, Position position);
Token* token_identifier(const char* identifier, Position position);


/*
 *  Lexer stuff
 */
typedef struct Lexer
{
    const char* stream;
    Position position;

    Token** tokens;
    int tokens_length;
    int tokens_capacity;
} Lexer;


/*
 *  File: lexer.c
 */
void lexer_init(Lexer* lexer, const char* source);
void lexer_free(Lexer* lexer);
void lex(Lexer* lexer);


/*
 *  AST stuff
 *
 *  File: ast.c
 */
typedef enum Declaration_Kind
{
    DECLARATION_NONE,
    DECLARATION_VARIABLE,
    DECLARATION_FUNCTION,
} Declaration_Kind;


typedef enum Type_Specifier
{
    TYPE_SPECIFIER_NONE,
    TYPE_SPECIFIER_INT,
    TYPE_SPECIFIER_BOOL,
} Type_Specifier;


struct AST_Declaration
{
    Declaration_Kind kind;
    Type_Specifier specifier;
    Token* identifier;
    AST_Expression* initializer;
};


typedef enum Statement_Kind
{
    STATEMENT_NONE,
    STATEMENT_EXPRESSION,
    STATEMENT_BLOCK,
    STATEMENT_IF,
    STATEMENT_WHILE,
    STATEMENT_RETURN,
} Statement_Kind;


struct AST_Statement
{
    Statement_Kind kind;
    union {
        AST_Expression* expression;
        struct {
            AST_Statement** statements;
            int statements_length;
            int statements_capacity;
        } block;
        struct {
            AST_Expression* value;
        } _return;
    };
};


typedef struct Parameter
{
    Token* identifier;
    Type_Specifier type;
} Parameter;


typedef enum Expression_Kind
{
    EXPRESSION_NONE,
    EXPRESSION_LITERAL,
    EXPRESSION_VARIABLE,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY,
    EXPRESSION_ASSIGNMENT,
    EXPRESSION_FUNCTION,
} Expression_Kind;


struct AST_Expression
{
    Expression_Kind kind;
    union {
        Token* literal;
        Token* identifier;
        struct {
            Token* _operator;
            AST_Expression* operand;
        } unary;
        struct {
            Token* _operator;
            AST_Expression* left;
            AST_Expression* right;
        } binary;
        struct {
            AST_Expression* variable;
            AST_Expression* value;
        } assignment;
        struct {
            Token* identifier;
            Parameter** parameters;
            int arity;
            int parameters_capacity;
            AST_Statement* body;
        } function;
    };
};


#endif
