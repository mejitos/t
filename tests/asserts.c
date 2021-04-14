#include "asserts.h"


void assert_base(Test_Runner* runner, bool assertion, const char* message, ...)
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


void assert_position(Test_Runner* runner, Position position, int line_start, int column_start, int line_end, int column_end)
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


void assert_token(Test_Runner* runner, Token* token, Token_Kind kind, const char* lexeme)
{
    assert_base(runner, token->kind == kind,
        "Invalid token kind '%d', expected '%d'", token->kind, kind);
    assert_base(runner, strcmp(token->lexeme, lexeme) == 0,
        "Invalid lexeme '%s', expected '%s'", token->lexeme, lexeme);
}


void assert_expression_str(Test_Runner* runner, char* result, const char* expected)
{
    assert_base(runner, strcmp(result, expected) == 0,
        "Invalid expression parse tree '%s', expected '%s'", result, expected);
}


void assert_expression(Test_Runner* runner, Expression_Kind actual, Expression_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid expression kind '%s', expected '%s'", expression_str(actual), expression_str(expected));
}


void assert_parameter(Test_Runner* runner, Parameter* parameter, const char* identifier, Type_Specifier specifier)
{
    assert_token(runner, parameter->identifier, TOKEN_IDENTIFIER, identifier);
    assert_base(runner, parameter->specifier == specifier,
        "Invalid type specifier '%s', expected '%s'", type_specifier_str(parameter->specifier), type_specifier_str(specifier));
}


void assert_statement(Test_Runner* runner, Statement_Kind actual, Statement_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid statement kind '%s', expected '%s'", statement_str(actual), statement_str(expected));
}


void assert_declaration(Test_Runner* runner, Declaration_Kind actual, Declaration_Kind expected)
{
    assert_base(runner, actual == expected,
        "Invalid declaration kind '%s', expected '%s'", declaration_str(actual), declaration_str(expected));
}


void assert_type_specifier(Test_Runner* runner, Type_Specifier actual, Type_Specifier expected)
{
    assert_base(runner, actual == expected,
        "Invalid type specifier '%s', expected '%s'", type_specifier_str(actual), type_specifier_str(expected));
}
