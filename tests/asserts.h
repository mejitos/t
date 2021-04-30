//
// TODO(timo): Filedocstring
//

#ifndef t_asserts_h
#define t_asserts_h

#include "../src/t.h"
#include "tests.h"


// Basic assertion from which every other specific assertion will be built.
// If the assertion is true, nothing will happend and if the assertion is
// false, a Test_Error will be created and at the end of the test run, the
// error will be shown.
//
// Arguments
//      runner: Initialized test runner.
//      assertion: Assertion with value of true or false.
//      message: Message for the user if the assertion was false.
//      ...: Optional format arguments for the message.
void assert_base(Test_Runner* runner, const bool assertion, const char* message, ...);


// Assertion for Position structs.
//
// Arguments
//      runner: Initialized test runner.
//      position: Actual position.
//      line_start: Expected starting line of the position.
//      column_start: Expected starting column of the position.
//      line_end: Expected ending line of the position.
//      column_end: Expected ending column of the position.
void assert_position(Test_Runner* runner, const Position position, const int line_start, const int column_start, const int line_end, const int column_end);


// Assertion for Token structs.
//
// Arguments
//      runner: Initialized test runner.
//      token: Actual token.
//      kind: Expected token kind.
//      lexeme: Expected lexeme of the token.
void assert_token(Test_Runner* runner, const Token* token, const Token_Kind kind, const char* lexeme);


// Assertion for comparing two strings together.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual string.
//      expected: Expected string.
void assert_string(Test_Runner* runner, const char* actual, const char* expected);


// Assertion for expressions.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual expression kind.
//      expected: Expected expression kind.
void assert_expression(Test_Runner* runner, const Expression_Kind actual, const Expression_Kind expected);


// Assertion for parameters.
//
// Arguments
//      runner: Initialized test runner.
//      parameter: Actual parameter.
//      identifier: Expected name of the parameter.
//      specifier: Expected type specifier of the parameter.
void assert_parameter(Test_Runner* runner, const Parameter* parameter, const char* identifier, const Type_Specifier specifier);


// Assertion for statements.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual statement kind.
//      expected: Expected statement kind.
void assert_statement(Test_Runner* runner, const Statement_Kind actual, const Statement_Kind expected);


// Assertion for declarations.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual declaration kind.
//      expected: Expected declaration kind.
void assert_declaration(Test_Runner* runner, const Declaration_Kind actual, const Declaration_Kind expected);


// Assertion for type specifiers.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual type specifier.
//      expected: Expected type specifier.
void assert_type_specifier(Test_Runner* runner, const Type_Specifier actual, const Type_Specifier expected);


// Assertion for types.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual kind of the type.
//      expected: Expected kind of the type.
void assert_type(Test_Runner* runner, const Type_Kind actual, const Type_Kind expected);


// Assertion for values. This can be used for both integers and booleans, since
// booleans are handled as integers in the C language.
//
// Arguments
//      runner: Initialized test runner.
//      actual: Actual value.
//      expected_type: Expected type of the value.
//      expected_value: Expected value of the type.
void assert_value(Test_Runner* runner, const Value actual, const Value_Type expected_type, const int expected_value);


// Assertion for instructions.
//
// Arguments
//      runner: Initialized test runner.
//      instruction: Actual instruction.
//      expected_operation: Expected operation of the instruction.
void assert_instruction(Test_Runner* runner, const Instruction* instruction, const Operation expected_operation);


#endif
