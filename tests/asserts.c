//
// TODO(timo): Filedocstring
//

#include "asserts.h"


void assert_base(Test_Runner* runner, const bool assertion, const char* message, ...)
{
    if (! assertion)
    {
        char* buffer; size_t buffer_length;
        FILE* out = open_memstream(&buffer, &buffer_length);

        va_list args;
        va_start(args, message);

        vfprintf(out, message, args);
        va_end(args);
        fclose(out);

        array_push(runner->errors, test_error(runner->current_set->description,
                                              runner->current_case->description,
                                              buffer));
        runner->error = true;
    }
}


void assert_position(Test_Runner* runner, const Position position, const int line_start, const int column_start, const int line_end, const int column_end)
{
    assert_base(runner, (line_start != 0 && position.line_start == line_start),
        "Invalid starting line '%d', expected '%d'", position.line_start, line_start);
    assert_base(runner, (column_start != 0 && position.column_start == column_start),
        "Invalid starting column '%d', expected '%d'", position.column_start, column_start);
    assert_base(runner, (line_end != 0 && position.line_end == line_end), 
        "Invalid ending line '%d', expected '%d'", position.line_end, line_end);
    assert_base(runner, (column_end != 0 && position.column_end == column_end), 
        "Invalid ending column '%d', expected '%d'", position.column_end, column_end);
}


void assert_token(Test_Runner* runner, const Token* token, const Token_Kind kind, const char* lexeme)
{
    assert_base(runner, token->kind == kind,
        "Invalid token kind '%d', expected '%d'", token->kind, kind);
    assert_base(runner, strcmp(token->lexeme, lexeme) == 0,
        "Invalid lexeme '%s', expected '%s'", token->lexeme, lexeme);
}


void assert_string(Test_Runner* runner, const char* actual, const char* expected)
{
    assert_base(runner, strcmp(actual, expected) == 0,
        "Invalid string '%s', expected '%s'", actual, expected);
}


void assert_expression(Test_Runner* runner, const Expression_Kind actual, const Expression_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid expression kind '%s', expected '%s'", expression_str(actual), expression_str(expected));
}


void assert_parameter(Test_Runner* runner, const Parameter* parameter, const char* identifier, const Type_Specifier specifier)
{
    assert_token(runner, parameter->identifier, TOKEN_IDENTIFIER, identifier);
    assert_base(runner, parameter->specifier == specifier,
        "Invalid type specifier '%s', expected '%s'", type_specifier_str(parameter->specifier), type_specifier_str(specifier));
}


void assert_statement(Test_Runner* runner, const Statement_Kind actual, const Statement_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid statement kind '%s', expected '%s'", statement_str(actual), statement_str(expected));
}


void assert_declaration(Test_Runner* runner, const Declaration_Kind actual, const Declaration_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid declaration kind '%s', expected '%s'", declaration_str(actual), declaration_str(expected));
}


void assert_type_specifier(Test_Runner* runner, const Type_Specifier actual, const Type_Specifier expected)
{
    assert_base(runner, actual == expected,
        "Invalid type specifier '%s', expected '%s'", type_specifier_str(actual), type_specifier_str(expected));
}


void assert_type(Test_Runner* runner, const Type_Kind actual, const Type_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid type '%s', expected '%s'", type_as_string(actual), type_as_string(expected));
}


void assert_value(Test_Runner* runner, const Value actual, const Value_Type expected_type, const int expected_value) {
    assert_base(runner, actual.type == expected_type,
        "Invalid value type '%s', expected '%s'", value_str(actual.type), value_str(expected_type));
    assert_base(runner, actual.integer == expected_value,
        "Invalid value '%d', expected '%d'", actual.integer, expected_value);
}


void assert_instruction(Test_Runner* runner, const Instruction* instruction, const Operation expected_operation)
{
    assert_base(runner, instruction->operation == expected_operation,
        "Invalid instruction '%s', expected '%s'", operation_str(instruction->operation), operation_str(expected_operation));
}
