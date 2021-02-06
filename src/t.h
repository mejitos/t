#ifndef t_header
#define t_header

#include <stdio.h>      // for printing and stuff
#include <stdlib.h>     // for allocs, exit
#include <assert.h>     // for assertions
#include <stdbool.h>    // for bool data type
#include <limits.h>     // for integer overflow check
#include <string.h>     // for memcpy, memcmp etc
#include <stdarg.h>     // for varargs

#include "array.h"      // for my dynamic arrays
#include "memory.h"     // for custom x-allocators


/*
 *  Type definitions
 */
typedef struct Position Position;
typedef struct AST_Declaration AST_Declaration;
typedef struct AST_Statement AST_Statement;
typedef struct AST_Expression AST_Expression;


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
    STATEMENT_DECLARATION,
} Statement_Kind;


struct AST_Statement
{
    Statement_Kind kind;
    union {
        AST_Expression* expression;
        AST_Declaration* declaration;
        struct {
            array* statements;
            int statements_length;
        } block;
        struct {
            AST_Expression* value;
        } _return;
    };
};


typedef struct Parameter
{
    Token* identifier;
    Type_Specifier specifier;
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
            array* parameters;
            int arity;
            AST_Statement* body;
        } function;
    };
};


AST_Declaration* function_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer);
AST_Declaration* variable_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer);
AST_Statement* expression_statement(AST_Expression* expression);
AST_Statement* block_statement(array* statements, int statements_length);
AST_Statement* return_statement(AST_Expression* value);
AST_Statement* declaration_statement(AST_Declaration* declaration);
AST_Expression* literal_expression(Token* literal);
AST_Expression* unary_expression(Token* _operator, AST_Expression* operand);
AST_Expression* binary_expression(AST_Expression* left, Token* _operator, AST_Expression* right);
AST_Expression* variable_expression(Token* identifier);
AST_Expression* assignment_expression(AST_Expression* variable, AST_Expression* value);
Parameter* function_parameter(Token* identifier, Type_Specifier specifier);
AST_Expression* function_expression(array* parameters, int arity, AST_Statement* body);
void declaration_free(AST_Declaration* declaration);
void statement_free(AST_Statement* statement);
void expression_free(AST_Expression* expression);


/*
 *  Parser
 *  
 *  File: parser.c
 */
typedef struct Parser
{
    Token** tokens;
    array* declarations;
} Parser;

void parser_init(Parser* parser, Token** tokens);
void parser_free(Parser* parser);
void parse(Parser* parser);
// Public functions to make testing easier
AST_Expression* parse_expression(Parser* parser);
AST_Statement* parse_statement(Parser* parser);
AST_Declaration* parse_declaration(Parser* parser);



/*
 *  Interpreter
 *
 *  File: interpreter.c
 */
typedef struct Interpreter
{
    array* declarations;
} Interpreter;

void interpret(array* declarations);

#endif
