// The main header file of the compiler which has definitions for all of the
// used interfaces in the compiler.
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
//
// Author: Timo Mehto
// Date: 2021/05/12

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


// General position struct for everyone to use
//
// Since for now we will only support compiling of single file, there is no 
// need to add the file attribute for the position at this stage. Maybe later.
//
// Members
//      line_start: Starting line of the thing.
//      column_start: Starting column of the thing.
//      line_end: Ending line of the thing.
//      column_end: Ending column of the thing.
typedef struct Position
{
    int line_start;
    int column_start;
    int line_end;
    int column_end;
} Position;


// Enumeration of different diagnostic kinds.
typedef enum Diagnostic_Kind
{
    DIAGNOSTIC_ERROR,
    DIAGNOSTIC_WARNING,
} Diagnostic_Kind;


// Errors and warnings collected during compilation process.
//
// Members
//      kind: Kind of the diagnostic (ERROR/WARNING).
//      position: Where the diagnostic was spotted in the source.
//      message: Message describing the diagnostic.
typedef struct Diagnostic
{
    Diagnostic_Kind kind;
    Position position;
    const char* message;
} Diagnostic;


// Factory function for initializing new diagnostic.
//
// Freeing the diagnostics is the responsibility of the user of the diagnostics.
//
// File(s): diagnostics.c
//
// Arguments
//      kind: Kind of the diagnostic (ERROR/WARNING).
//      position: Where the diagnostic was spotted in the source.
//      message: Message describing the diagnostic.
//      ...: Arguments for the formatted message.
Diagnostic* diagnostic(const Diagnostic_Kind kind, const Position position, const char* message, ...);


// Prints all diagnostic messages in array of diagnostics.
//
// File(s): diagnostics.c
//
// Arguments
//      diagnostics: Array of diagnostics.
void print_diagnostics(const array* diagnostics);


// Prints diagnostic message.
//
// File(s): diagnostics.c
//
// Arguments
//      diagnostic: Diagnostic to be printed.
void print_diagnostic(const Diagnostic* diagnostic);


// Struture for compiler options which are parsed at the beginning of the
// program and will be passed to the compilation functions.
//
// Members
//      program: Pointer to the name of the program.
//      source_file: Pointer to the source file path.
//      interpret: If the source file will be interpreted instead of being 
//                 compiled.
//      interpret_ast: If the tree walker interpreter is used.
//      interpret_ir: If the IR runner/interpreter is used. NOT IMPLEMENTED.
//      show_summary: If the summary of the compilation is printed at the end.
//      show_symbols: If the symbols of the symbol table are printed after 
//                    resolving stage.
//      show_ir: If the IR instructions are printed after generating them.
//      show_asm: If the generated assembly file is printed.
struct Options
{
    const char* program;
    const char* source_file;

    bool interpret;
    bool interpret_ast;
    bool interpret_ir;

    bool show_summary;
    bool show_symbols;
    bool show_ir;
    bool show_asm;
};


// Parses command line arguments and creates options for compiler.
//
// File(s): t.c
//
// Arguments
//      options: Pointer to Options structure.
//      argc: Pointer to the argument count.
//      argv: Pointer to the argument vector.
void parse_options(struct Options* options, int* argc, char*** argv);


// Compiles the program from a source string.
//
// File(s): t.c
//
// Arguments
//      source: Pointer to the source string to be compiled.
//      options: Options structure as compiler options.
void compile(const char* source, struct Options options);


// Compiles the program from a source file.
//
// File(s): t.c
//
// Arguments
//      path:
//      options: Options structure as compiler options.
void compile_from_file(const char* path, struct Options options);


// Enumeration of the classification of the lexeme.
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


// Represents the analyzed lexeme and its attributes from the source.
//
// Members
//      kind: Classification of the lexeme.
//      position: Position of the lexeme in the source file.
//      lexeme: The lexeme itself.
//      lexeme_length: Length of the lexeme.
typedef struct Token 
{
    Token_Kind kind;
    Position position;
    const char* lexeme;
    int lexeme_length;
} Token;


// Factory function for initializing a token.
//
// File(s): token.c
//
// Arguments
//      kind: Classification of the lexeme.
//      lexeme: Scanned lexeme.
//      lexeme_length: Length of the lexeme.
//      position: Position of the lexeme in the source file.
// Returns
//      Pointer to the newly created Token.
Token* token(Token_Kind kind, const char* lexeme, const int lexeme_length, Position position);


// Lexer scans through the source (file or string), analyzes the scanned
// lexemes and creates a stream of tokens from the lexemes.
//
// Members
//      stream: Source stream of characters.
//      diagnostics: Array of collected diagnostics.
//      tokens: Array of collected tokens.
//      position: Position of the current lexeme.
typedef struct Lexer
{
    const char* stream;
    array* diagnostics;
    array* tokens;
    Position position;
} Lexer;


// Factory function to initialize new Lexer.
//
// File(s): lexer.c
//
// Arguments:
//      lexer: Pointer to Lexer structure.
//      source: Source as a stream of characters.
void lexer_init(Lexer* lexer, const char* source);


// Frees the memory allocated for Lexer.
//
// File(s): lexer.c
//
// Arguments:
//      lexer: Lexer to be freed.
void lexer_free(Lexer* lexer);


// Main function of the lexer which scans through the source and produces a
// stream of tokens. Generated tokens can be accessed from the Lexers 'tokens'
// which is an array.
//
// File(s): lexer.c
//
// Arguments:
//      lexer: Pointer to already initialized Lexer.
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
// Members
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
const bool value_is_none(const Value value);
const bool value_is_integer(const Value value);
const bool value_is_boolean(const Value value);
const bool value_is_not_none(const Value value);
const bool value_is_not_boolean(const Value value);
const bool value_is_not_integer(const Value value);


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


// Returns string presentation of a type specifier.
//
// File(s): ast.c
//
// Arguments
//      specifier: Type specifier.
const char* type_specifier_str(const Type_Specifier specifier);


// Enumeration of different declaration classifications.
typedef enum Declaration_Kind
{
    DECLARATION_NONE,
    DECLARATION_VARIABLE,
    DECLARATION_FUNCTION,
} Declaration_Kind;


// Returns a string representation of a declaration kind.
//
// File(s): ast.c
//
// Arguments
//      kind: Declaration kind.
const char* declaration_str(const Declaration_Kind kind);


// General structure for all declarations.
//
// Members
//      kind: Classification of the declaration.
//      position: Position of the declaration.
//      specifier: Type of the declaration.
//      identifier: Name of the declaration.
//      initializer: Value of the declaration.
struct AST_Declaration
{
    Declaration_Kind kind;
    Position position;
    Type_Specifier specifier;
    Token* identifier;
    AST_Expression* initializer;
};


// Factory functions for initializing declarations.
//
// File(s): ast.c
//
// Arguments
//      identifier: Name of the declaration.
//      specifier: Intended type of the declaration.
//      initializer: Value of the declaration.
// Returns
//      Pointer to newly created declaration.
AST_Declaration* function_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer);
AST_Declaration* variable_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer);


// Frees the memory allocated for a declaration.
//
// File(s): ast.c
//
// Arguments
//      declaration: Declaration to be freed.
void declaration_free(AST_Declaration* declaration);



// Enumeration of different statement classifications.
typedef enum Statement_Kind
{
    STATEMENT_NONE,
    STATEMENT_EXPRESSION,
    STATEMENT_BLOCK,
    STATEMENT_IF,
    STATEMENT_WHILE,
    STATEMENT_RETURN,
    STATEMENT_BREAK,
    STATEMENT_CONTINUE,
    STATEMENT_DECLARATION,
} Statement_Kind;


// Returns a string representation of a statement kind.
//
// File(s): ast.c
//
// Arguments
//      kind: Statement kind.
const char* statement_str(const Statement_Kind kind);


// General structure for all statements.
//
// Members
//      kind: Classification of the statement.
//      position: Position of the statement.
//
//      expression: Expression if the statement is a expression statement.
//      declaration: Declaration if the statement is a declaration statement.
//      block:
//          statements: Array of statements.
//          statements_length: Number of statements in the block.
//      if:
//          condition: Condition/test part of the if statement.
//          then: Statement to be run if the condition is true.
//          else: Statement to be run if the condition is false.
//      while:
//          condition: Condition/test part of the while statement.
//          body: Statement to be run if the condition is true.
//      return:
//          value: Value to be returned with the return statement.
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


// Factory functions for initializing statements.
//
// File(s): ast.c
//
// Arguments
//      Arguments depends on what kind of statement is desired. Each function
//      will set the kind and position based on given arguments so the needed
//      arguments are the ones in the union where arguments are given based
//      on the statement.
// Returns
//      Pointer to the newly created statement.
AST_Statement* expression_statement(AST_Expression* expression);
AST_Statement* block_statement(array* statements, int statements_length);
AST_Statement* if_statement(AST_Expression* condition, AST_Statement* then, AST_Statement* _else);
AST_Statement* while_statement(AST_Expression* condition, AST_Statement* body);
AST_Statement* break_statement();
AST_Statement* continue_statement();
AST_Statement* return_statement(AST_Expression* value);
AST_Statement* declaration_statement(AST_Declaration* declaration);


// Frees the memory allocated for a statement.
//
// File(s): ast.c
//
// Arguments
//      statement: Statement to be freed.
void statement_free(AST_Statement* statement);



// Function parameter.
//
// Members
//      position: Position of the parameter.
//      identifier: Name of the parameter.
//      specifier: Type of the parameter.
typedef struct Parameter
{
    Position position;
    Token* identifier;
    Type_Specifier specifier;
} Parameter;


// Factory function to initialize new function parameter.
//
// File(s): ast.c
//
// Arguments
//      identifier: Name of the parameter.
//      specifier: Type of the parameter.
// Returns
//      Pointer to the newly created parameter.
Parameter* function_parameter(Token* identifier, Type_Specifier specifier);


// Enumeration of different expression classifications.
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


// Returns a string representation of a expression kind.
//
// File(s): ast.c
//
// Arguments
//      kind: Expression kind.
const char* expression_str(const Expression_Kind kind);


// General structure for expressions.
//
// Members
//      kind: Classification of the expression.
//      position: Position of the expression. 
//      type: Type of the expression.
//      value: Value of the expression.
//
//      identifier: Identifier token if the expression is a variable expression.
//      literal: Literal value if the expression is a literal expression.
//      unary:
//          operator: Token of a unary operator.
//          operand: Operand expression of unary expression.
//      binary:
//          operator: Token of a binary operator.
//          left: Left side expression of binary expression.
//          right: Right side expression of binary expression.
//      assignment:
//          variable: Variable to be assigned with value.
//          value: Value to be assigned to the variable.
//      index:
//          variable: Name of the subscriptable variable.
//          value: Value of the subscript.
//      function:
//          parameters: Array of function parameters.
//          arity: How many arguments the function can take.
//          body: Body of the function containing array of statements.
//      call:
//          variable: Name of the function or callable being called.
//          arguments: Array of arguments passed to the function or callable.
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


// Factory functions to initialize expressions.
//
// File(s): ast.c
//
// Arguments
//      Arguments depends on what kind of expression is desired. Each function
//      will set the kind and position based on given arguments so the needed
//      arguments are the ones in the union where arguments are given based
//      on the statement.
//
//      Type and Value of the expression will be set on later stages of the
//      compilation.
// Returns
//      Pointer to the newly created expression.
AST_Expression* literal_expression(Token* literal);
AST_Expression* unary_expression(Token* _operator, AST_Expression* operand);
AST_Expression* binary_expression(AST_Expression* left, Token* _operator, AST_Expression* right);
AST_Expression* variable_expression(Token* identifier);
AST_Expression* assignment_expression(AST_Expression* variable, AST_Expression* value);
AST_Expression* index_expression(AST_Expression* variable, AST_Expression* value);
AST_Expression* function_expression(array* parameters, int arity, AST_Statement* body);
AST_Expression* call_expression(AST_Expression* variable, array* arguments);
AST_Expression* error_expression();


// Frees the memory allocated for an expression.
//
// File(s): ast.c
//
// Arguments
//      expression: Expression to freed.
void expression_free(AST_Expression* expression);


// Makes a string presentation of parse tree of a expression.
//
// File(s): ast.c
//
// Arguments
//      expression: Expression to be turned into string.
// Returns
//      String presentation of parse tree of the expression.
const char* expression_to_string(const AST_Expression* expression);


// Structure for keeping track of the parser data.
// 
// Members
//      diagnostics: Array of collected diagnostics.
//      position: TODO(timo): Is this needed to anything?
//      index: Index of the current token.
//      tokens: Stream of tokens in an array.
//      current_token: Current token from the token stream.
//      declarations: Array of declarations.
//      panic: If error recovery is needed to execute.
typedef struct Parser
{
    array* diagnostics;
    Position position;
    array* tokens;
    int index;
    Token* current_token;
    array* declarations;
    bool panic;
} Parser;


// Factory function to initialize new parser.
//
// File(s): parser.c
//
// Arguments
//      parser: Pointer to Parser structure.
//      tokens: Stream of tokens to be parsed into abstract syntax tree.
void parser_init(Parser* parser, array* tokens);


// Frees the memory allocated for a parser.
//
// File(s): parser.c
//
// Arguments
//      parser: Parser to be freed.
void parser_free(Parser* parser);


// Main function that turns a stream of tokens into a abstract syntax tree.
// The stream of tokens will be parsed into an array of declarations which
// can be accessed through the field 'declarations' after parsing.
//
// File(s): parser.c
//
// Arguments
//      parser: Pointer to a already initialized Parser.
void parse(Parser* parser);


// Main interface for parsing type specifiers. 
//
// File(s): parser.c
//
// Arguments
//      parser: Pointer to a already initialized Parser.
// Returns
//      Parsed type specifier.
Type_Specifier parse_type_specifier(Parser* parser);


// Main interface for parsing expressions.
//
// File(s): parser.c
//
// Arguments
//      parser: Pointer to a already initialized Parser.
// Returns
//      Pointer to parsed expression.
AST_Expression* parse_expression(Parser* parser);


// Main interface for parsing statements.
//
// File(s): parser.c
//
// Arguments
//      parser: Pointer to a already initialized Parser.
// Returns
//      Pointer to parsed statement.
AST_Statement* parse_statement(Parser* parser);


// Main interface for parsing declarations.
//
// File(s): parser.c
//
// Arguments
//      parser: Pointer to a already initialized Parser.
// Returns
//      Pointer to parsed declaration.
AST_Declaration* parse_declaration(Parser* parser);


// Enumeration of different type classicifations.
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
// Members
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


// Factory functions for initializing types.
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


// Used to tie the type and the value together when resolving expressions
// and having most use when doing constant folding.
//  
// HOX! NOT IN USE AT THE MOMENT.
//
// Members
//      type: Type of the operand.
//      value: Value of the operand.
typedef struct Operand
{
    Type* type;
    Value value;
} Operand;


// Enumeration of different symbol kinds to separate different kind of symbols 
// from each other.
typedef enum Symbol_Kind
{
    SYMBOL_NONE,
    SYMBOL_VARIABLE,
    SYMBOL_PARAMETER,
    SYMBOL_FUNCTION,
    SYMBOL_TEMP,
} Symbol_Kind;


// Symbols state of resolving. This is used if symbols are being resolved
// out of declaration order or if the symbols are being resolved in multiple
// passes.
//
// NOT IN USE AT THE MOMENT
typedef enum Symbol_State
{
    STATE_UNRESOLVED,
    STATE_RESOLVING,
    STATE_RESOLVED,
} Symbol_State;


// Represents symbols in the symbol table.
//
// Members
//      kind: Symbol kind.
//      state: Resolving state of the symbol. NOT USED AT THE MOMENT.
//      scope: Scope which the symbols belongs to.
//      identifier: Identifier/name for the symbol.
//      type: Type of the symbol.
//      value: Value of the symbol.
//
//      offset: Stack offset from the stack frame base.
//      _register: Register where the symbol is allocated. If no register
//                 is allocated, value will be -1.
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
} Symbol;


// Constructor functions for all the used symbols.
//
// File(s): symbol.c
//
// Arguments
//      scope: Pointer to the scope which the symbol belongs to.
//      identifier: Identifier or the name for the symbol.
//      type: Type of the symbol.
// Returns
//      Pointer to the newly created Symbol structure.
Symbol* symbol_variable(Scope* scope, const char* identifier, Type* type);
Symbol* symbol_temp(Scope* scope, const char* identifier, Type* type);
Symbol* symbol_parameter(Scope* scope, const char* identifier, Type* type);
Symbol* symbol_function(Scope* scope, const char* identifier, Type* type);


// Frees the memory allocated for the symbol.
//
// The identifier comes from the AST nodes and is also freed when they are freed.
// The type is free'd by type table if it is primitive type, else symbol has the
// responsibility of freeing the type of the symbol.
//
// File(s): symbol.c
//
// Arguments
//      symbol: Symbol to be freed.
void symbol_free(Symbol* symbol);


// Structure representing scopes in the language. At the moment there is only
// two different scopes in use: global scope and local scopes for functions.
//
// File(s): scope.c
//
// Members
//      name: Name of the scope.
//      offset: Total offset for the scope. Used to reserve memory from the
//              stack for all the local variables.
//      offset_parameter: Total offset for parameters in the scope. Separate
//                        offset is used because parameters live in the other
//                        side of the stack frame than local variables.
//      enclosing: Enclosing scope.
//      symbols: Symbol table containing the symbols in the scope.
struct Scope
{
    const char* name;
    int offset; // alignment
    int offset_parameter;
    Scope* enclosing;
    hashtable* symbols;
};


// Initializes new scope structure.
//
// File(s): scope.c
//
// Arguments
//      enclosing: Pointer to the enclosing scope.
//      name: Name of the scope.
// Returns
//      Pointer to the newly created Scope structure.
Scope* scope_init(Scope* enclosing, const char* name); 


// Frees the memory allocated for a scope.
//
// File(s): scope.c
//
// Arguments
//      scope: Scope to be freed.
void scope_free(Scope* scope);


// Finds symbol from the passed scope.
//
// File(s): scope.c
//
// Arguments
//      scope: Pointer to scope where the symbol will be looked from.
//      identifier: Name of the symbol to be looked from the scope.
// Returns
//      Pointer to symbol if it is found, otherwise NULL.
Symbol* scope_get(const Scope* scope, const char* identifier);


// Finds symbol from the passed scope or one of its enclosing scopes it doesn't
// exist in the passed scope.
//
// File(s): scope.c
//
// Arguments
//      scope: The first scope where the symbol will be looked from.
//      identifier: Identifier of the symbol to be looked from the scope.
// Returns
//      Pointer to the symbol if it is found, otherwise NULL.
Symbol* scope_lookup(const Scope* scope, const char* identifier);


// Declares new symbol into scope and computes the offsets and alignments
// of the symbols in the scope.
//
// Function does not check for already existing symbols.
//
// File(s): scope.c
//
// Arguments
//      scope: Scope where the symbol will be declared.
//      symbol: Symbol to be declared.
void scope_declare(Scope* scope, Symbol* symbol);


// Checks if the scope contains a symbol in its symbol table. The lookup
// will be made based on the identifier of the symbol.
//
// File(s): scope.c
//
// Arguments
//      scope: Scope where the key will be looked up from.
//      identifier: Identifier of the symbol to be looked from the scope.
// Returns
//      Value true if the symbol is found from the scope, otherwise false.
const bool scope_contains(const Scope* scope, const char* identifier);


// Prints the scope info and its symbols. In case of functions there will be
// a recursive function call to print the local scope of the function.
//
// File(s): scope.c
//
// Arguments
//      scope: Pointer to the Scope to be printed.
//      indentation: Starting indentation level. Level will be incremented
//                   by one on every recursive call.
void dump_scope(const Scope* scope, int indentation);


// Resolver handles type checking of the variables and expressions, semantic 
// analysis of the generated abstract syntax tree and creates the global scope 
// with resolved symbols.
//
// At the moment the language has only two different scopes: global and local
// scope of a function. Therefore the scopes are lexical at global level, and
// dynamic at local level.
//  
// File(s): resolver.c
//
// Members
//      diagnostics: Array of collected diagnostics.
//      type_table: Type table with languages primitive data types.
//      global: Global scope of the program.
//      local: Current scope.
//      context:
//          current_function: The name of the current context/scope.
//          not_int_loop: If loop structure is currently being resolved.
//          not_in_function: If function is currently being resolved.
//          returned: If function has already returned or not.
//          return_type: Type of the resolved return value of a function.
typedef struct Resolver
{
    array* diagnostics;
    hashtable* type_table;
    // TODO(timo): Separate the global scope as it's own variable in the top level
    Scope* global;
    Scope* local;

    struct {
        // TODO(timo): Check if we can remove this current_function somehow
        char* current_function; // TODO(timo): This could also be pointer to symbol
        bool not_in_loop;
        bool not_in_function;
        bool not_returned;
        Type* return_type;
    } context;
} Resolver;


// Factory function for initalizing new Resolver.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Pointer to Resolver structure.
//      type_table: Pointer to already initialized type table.
void resolver_init(Resolver* resolver, hashtable* type_table);


// Frees the memory allocated for resolver.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Resolver to be freed.
void resolver_free(Resolver* resolver);


// The main interface for resolving expressions. Expressions will be resolved
// based on their values of the field 'kind'.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      expression: Expression to be resolved.
// Returns
//      The type resolved for the expression.
Type* resolve_expression(Resolver* resolver, AST_Expression* expression);


// The main interface for resolving statements. Statements will be resolved
// based on their values of the field 'kind'.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      statement: Statement to be resolved.
void resolve_statement(Resolver* resolver, AST_Statement* statement);


// The main interface for resolving declarations. Declarations will be resolved
// based on their values of the field 'kind'.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      declaration: Declaration to be resolved.
void resolve_declaration(Resolver* resolver, AST_Declaration* declaration);


// The main interface for resolving type specifiers. Type specifiers will be 
// resolved based on their value itself because the type specifiers are just
// a simple enumeration at this point.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      specifier: Type specifier to be resolved.
// Returns
//      The resolved type for the type specifier.
Type* resolve_type_specifier(Resolver* resolver, Type_Specifier specifier);


// The main interface for resolving generated abstract syntax tree.
//
// File(s): resolver.c
//
// Arguments
//      resolver: Pointer to initialized Resolver.
//      declarations: Array of AST_Declarations to be resolved.
void resolve(Resolver* resolver, array* declarations);


//  Interpreter
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


// Factory function for creating interpreter.
//
// File(s): interpreter.c
//
// Arguments
//      interpreter: Pointer to interpreter structure.
//      global: Global scope with resolved symbols.
void interpreter_init(Interpreter* interpreter, Scope* global);


//
//
// File(s): interpreter.c
//
// Arguments
//      interpreter:
void interpreter_free(Interpreter* interpreter);


//
//
// File(s): interpreter.c
//
// Arguments
//      interpreter:
//      expression:
// Returns
//      The value of the expression.
Value evaluate_expression(Interpreter* interpreter, AST_Expression* expression);


//
//
// File(s): interpreter.c
//
// Arguments
//      interpreter:
//      statement:
void evaluate_statement(Interpreter* interpreter, AST_Statement* statement);


//
//
// File(s): interpreter.c
//
// Arguments
//      interpreter:
//      declaration:
void evaluate_declaration(Interpreter* interpreter, AST_Declaration* declaration);


//
//
// File(s): interpreter.c
//
// Arguments
//      source:
// Returns
//      Value returned by the main program.
const Value interpret(const char* source);


//
//
// File(s): interpreter.c
//
// Arguments
//      path:
// Returns
//      Value returned by the main program.
const Value interpret_from_file(const char* path);


// Enumeration of different kind of operations.
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


// Returns a string representation of Operation
// 
// File(s): instruction.c
//
// Arguments
//      operation: Operation
const char* operation_str(Operation operation);


// NOTE(timo): Address can be a name, a constant or a compiler generated temporary
// These are pretty much the operands used in some literature
//
// TODO(timo): Addresses are not used in Instructions for now
//
// Members
//      name:       program name, pointer to the names symbol table entry where 
//                  all the information of the name is kept
//      constant:   constant value but seems like it can be a variable too?
//      label:      just label
typedef struct Address
{
    union {
        Symbol* name;
        // Value constant; // These can be fetched from the symbol table?
        const char* label;
    };
} Address;


// Represents a single instruction in the intermediate reperesentation. The
// instructions are quads, so they have the operations and maximum of three
// operands.
//
// Members
//      operation: Operation of the instruction.
//      arg1: Address of the first operand of the instruction.
//      arg2: Address of the second operand of the instruction.
//      result: Address of the result of the instruction.
//      size: Used to compute sizes, aligments etc. numerical info.
//      label: Used to save labels e.g. for jump instructions.
typedef struct Instruction 
{
    Operation operation;

    // TODO(timo): All of these could or should be pointers to the symbol table
    char* arg1;
    char* arg2;
    char* result;

    int size;
    const char* label;
} Instruction;


// Factory functions for different kind of instructions.
//
// TODO(timo): I can do more informative docstrings by grouping some of these
// things.
//
// File(s): instruction.c
//
// Arguments
//      Arguments are based on the instruction used. Some instructions have
//      more operands than others.
// Returns
//      Pointer to the newly created Instruction.
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


// Frees the memory allocated for instruction
//
// File(s): instruction.c
//
// Arguments
//      instruction: Instruction to be freed
void instruction_free(Instruction* instruction);


// Prints the instruction to terminal/console
//
// File(s): instruction.c
//
// Arguments
//      instruction: Instruction to be printed
void dump_instruction(Instruction* instruction);


// Prints all instructions from array to terminal/console
//
// File(s): instruction.c
//
// Arguments
//      instructions: Array of instructions to be printed
void dump_instructions(array* instructions);


//  Code in basic block has only one entry point and one exit point, meaning
//  there is no jump destinations inside the block and that only last instruction
//  can start executing next block
//
//  TODO(timo): Basic blocks are not implemented for now.
typedef struct Basic_Block
{
    // start, end?
    // leader?
    array* instructions;
} Basic_Block;


// Enumeration of different kind of contexts used in IR generation.
typedef enum IR_Context_Kind
{
    IR_CONTEXT_NONE,
    IR_CONTEXT_WHILE,
    IR_CONTEXT_IF,
} IR_Context_Kind;


// Context item used to keep track of the correct jump labels in different
// kind of contexts.
//
// Members
//      kind: Classification of the context.
//      while:
//          start_label: Start label of the while loop.
//          exit_label: Exit label of the while loop.
//      if:
//          exit_label: Exit label of the if statement.
//          exit_not_generated: If the exit label is generated or not.
//          new_context: If the new context if allowed or not.
typedef struct IR_Context
{
    IR_Context_Kind kind;

    union {
        struct {
            char* start_label;
            char* exit_label;
        } _while;
        struct {
            char* exit_label;
            bool exit_not_generated;
            bool new_context;
        } _if;
    };
} IR_Context;


// IR Generator is responsible for generating intermediate representation from
// the resolved and annotated abstract syntax tree. The generated instructions
// are quads.
//
// Members
//      instructions: Array of generated instructions.
//      diagnostics: Array of collected diagnostics.
//      label: Running number for general labels.
//      temp: Running number for labels of the temporary variables.
//      gloal: Global scope.
//      local: Current local scope.
//      contexts: Stack of IR Contexts.
//      current_context: Current context in the IR generation.
typedef struct IR_Generator
{
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


// Factory function for initializing new IR generator.
//
// File(s): ir_generator.c
//
// Arguments
//      generator: Pointer to generator to be initialized.
//      global: Pointer to global symbol table with resolved symbols.
void ir_generator_init(IR_Generator* generator, Scope* global);


// Frees the memory allocated for the IR generator.
//
// File(s): ir_generator.c
//
// Arguments
//      generator: IR generator to be freed.
void ir_generator_free(IR_Generator* generator);


// Generates intermediate representation of the resolved and annotated abstract
// syntax tree. The main interface used with IR generator.
//
// The instructions will be saved into the 'instructions' member of the
// Code_Generator strcture.
//
// File(s): ir_generator.c
//
// Arguments
//      generator: Pointer to initialized IR generator.
//      declarations: Array of declarations which the IR instructions will
//                    be generated from.
void ir_generate(IR_Generator* generator, array* declarations);


// Generates intermediate representation the expression.
//
// File(s): ir_generator.c
//
// Arguments
//      generator: Pointer to initialized IR generator.
//      expression: Expression to be generated.
char* ir_generate_expression(IR_Generator* generator, AST_Expression* expression);


// Generates intermediate representation of the statement.
//
// File(s): ir_generator.c
//
// Arguments
//      generator: Pointer to initialized IR generator.
//      statement: Statement to be generated.
void ir_generate_statement(IR_Generator* generator, AST_Statement* statement);


// Generates intermediate representation of the declaration.
//
// File(s): ir_generator.c
//
// Arguments
//      generator: Pointer to initialized IR generator.
//      declaration: Declaration to be generated.
void ir_generate_declaration(IR_Generator* generator, AST_Declaration* declaration);


// Prints all the instructions from the array of instructions.
//
// Arguments
//      instructions: Array of instructions.
void dump_instructions(array* instructions);


// Code generator is responsible of generating target machine instructions
// from the intermediate representation. At the moment the created instructions
// are x86-64 or AMD64 instructions.
//
// Members
//      output: Handle to the output file.
//      instructions: Array of instructions generated by the IR generator.
//      diagnostics: Array of collected diagnostics.
//      global: Global scope.
//      local: Current local scope.
//      asm_file: Name of the output file.
//      destination: Current destination register.
//      source: Current source register.
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
    // NOTE(timo): These are not used for anything at the moment.
    int destination;
    int source;
} Code_Generator;


// Factory function for initializing new code generator.
//
// File(s): code_generator.c
//
// Arguments
//      generator: Address of the code generator to be initialized.
//      global: Global scope with resolved symbols.
//      instructions: Array of instructions from the IR generator.
void code_generator_init(Code_Generator* generator, Scope* global, array* instructions);


// Generates the target machine instructions from the intermediate 
// representation. Main interface used with the code generator.
//
// File(s): code_generator.c
//
// Arguments
//      generator: Pointer to initialized code generator.
void code_generate(Code_Generator* generator);


// Frees the memory allocated for code generator.
//
// File(s): code_generator.c
//
// Arguments
//      generator: Code generator to be freed.
void code_generator_free(Code_Generator* generator);


#endif
