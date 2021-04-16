#ifndef t_asserts_h
#define t_asserts_h

#include "../src/t.h"
#include "tests.h"


void assert_base(Test_Runner* runner, bool assertion, const char* message, ...);

void assert_position(Test_Runner* runner, Position position, int line_start, int column_start, int line_end, int column_end);
// void assert_position_token(Token* token, int line_start, int column_start, int line_end, int column_end);
// void assert_position_expression(Test_Runner* runner, AST_Expression* expression, int line_start, int column_start, int line_end, int column_end);
// void assert_position_statement(Test_Runner* runner, AST_Statement* statement, int line_start, int column_start, int line_end, int column_end);
// void assert_position_declaration(Test_Runner* runner, AST_Declaration* declaration, int line_start, int column_start, int line_end, int column_end);

void assert_token(Test_Runner* runner, Token* token, Token_Kind kind, const char* lexeme);
void assert_expression_str(Test_Runner* runner, char* result, const char* expected);
void assert_expression(Test_Runner* runner, Expression_Kind actual, Expression_Kind expected);
void assert_parameter(Test_Runner* runner, Parameter* parameter, const char* identifier, Type_Specifier specifier);
void assert_statement(Test_Runner* runner, Statement_Kind actual, Statement_Kind expected);
void assert_declaration(Test_Runner* runner, Declaration_Kind actual, Declaration_Kind expected);
void assert_type_specifier(Test_Runner* runner, Type_Specifier actual, Type_Specifier expected);
void assert_type(Test_Runner* runner, Type_Kind actual, Type_Kind expected);
void assert_value(Test_Runner* runner, Value actual, Value_Type expected_type, int expected_value);


#endif
