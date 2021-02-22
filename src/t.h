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
typedef struct Type Type;
typedef struct Value Value;
typedef struct Scope Scope;
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

    // Type specifiers
    TOKEN_INT, TOKEN_BOOL,
} Token_Kind;


/*
 *  Represents the analyzed lexeme and its attributes from the source file
 *
 */
typedef struct Token 
{
    Token_Kind kind;
    Position position;
    const char* lexeme;
    int lexeme_length;
} Token;


/*
 *  Allocator for basic token
 *
 *  File: token.c
 */
Token* token(Token_Kind kind, const char* lexeme, int lexeme_length, Position position);


/*
 *  Lexer stuff
 */
typedef struct Lexer
{
    const char* stream;
    array* tokens;
    Position position;
} Lexer;


/*
 *  File: lexer.c
 */
void lexer_init(Lexer* lexer, const char* source);
void lexer_free(Lexer* lexer);
void lex(Lexer* lexer);


/*
 *  Value
 *
 *  File: value.c
 */
typedef enum Value_Type
{
    VALUE_NONE,
    VALUE_INTEGER,
    VALUE_BOOLEAN,
} Value_Type;


struct Value
{
    Value_Type type;
    union {
        int integer;
        bool boolean;
    };
};

/*
bool is_type(Value value, Value_Type type);
bool is_int(Value value);
bool is_bool(Value value);
*/
bool value_as_boolean(Value value);
bool value_as_integer(Value value);


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
    TYPE_SPECIFIER_ARRAY_INT,
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
    STATEMENT_BREAK,
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
            AST_Expression* condition;
            AST_Statement* then;
            AST_Statement* _else;
        } _if;
        struct {
            AST_Expression* condition;
            AST_Statement* body;
        } _while;
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
    EXPRESSION_INDEX,
    EXPRESSION_FUNCTION,
    EXPRESSION_CALL,
} Expression_Kind;


typedef struct AST_Literal
{
    Token* literal;
    Value value;
} AST_Literal;


struct AST_Expression
{
    Expression_Kind kind;
    // Position position;
    // TODO(timo): Is this type actually needed for every expression?
    Type* type;

    union {
        Token* identifier;
        AST_Literal literal;
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
            AST_Expression* variable;
            AST_Expression* value;
        } index;
        struct {
            array* parameters;
            int arity;
            AST_Statement* body;
        } function;
        struct {
            AST_Expression* variable;
            array* arguments;
        } call;
    };
};


AST_Declaration* function_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer);
AST_Declaration* variable_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer);
AST_Statement* expression_statement(AST_Expression* expression);
AST_Statement* block_statement(array* statements, int statements_length);
AST_Statement* if_statement(AST_Expression* condition, AST_Statement* then, AST_Statement* _else);
AST_Statement* while_statement(AST_Expression* condition, AST_Statement* body);
AST_Statement* break_statement();
AST_Statement* return_statement(AST_Expression* value);
AST_Statement* declaration_statement(AST_Declaration* declaration);
AST_Expression* literal_expression(Token* literal);
AST_Expression* unary_expression(Token* _operator, AST_Expression* operand);
AST_Expression* binary_expression(AST_Expression* left, Token* _operator, AST_Expression* right);
AST_Expression* variable_expression(Token* identifier);
AST_Expression* assignment_expression(AST_Expression* variable, AST_Expression* value);
AST_Expression* index_expression(AST_Expression* variable, AST_Expression* value);
Parameter* function_parameter(Token* identifier, Type_Specifier specifier);
AST_Expression* function_expression(array* parameters, int arity, AST_Statement* body);
AST_Expression* call_expression(AST_Expression* variable, array* arguments);
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
    int index;
    array* tokens;
    Token* current_token;
    bool panic;
    array* declarations;
} Parser;


void parser_init(Parser* parser, array* tokens);
void parser_free(Parser* parser);
void parse(Parser* parser);

Type_Specifier parse_type_specifier(Parser* parser);
AST_Expression* parse_expression(Parser* parser);
AST_Statement* parse_statement(Parser* parser);
AST_Declaration* parse_declaration(Parser* parser);


/*
 *  Type
 *
 */
typedef enum Type_Kind
{
    TYPE_NONE,
    TYPE_INTEGER,
    TYPE_BOOLEAN,
} Type_Kind;


struct Type
{
    Type_Kind kind;
    size_t size;
    // alignment?

};

Type* type_integer();
Type* type_boolean();
bool type_is_integer(Type* type);
bool type_is_boolean(Type* type);


/*
 *  Operand
 *
 *  Used to tie the type and the value together
 *  
 *  HOX! Not used for anything at the moment!
 */
typedef struct Operand
{
    Type* type;
    Value value;
} Operand;


/*
 *  Symbol
 *
 *  TODO(timo): Symbols are not in use at the moment, just started
 *  to draft the outlines
 */
typedef enum Symbol_Kind
{
    SYMBOL_NONE,
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
} Symbol_Kind;


typedef enum Symbol_State
{
    STATE_UNRESOLVED,
    STATE_RESOLVING,
    STATE_RESOLVED,
} Symbol_State;


typedef struct Symbol
{
    Symbol_Kind kind;
    Symbol_State state;
    const char* identifier;
    Type* type;
    // value?
    Value value;
    // other?
} Symbol;

Symbol* symbol_variable(AST_Declaration* declaration);
Symbol* symbol_function(AST_Declaration* declaration);


/*
 *  Scope
 *
 *  File: scope.c
 */
struct Scope
{
    Scope* enclosing;
    array* symbols;
};

Scope* scope_init(Scope* enclosing); // returns the created scope
Scope* scope_enter(Scope* enclosing); // returns the entered scope
Scope* scope_leave(); // returns the closed scope
Symbol* scope_lookup(Scope* scope, const char* identifier);
void scope_declare(Scope* scope, Symbol* symbol);


/*
 *  Resolver
 *  
 *  File: resolver.c
 */
typedef struct Resolver
{
    Scope* global;
    Scope* local; // the current scope
    array* scopes; // the scope stack TODO(timo): push and pop functions
    struct {
        bool not_in_loop;
        bool not_in_function;
        bool returned;
        Type* return_type;
    } context;
} Resolver;

void resolver_init(Resolver* resolver);
void resolver_free(Resolver* resolver);
Type* resolve_expression(Resolver* resolver, AST_Expression* expression);
void resolve_statement(Resolver* resolver, AST_Statement* statement);
void resolve_declaration(Resolver* resolver, AST_Declaration* declaration);
Type* resolve_type_specifier(Type_Specifier specifier);
void resolve(Resolver* resolver, array* declarations);


/*
 *  Interpreter
 *
 *  File: interpreter.c
 */
typedef struct Interpreter
{
    array* declarations;
    // params for the program?
    // symbol table
    // NOTE(timo): Only global scope used for now to get things going
    Scope* global;
    Scope* local;
    // activation records / stack frames?
    // NOTE(timo): This is used for now just to be able to return something
    Value return_value;
} Interpreter;

void interpreter_init(Interpreter* interpreter);
Value evaluate_expression(Interpreter* interpreter, AST_Expression* expression);
void evaluate_statement(Interpreter* interpreter, AST_Statement* statement);
void evaluate_declaration(Interpreter* interpreter, AST_Declaration* declaration);
void interpret(Interpreter* interpreter);


#endif
