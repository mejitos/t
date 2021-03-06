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
#include "hashtable.h"  // for my hashtable implementation
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
 *  Diagnostics
 *
 *  File(s): diagnostics.c
 */
typedef enum Diagnostic_Kind
{
    DIAGNOSTIC_ERROR,
    DIAGNOSTIC_WARGNING,
} Diagnostic_Kind;


typedef struct Diagnostic
{
    Diagnostic_Kind kind;
    Position position;
    const char* message;
} Diagnostic;


Diagnostic* diagnostic(Diagnostic_Kind kind, Position position, const char* message, ...);
void print_diagnostics(array* diagnostic);
void print_diagnostic(Diagnostic* diagnostic);


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
    array* diagnostics;
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
    Position position;
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
    Position position;

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
    Position position;
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


struct AST_Expression
{
    Expression_Kind kind;
    Position position;
    Type* type;
    Value value;

    union {
        Token* identifier;
        Token* literal;
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
AST_Expression* error_expression();
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
    array* diagnostics;
    Position position;
    int index;
    array* tokens;
    Token* current_token;
    array* declarations;
    bool panic;
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
    TYPE_FUNCTION,
    TYPE_ARRAY,
} Type_Kind;


struct Type
{
    Type_Kind kind;
    size_t size;
    // alignment?
    union {
        struct {
            Type* return_type;
            int arity;
            array* parameters; // array of pointers to parameter symbols
        } function;
        // NOTE(timo): Even though the only array we have for now is the argv,
        // it is probably best idea to try to implement it "properly"
        struct {
            Type* element_type;
            int length;
        } array;
    };
};

Type* type_none();
Type* type_integer();
Type* type_boolean();
// Type* type_function(Type* return_type, array* parameters);
Type* type_function();
// Type* type_array(Type* element_type, int length);
Type* type_array(Type* element_type);
bool type_is_integer(Type* type);
bool type_is_boolean(Type* type);


/*
 *  Operand
 *
 *  Used to tie the type and the value together when resolving expressions
 *  and having most use when doing constant folding.
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
 *  NOTE(timo): Symbol state is not in use at the moment. It is only necessary
 *  when we have order independent declarations and we don't do that now.
 */
typedef enum Symbol_Kind
{
    SYMBOL_NONE,
    SYMBOL_VARIABLE,
    SYMBOL_PARAMETER,
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
    Value value;
} Symbol;


Symbol* symbol_variable(const char* identifier, Type* type);
Symbol* symbol_parameter(const char* identifier, Type* type);
Symbol* symbol_function(const char* identifier, Type* type);
void symbol_free(Symbol* symbol);


/*
 *  Scope
 *
 *  File: scope.c
 */
struct Scope
{
    Scope* enclosing;
    hashtable* symbols;
};


Scope* scope_init(Scope* enclosing); // returns the created scope
void scope_free(Scope* scope);
Scope* scope_enter(Scope* enclosing); // returns the entered scope
Scope* scope_leave(); // returns the closed scope
Symbol* scope_lookup(Scope* scope, const char* identifier);
void scope_declare(Scope* scope, Symbol* symbol);
bool scope_includes(Scope* scope, const char* identifier);


/*
 *  Resolver
 *  
 *  File: resolver.c
 */
typedef struct Resolver
{
    array* diagnostics;
    Scope* global;
    Scope* local; // the current scope
    // NOTE(timo): This scopes variable is probably not necessary since I actually
    // have only two scopes: global and local.
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
Type* resolve_type_specifier(Resolver* resolver, Type_Specifier specifier);
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
void interpreter_free(Interpreter* interpreter);
Value evaluate_expression(Interpreter* interpreter, AST_Expression* expression);
void evaluate_statement(Interpreter* interpreter, AST_Statement* statement);
void evaluate_declaration(Interpreter* interpreter, AST_Declaration* declaration);
void interpret(Interpreter* interpreter);


/*
 *  Intermediate representation
 *
 *  File(s): ir_generator.c
 *           ir_runner.c
 */
typedef enum Operation
{
    OP_NOOP,
    // binary
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    // unary
    OP_MINUS, // unary minus
    OP_NEG, // logical negation
    // relational 
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE,
    OP_EQ,
    OP_NEQ,
    OP_AND,
    OP_OR,
    // other
    OP_COPY, // assign / move / store
    OP_GOTO, // jump
    OP_GOTO_IF, // branch
    OP_GOTO_IF_FALSE,
    OP_GOTO_IF_TRUE,
    OP_FUNCTION_BEGIN,
    OP_FUNCTION_END,
    OP_PARAM_PUSH,
    OP_PARAM_POP,
    OP_CALL,
    OP_RETURN,
    OP_LABEL,
} Operation;

// Label
// Symbolic labels used to alter the flow of control
// Represents the index of a three address instruction in the sequence of instructions
// Actual indexes can be substituted for the labels either by making separate pass or backpatching
//
// Running numbers for the instructions could also be used. I think I'll use the labels
// since they are the closest to the general assembly and in general they just make more sense

typedef struct Instruction Instruction;

// NOTE(timo): Address can be a name, a constant or a compiler generated temporary
// These are pretty much the operands used in some literature
//
// name:        program name, pointer to the names symbol table entry where 
//              all the information of the name is kept
// constant:    constant value but seems like it can be a variable too?
// label:       just label
typedef struct Address
{
    // TODO(timo): I think we don't need this abstraction, since the goal is
    // just to get everything into the symbol table. But we could keep this,
    // in case of situation there is labels or something else than symbols
    union {
        Symbol* name;
        // Value constant; // These can be fetched from the symbol table?
        const char* label;
    };
} Address;


// When using triples, the result of the operation x op y is referred by its position
// rather than by an explicit temporary name
//
// Quads are easier to handle and undestand for now, so we use quads
struct Instruction 
{
    // position / line / index?
    Operation operation;
    char* arg1;
    char* arg2;
    char* result;

    union {
        int integer; // this is used to compute sizes, alignments etc. info like that
        const char* string; // Field for label value?
    } value;
    // Instruction* next; // Linked list for this one since it is easier to modify?
};


/*
 *  Code in basic block has only one entry point and one exit point, meaning
 *  there is no jump destinations inside the block and that only last instruction
 *  can start executing next block
 *
 */
typedef struct Basic_Block
{
    // start, end?
    // leader?
    array* instructions;
} Basic_Block;


typedef struct IR_Generator
{
    FILE* output_file;
    // array* blocks;
    array* instructions;
    int label;
    int temp;
    Scope* symbol_table;
} IR_Generator;


void generator_init(IR_Generator* generator);
void generator_free(IR_Generator* generator);
char* generate_expression(IR_Generator* generator, AST_Expression* expression);
void generate_statement(IR_Generator* generator, AST_Statement* statement);
void generate_declaration(IR_Generator* generator, AST_Declaration* declaration);

void dump_instructions(array* instructions);


#endif
