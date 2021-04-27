//
// TODO(timo): Filedocstring
//
// File(s):
//      ast.c
//      code_generator.c
//      diagnostics.c
//      intruction.c
//      interpreter.c
//      ir_generator.c
//      ir_runner.c
//      lexer.c
//      parser.c
//      resolver.c
//      scope.c
//      symbol.c
//      token.c
//      type.c
//      value.c

#ifndef t_header
#define t_header

#include "array.h"          // for dynamic array implementation
#include "stringbuilder.h"  // for my stringbuilder
#include "hashtable.h"      // for hashtable implementation
#include "memory.h"         // for custom x-allocators
#include "common.h"         // for general utility functions

#include <stdio.h>          // for printing and stuff
#include <stdlib.h>         // for allocs, exit
#include <assert.h>         // for assertions
#include <stdbool.h>        // for bool data type
#include <limits.h>         // for integer overflow check
#include <string.h>         // for memcpy, memcmp etc
#include <stdarg.h>         // for varargs


// Type definitions
//
// List of type definitions forward declared just so they can be used everywhere.
typedef struct Type Type;
typedef struct Scope Scope;
typedef struct AST_Declaration AST_Declaration;
typedef struct AST_Statement AST_Statement;
typedef struct AST_Expression AST_Expression;


//
//  Options
//
//  Fields
//      program:
typedef struct Options
{
    bool flag_verbose;
    const char* program;
} Options;


//
//  General position struct for everyone to use
//
//  Since for now we will only support compiling of single file, there is no need to add the
//  file attribute for the position at this stage. Maybe later.
//
//  Fields
//      line_start:
//      column_start:
//      line_end:
//      column_end:

typedef struct Position
{
    int line_start;
    int column_start;
    int line_end;
    int column_end;
} Position;


//
//  Diagnostics
//
//  File(s): diagnostics.c
//
//

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
Diagnostic* diagnostic_simple(Diagnostic_Kind kind, const char* message, ...);
void print_diagnostics(array* diagnostic);
void print_diagnostic(Diagnostic* diagnostic);


//
//  The main routine of the compiler called from the entry point main.c
//
//  File: t.c

void compile(const char* source, Options options);
void compile_from_file(const char* path, Options options);


//
//  Token related stuff

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


//
//  Represents the analyzed lexeme and its attributes from the source file
//

typedef struct Token 
{
    Token_Kind kind;
    Position position;
    const char* lexeme;
    int lexeme_length;
} Token;


//
//  Allocator for basic token
//
//  File: token.c

Token* token(Token_Kind kind, const char* lexeme, int lexeme_length, Position position);


//
//  Lexer stuff

typedef struct Lexer
{
    const char* stream;
    array* diagnostics;
    array* tokens;
    Position position;
} Lexer;


//
//  File: lexer.c

void lexer_init(Lexer* lexer, const char* source);
void lexer_free(Lexer* lexer);
void lex(Lexer* lexer);


// Enumeration of different value types to separate different kind of Value
// structures from each other.
//
// VALUE_NONE is not used for anything right now.
typedef enum Value_Type
{
    VALUE_NONE,
    VALUE_INTEGER,
    VALUE_BOOLEAN,
} Value_Type;


// Returns the string representation of the Value_Type.
//
// File(s): value.c
//
// Arguments
//      type: Value_Type to be represented as string.
const char* value_str(const Value_Type type);


// Value represents values of primitive datatypes boolean and integer. Values
// are separated from each other with the Value_Type enumeration. This
// structure is used for easier evaluation of the values and passing them 
// around.
//
// Fields
//      type: Type of the value.
//      integer: The actual integer value if the value is type of integer.
//      boolean: The actual boolean value if the value is type of boolean.
typedef struct Value
{
    Value_Type type;
    union {
        int64_t integer;
        bool boolean;
    };
} Value;


// Checks whether the value is certain type or not.
//
// There is functions for both true and false situations, just because it reads
// a lot more nicely instead of using the discrete and ugly !-operator.
//
// File(s): value.c
//
// Arguments
//      value: Value structure to be checked.
// Returns
//      Value true if the Value is the correct type according to the function, 
//      otherwise false.
const bool value_is_integer(const Value value);
const bool value_is_boolean(const Value value);
const bool value_is_not_boolean(const Value value);
const bool value_is_not_integer(const Value value);


//
//  AST stuff
//
//  File: ast.c

// Enumeration reperesenting type specifiers used with declarations.
// 
// Type specifier is just a enum even though it could also have a struct
// representation for more complex type like arrays for example. But since
// we only have two type specifiers and the only compound type we have, is
// array of integers used in the argument vector, we can use this simple
// enumeration representation of type specifiers.
//
// TYPE_SPECIFIER_NONE is not used for anything right now.
typedef enum Type_Specifier
{
    TYPE_SPECIFIER_NONE,
    TYPE_SPECIFIER_INT,
    TYPE_SPECIFIER_ARRAY_INT,
    TYPE_SPECIFIER_BOOL,
} Type_Specifier;


//
//
//
typedef enum Declaration_Kind
{
    DECLARATION_NONE,
    DECLARATION_VARIABLE,
    DECLARATION_FUNCTION,
} Declaration_Kind;


//
//
//
//
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
char* expression_str(Expression_Kind kind);
stringbuilder* expression_to_string(AST_Expression* expression, stringbuilder* sb);
char* statement_str(Statement_Kind kind);
char* declaration_str(Declaration_Kind kind);
char* type_specifier_str(Type_Specifier specifier);
void declaration_free(AST_Declaration* declaration);
void statement_free(AST_Statement* statement);
void expression_free(AST_Expression* expression);


//
//  Parser
//  
//  File: parser.c

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


// Enumeration of different types to separate types from each other
typedef enum Type_Kind
{
    TYPE_NONE,
    TYPE_INTEGER,
    TYPE_BOOLEAN,
    TYPE_FUNCTION,
    TYPE_ARRAY,
} Type_Kind;


// Returns the string representation of the Type_Kind.
//
// File(s): type.c
//
// Arguments
//      kind: Type_Kind to be represented as string.
const char* type_as_string(const Type_Kind kind);


// Structure representing types and their sizes, alignments etc. necessary
// info used later for type checking and for allocating and aligning the 
// memory correctly.
//
// Fields
//      kind: Kind of the type
//      size: Size of the type. At this point every type is 8 bytes to make
//            things more simple.
//      alignment: Alignment of the type. At this point everything is aligned
//                 to 8 bytes to make things more simple.
//      function:
//          return_type: Type of the value returned by the function.
//          arity: How many arguments the function takes.
//          parameters: Parameters of the function if there is any.
//          scope: Functions local scope.
//      array:
//          element_type: Type of the elements in the array.
//          length: Length/size of the array. Not in any use at this point
//                  because there is no general arrays in the language. The
//                  only usable array argv will be set at runtime.
struct Type
{
    Type_Kind kind;
    size_t size;
    int alignment;

    union {
        struct {
            Type* return_type;
            int arity;
            array* parameters; // array of pointers to parameter symbols
            Scope* scope;
        } function;
        struct {
            Type* element_type;
            int length;
        } array;
    };
};


// Constructors for different kind of types.
//
// File(s): type.c
//
// Arguments
//      element_type: Type of the element in the array. Only used with arrays.
// Returns
//      Pointer to the newly created type.
Type* type_none();
Type* type_integer();
Type* type_boolean();
Type* type_function();
Type* type_array(Type* element_type);


// Functions to check and compare types.
//
// File(s): type.c
//
// Arguments
//      Either one or two Type structures, depending on what is wanted to
//      check or compare.
// Returns
//      Value true if the check is true, otherwise false.
const bool type_is_none(const Type* type);
const bool type_is_not_none(const Type* type);
const bool type_is_boolean(const Type* type);
const bool type_is_not_boolean(const Type* type);
const bool type_is_integer(const Type* type);
const bool type_is_not_integer(const Type* type);
const bool type_is_array(const Type* type);
const bool type_is_not_array(const Type* type);
const bool type_is_function(const Type* type);
const bool type_is_not_function(const Type* type);
const bool types_equal(const Type* type1, const Type* type2);
const bool types_not_equal(const Type* type1, const Type* type2);


// Frees the memory allocated for the Type structure.
//
// File(s): type.c
//
// Arguments
//      type: Type to be freed.
void type_free(Type* type);


// Initializes a new type table containing all the primitive types.
//
// File(s): type.c
//
// Returns
//      Pointer to the newly created type table.
hashtable* type_table_init();


// Frees the memory allocated for the type table.
//
// File(s): type.c
//
// Arguments
//      table: Type table to be freed.
void type_table_free(hashtable* table);


//  Operand
//
//  Used to tie the type and the value together when resolving expressions
//  and having most use when doing constant folding.
//  
//  HOX! Not used for anything at the moment!
typedef struct Operand
{
    Type* type;
    Value value;
} Operand;


//
//  Symbol stuff
//

typedef enum Symbol_Kind
{
    SYMBOL_NONE,
    SYMBOL_VARIABLE,
    SYMBOL_PARAMETER,
    SYMBOL_FUNCTION,
    SYMBOL_TEMP,
} Symbol_Kind;


//
//  Symbol state is not in use at the moment.

typedef enum Symbol_State
{
    STATE_UNRESOLVED,
    STATE_RESOLVING,
    STATE_RESOLVED,
} Symbol_State;


//
//  - identifier comes from AST nodes and it is being free'd when AST nodes are freed
//  - the type is free'd by type table if it is primitive type, else symbol has the
//  responsibility of freeing the type

typedef struct Symbol
{
    Symbol_Kind kind;
    Symbol_State state;
    Scope* scope;
    const char* identifier;
    Type* type;
    Value value;

    // Register stuff
    int offset;
    int _register;
    // const char* _register;
} Symbol;


Symbol* symbol_variable(Scope* scope, const char* identifier, Type* type);
Symbol* symbol_temp(Scope* scope, const char* identifier, Type* type);
Symbol* symbol_parameter(Scope* scope, const char* identifier, Type* type);
Symbol* symbol_function(Scope* scope, const char* identifier, Type* type);
void symbol_free(Symbol* symbol);


//
//  Scope
//
//  File: scope.c

struct Scope
{
    const char* name;
    int offset; // alignment
    int offset_parameter;
    Scope* enclosing;
    hashtable* symbols;
};


Scope* scope_init(Scope* enclosing, const char* name); 
void scope_free(Scope* scope);
Symbol* scope_lookup(Scope* scope, const char* identifier);
void scope_declare(Scope* scope, Symbol* symbol);
bool scope_contains(Scope* scope, const char* identifier);
void dump_scope(Scope* scope, int indentation);


//
//  Resolver
//  
//  File: resolver.c

typedef struct Resolver
{
    array* diagnostics;
    hashtable* type_table;
    Scope* global;
    Scope* local;
    struct {
        bool not_in_loop;
        bool not_in_function;
        bool returned;
        char* current_function; // TODO(timo): This could also be pointer to symbol
        Type* return_type;
    } context;
} Resolver;

void resolver_init(Resolver* resolver, hashtable* type_table);
void resolver_free(Resolver* resolver);
Type* resolve_expression(Resolver* resolver, AST_Expression* expression);
void resolve_statement(Resolver* resolver, AST_Statement* statement);
void resolve_declaration(Resolver* resolver, AST_Declaration* declaration);
Type* resolve_type_specifier(Resolver* resolver, Type_Specifier specifier);
void resolve(Resolver* resolver, array* declarations);


//
//  Interpreter
//
//  File: interpreter.c

typedef struct Interpreter
{
    // array* declarations;
    // params for the program?
    Scope* global;
    Scope* local;
    // activation records / stack frames?
    // NOTE(timo): This is used for now just to be able to return something
    Value return_value;
} Interpreter;

// void interpreter_init(Interpreter* interpreter);
void interpreter_init(Interpreter* interpreter, Scope* global);
void interpreter_free(Interpreter* interpreter);
Value evaluate_expression(Interpreter* interpreter, AST_Expression* expression);
void evaluate_statement(Interpreter* interpreter, AST_Statement* statement);
void evaluate_declaration(Interpreter* interpreter, AST_Declaration* declaration);
// void interpret(const char* source);
Value interpret(const char* source);
// void interpret_from_file(const char* path);
Value interpret_from_file(const char* path);


//
//  Intermediate representation
//
//  File(s): ir_generator.c
//           ir_runner.c

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
    OP_NOT, // logical negation
    // relational 
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE,
    OP_EQ,
    OP_NEQ,
    // logical
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
    OP_DEREFERENCE,
} Operation;


//
//  Returns a string representation of Operation
//
//  Arguments
//      operation: Operation

const char* operation_str(Operation operation);


//
//  NOTE(timo): Address can be a name, a constant or a compiler generated temporary
//  These are pretty much the operands used in some literature
//
//  TODO(timo): Addresses are not used in Instructions for now
//
//  Fields
//      name:       program name, pointer to the names symbol table entry where 
//                  all the information of the name is kept
//      constant:   constant value but seems like it can be a variable too?
//      label:      just label

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


//
//  Represents a single instruction of the intermediate reperesentation
//
//  Fields
//      operation:
//      arg1:
//      arg2:
//      result:
//      size:
//      label:

typedef struct Instruction 
{
    Operation operation;

    // TODO(timo): All of these could or should be pointers to the symbol table
    char* arg1;
    char* arg2;
    char* result;

    int size; // this is used to compute sizes, alignments etc. info like that
    const char* label;
} Instruction;


Instruction* instruction_copy(char* arg, char* result);
Instruction* instruction_add(char* arg1, char* arg2, char* result);
Instruction* instruction_sub(char* arg1, char* arg2, char* result);
Instruction* instruction_mul(char* arg1, char* arg2, char* result);
Instruction* instruction_div(char* arg1, char* arg2, char* result);
Instruction* instruction_eq(char* arg1, char* arg2, char* result);
Instruction* instruction_neq(char* arg1, char* arg2, char* result);
Instruction* instruction_lt(char* arg1, char* arg2, char* result);
Instruction* instruction_lte(char* arg1, char* arg2, char* result);
Instruction* instruction_gt(char* arg1, char* arg2, char* result);
Instruction* instruction_gte(char* arg1, char* arg2, char* result);
Instruction* instruction_and(char* arg1, char* arg2, char* result);
Instruction* instruction_or(char* arg1, char* arg2, char* result);
Instruction* instruction_minus(char* arg, char* result);
Instruction* instruction_not(char* arg, char* result);
Instruction* instruction_function_begin(char* label);
Instruction* instruction_function_end(char* label);
Instruction* instruction_param_push(char* arg);
Instruction* instruction_param_pop(char* arg);
Instruction* instruction_call(char* arg, char* result, int n);
Instruction* instruction_return(char* arg);
Instruction* instruction_label(char* label);
Instruction* instruction_goto(char* label);
Instruction* instruction_goto_if_false(char* arg, char* label);
Instruction* instruction_dereference(char* arg, char* result, int offset);


//
// Frees the memory allocated for instruction
//
// Arguments
//      instruction: Instruction to be freed

void instruction_free(Instruction* instruction);


//
// Prints the instruction to terminal/console
//
// Arguments
//      instruction: Instruction to be printed

void dump_instruction(Instruction* instruction);


//
// Prints all instructions from array to terminal/console
//
// Arguments
//      instructions: Array of instructions to be printed

void dump_instructions(array* instructions);




//
//  Code in basic block has only one entry point and one exit point, meaning
//  there is no jump destinations inside the block and that only last instruction
//  can start executing next block
//
//  TODO(timo): Basic blocks are not implemented for now.
//

typedef struct Basic_Block
{
    // start, end?
    // leader?
    array* instructions;
} Basic_Block;


// TODO(timo): Basically all of this context stuff could be put into ir generator file
typedef enum IR_Context_Kind
{
    IR_CONTEXT_NONE,
    IR_CONTEXT_WHILE,
    IR_CONTEXT_IF,
} IR_Context_Kind;


typedef struct IR_Context
{
    IR_Context_Kind kind;

    union {
        struct {
            char* exit_label;
        } _while;
        struct {
            char* exit_label;
            bool exit_not_generated;
            bool new_context;
        } _if;
    };
} IR_Context;


typedef struct IR_Generator
{
    FILE* output_file;
    // array* blocks;
    array* instructions;
    array* diagnostics;
    int label;
    int temp;
    Scope* global;
    Scope* local;

    array* contexts;
    IR_Context* current_context;
} IR_Generator;


void ir_generator_init(IR_Generator* generator, Scope* global);
void ir_generator_free(IR_Generator* generator);
void ir_generate(IR_Generator* generator, array* declarations);
char* ir_generate_expression(IR_Generator* generator, AST_Expression* expression);
void ir_generate_statement(IR_Generator* generator, AST_Statement* statement);
void ir_generate_declaration(IR_Generator* generator, AST_Declaration* declaration);
void dump_instructions(array* instructions);


//
//  Code generation
//
//  Fields
//      output:
//      instructions:
//      diagnostics:
//      global:
//      local:

typedef struct Code_Generator
{
    FILE* output;
    array* instructions;
    array* diagnostics;
    Scope* global;
    Scope* local;

    // TODO(timo): this should probably just come via options or something
    // and not be saved in here
    const char* asm_file;

    // Used for register allocation stuff
    int destination;
    int source;
} Code_Generator;


void code_generator_init(Code_Generator* generator, Scope* global, array* instructions);
void code_generate(Code_Generator* generator);
void code_generator_free(Code_Generator* generator);


#endif
