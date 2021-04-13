#include "../src/t.h"
#include "asserts.h"
#include "tests.h"


static bool not_error = true;


static void test_skip_whitespace(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "  \n    \t   \n    ");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 1, 
                "Invalid number of tokens '%d' expected 1", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 3, 5, 3, 5);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_skip_comments_1(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "# asdasdasdasdasd");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 1, 
                "Invalid number of tokens '%d' expected 1", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 18, 1, 18);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_skip_comments_2(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "# asdasdasdasdasd\n   \t \n");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 1, 
                "Invalid number of tokens '%d' expected 1", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 3, 1, 3, 1);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_arithmetic_operators(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "+ - * /");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 5, 
                "Invalid number of tokens '%d' expected 5", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 3);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 5);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 8, 1, 8);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 4; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_comparison_operators(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "== != < <= > >=");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 7, 
                "Invalid number of tokens '%d' expected 7", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 2);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 4, 1, 5);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_NOT_EQUAL, "!=");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LESS_THAN, "<");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 9, 1, 10);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 12, 1, 12);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 14, 1, 15);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN_EQUAL, ">=");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 16, 1, 16);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 6; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_assignment_operators(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, ": = :=");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 4,
                "Invalid number of tokens '%d' expected 4", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_COLON, ":");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 3);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 6);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_COLON_ASSIGN, ":=");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 3; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_punctuations(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "( ) [ ] { } , ;");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 9,
                "Invalid number of tokens '%d' expected 9", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LEFT_PARENTHESIS, "(");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 3);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_RIGHT_PARENTHESIS, ")");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 5);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LEFT_BRACKET, "[");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_RIGHT_BRACKET, "]");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 9, 1, 9);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LEFT_CURLYBRACE, "{");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 11, 1, 11);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_RIGHT_CURLYBRACE, "}");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 13, 1, 13);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_COMMA, ",");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 15, 1, 15);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_SEMICOLON, ";");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 16, 1, 16);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 8; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_misc_operators(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "=>");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 2,
                "Invalid number of tokens '%d' expected 2", lexer.tokens->length);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_ARROW, "=>");
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 1; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_integer_literals(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "7 42");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 3,
                "Invalid number of tokens '%d' expected 3", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_INTEGER_LITERAL, "7");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 4);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_INTEGER_LITERAL, "42");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 5);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 2; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_boolean_literals(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "true false");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 3,
                "Invalid number of tokens '%d' expected 3", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 4);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_BOOLEAN_LITERAL, "true");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 6, 1, 10);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_BOOLEAN_LITERAL, "false");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 11, 1, 11);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 2; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_keywords(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "if then else while do break continue return and or not int bool");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 14,
                "Invalid number of tokens '%d' expected 14", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 2);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IF, "if");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 4, 1, 7);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_THEN, "then");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 9, 1, 12);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_ELSE, "else");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 14, 1, 18);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_WHILE, "while");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 20, 1, 21);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_DO, "do");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 23, 1, 27);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_BREAK, "break");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 29, 1, 36);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_CONTINUE, "continue");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 38, 1, 43);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_RETURN, "return");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 45, 1, 47);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_AND, "and");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 49, 1, 50);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_OR, "or");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 52, 1, 54);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_NOT, "not");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 56, 1, 58);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_INT, "int");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 60, 1, 63);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_BOOL, "bool");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 64, 1, 64);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 13; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_identifiers(Test_Runner* runner)
{
    Lexer lexer;

    lexer_init(&lexer, "foo _bar FOOBAR");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 4,
                "Invalid number of tokens '%d' expected 4", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 3);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "foo");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 8);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "_bar");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 10, 1, 15);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "FOOBAR");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 16, 1, 16);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 3; // unwinding of the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_identifier_edge_cases(Test_Runner* runner)
{
    Lexer lexer;

    //TODO(timo): Here the 17if should probably cause an error
    lexer_init(&lexer, "_while doo integer 17if");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 6,
                "Invalid number of tokens '%d' expected 6", lexer.tokens->length);
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 6);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "_while");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 8, 1, 10);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "doo");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 12, 1, 18);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "integer");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 20, 1, 21);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_INTEGER_LITERAL, "17");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 22, 1, 23);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IF, "if");
    assert_position(runner, ((Token*)(*lexer.tokens->items))->position, 1, 24, 1, 24);
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 5; // unwind the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_sequential_arithmetic_operators(Test_Runner* runner)
{
    Lexer lexer;
    // TODO(timo): These tests show that with the operators, the lexer doesn't 
    // actually follow the maximum munching rule when it comes to operators.
    lexer_init(&lexer, "++ -- ** // +-*/");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 13,
                "Invalid number of tokens '%d' expected 13", lexer.tokens->length);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 12; // unwind the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_sequential_comparison_operators(Test_Runner* runner)
{
    Lexer lexer;

    // TODO(timo): These tests show that with the operators, the lexer doesn't 
    // actually follow the maximum munching rule when it comes to operators.
    lexer_init(&lexer, "=== ==== << >> <== ==> =!=");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 15,
                "Invalid number of tokens '%d' expected 15", lexer.tokens->length);
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LESS_THAN, "<");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LESS_THAN, "<");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_token(runner, (Token*)*lexer.tokens->items++, TOKEN_NOT_EQUAL, "!=");
    assert_token(runner, (Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 14; // unwind the array
    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_diagnose_invalid_character(Test_Runner* runner)
{
    Lexer lexer;
    
    // general
    lexer_init(&lexer, "foo.bar");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 3,
                "Invalid number of tokens '%d' expected 3", lexer.tokens->length);
    assert_base(runner, lexer.diagnostics->length == 1,
                "Invalid number of diagnostics '%d' expected 1", lexer.diagnostics->length);

    Diagnostic* diagnostic = lexer.diagnostics->items[0];
    char* message = ":LEXER - SyntaxError: Invalid character '.'";

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
                "Invalid message '%s', expected '%s'", diagnostic->message, message);
    assert_position(runner, diagnostic->position, 1, 4, 1, 4);

    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_diagnose_invalid_not_equal(Test_Runner* runner)
{
    // expecting '=' after '!'
    Lexer lexer;

    lexer_init(&lexer, "foo ! 1");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 3,
                "Invalid number of tokens '%d' expected 3", lexer.tokens->length);
    assert_base(runner, lexer.diagnostics->length == 1,
                "Invalid number of diagnostics '%d' expected 1", lexer.diagnostics->length);

    Diagnostic* diagnostic = lexer.diagnostics->items[0];
    char* message = ":LEXER - SyntaxError: Invalid character ' ', expected '='";

    assert_base(runner, strcmp(diagnostic->message, message) == 0,
                "Invalid message '%s', expected '%s'", diagnostic->message, message);
    assert_position(runner, diagnostic->position, 1, 5, 1, 6);

    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


static void test_multiple_diagnostics(Test_Runner* runner)
{
    Lexer lexer;
    char* message;
    Diagnostic* diagnostic;

    lexer_init(&lexer, "foo.bar ! foo&baz");
    lex(&lexer);

    assert_base(runner, lexer.tokens->length == 5,
                "Invalid number of tokens '%d' expected 5", lexer.tokens->length);
    assert_base(runner, lexer.diagnostics->length == 3,
                "Invalid number of diagnostics '%d' expected 3", lexer.diagnostics->length);

    diagnostic = lexer.diagnostics->items[0];
    message = ":LEXER - SyntaxError: Invalid character '.'";
    assert_base(runner, strcmp(diagnostic->message, message) == 0,
                "Invalid message '%s', expected '%s'", diagnostic->message, message);
    assert_position(runner, diagnostic->position, 1, 4, 1, 4);

    diagnostic = lexer.diagnostics->items[1];
    message = ":LEXER - SyntaxError: Invalid character ' ', expected '='";
    assert_base(runner, strcmp(diagnostic->message, message) == 0,
                "Invalid message '%s', expected '%s'", diagnostic->message, message);
    assert_position(runner, diagnostic->position, 1, 9, 1, 10);

    diagnostic = lexer.diagnostics->items[2];
    message = ":LEXER - SyntaxError: Invalid character '&'";
    assert_base(runner, strcmp(diagnostic->message, message) == 0,
                "Invalid message '%s', expected '%s'", diagnostic->message, message);
    assert_position(runner, diagnostic->position, 1, 14, 1, 14);

    lexer_free(&lexer);
    
    if (runner->error) runner->failed++;
    else runner->passed++;
}


Test_Set* lexer_test_set()
{
    Test_Set* set = test_set("Lexer");
    
    // Base cases
    array_push(set->tests, test_case("Skip whitespace", test_skip_whitespace));
    array_push(set->tests, test_case("Skip comments 1", test_skip_comments_1));
    array_push(set->tests, test_case("Skip comments 2", test_skip_comments_2));
    array_push(set->tests, test_case("Arithmetic operators", test_arithmetic_operators));
    array_push(set->tests, test_case("Comparison operators", test_comparison_operators));
    array_push(set->tests, test_case("Assignment operators", test_assignment_operators));
    array_push(set->tests, test_case("Misc operators", test_misc_operators));
    array_push(set->tests, test_case("Integer literals", test_integer_literals));
    array_push(set->tests, test_case("Boolean literals", test_boolean_literals));
    array_push(set->tests, test_case("Keywords", test_keywords));
    array_push(set->tests, test_case("Identifiers", test_identifiers));

    // Edge cases
    array_push(set->tests, test_case("Identifier edge cases", test_identifier_edge_cases));
    array_push(set->tests, test_case("Sequential arithmetic operators", test_sequential_arithmetic_operators));
    array_push(set->tests, test_case("Sequential comparison operators", test_sequential_comparison_operators));

    // Diagnostics
    array_push(set->tests, test_case("Diagnose invalid character", test_diagnose_invalid_character));
    array_push(set->tests, test_case("Diagnose invalid not equal operator", test_diagnose_invalid_not_equal));
    array_push(set->tests, test_case("Diagnose multiple diagnostics", test_multiple_diagnostics));

    set->length = set->tests->length;

    return set;
}
