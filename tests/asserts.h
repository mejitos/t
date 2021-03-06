#ifndef t_asserts_h
#define t_asserts_h

#include "../src/t.h"

// Positions
void assert_position(Position position, int line_start, int column_start, int line_end, int column_end);
// void assert_position_token(Token* token, int line_start, int column_start, int line_end, int column_end);
// void assert_position_expression(AST_Expression* expression, int line_start, int column_start, int line_end, int column_end);
// void assert_position_statement(AST_Statement* statement, int line_start, int column_start, int line_end, int column_end);
// void assert_position_declaration(AST_Declaration* declaration, int line_start, int column_start, int line_end, int column_end);

// Tokens
void assert_token(Token* token, Token_Kind kind, const char* lexeme);

// Expressions

// Statements

// Declarations

#endif
