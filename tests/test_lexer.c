#include "../src/t.h"
#include "tests.h"


static bool not_error = true;


void assert_token(Token* token, Token_Kind kind, const char* lexeme)
{
    if (token->kind != kind)
    {
        printf("\n\t\tInvalid token kind '%d', expected '%d'", token->kind, kind);
        not_error = false;
    }
    if (strcmp(token->lexeme, lexeme) != 0)
    {
        printf("\n\t\tInvalid lexeme '%s', expected '%s'", token->lexeme, lexeme);
        not_error = false;
    }
}


// TODO(timo): This should actually take the token as an argument so we can
// give better information about WHAT is being positioned wrong when testing.
// Though this means that there should be position assertions for everything
// that has positions but so be it.
void assert_position(Position position, int line_start, int column_start, int line_end, int column_end)
{
    if (line_start != 0)
    {
        if (position.line_start != line_start)
        {
            printf("\n\t\tInvalid starting line '%d', expected '%d'", position.line_start, line_start);
            not_error = false;
        }
    }
    if (column_start != 0)
    {
        if (position.column_start != column_start)
        {
            printf("\n\t\tInvalid starting column '%d', expected '%d'", position.column_start, column_start);
            not_error = false;
        }
    }
    if (line_end != 0)
    {
        if (position.line_end != line_end)
        {
            printf("\n\t\tInvalid ending line '%d', expected '%d'", position.line_end, line_end);
            not_error = false;
        }
    }
    if (column_end != 0)
    {
        if (position.column_end != column_end)
        {
            printf("\n\t\tInvalid ending column '%d', expected '%d'", position.column_end, column_end);
            not_error = false;
        }
    }
}


static void test_skip_whitespace()
{
    printf("\tSkipping whitespace...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "  \n    \t   \n    ");
    lex(&lexer);

    assert(lexer.tokens->length == 1);
    assert_position(((Token*)(*lexer.tokens->items))->position, 3, 5, 3, 5);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_skip_comments()
{
    printf("\tSkipping comments...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "# asdasdasdasdasd");
    lex(&lexer);

    assert(lexer.tokens->length == 1);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 18, 1, 18);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer_free(&lexer);

    // ----

    lexer_init(&lexer, "# asdasdasdasdasd\n   \t \n");
    lex(&lexer);

    assert(lexer.tokens->length == 1);
    assert_position(((Token*)(*lexer.tokens->items))->position, 3, 1, 3, 1);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");
    lexer_free(&lexer);

    // Test for basic arithmetic operators
    lexer_init(&lexer, "+ - * /");
    lex(&lexer);

    assert(lexer.tokens->length == 5);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 3);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 5);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 8, 1, 8);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 4; // "unwinding" of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_comparison_operators()
{
    printf("\tComparison operators...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "== != < <= > >=");
    lex(&lexer);

    assert(lexer.tokens->length == 7);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 2);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 4, 1, 5);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_NOT_EQUAL, "!=");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LESS_THAN, "<");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 9, 1, 10);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 12, 1, 12);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 14, 1, 15);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN_EQUAL, ">=");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 16, 1, 16);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 6; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_assignment_operators()
{
    printf("\tAssignment operators etc...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, ": = :=");
    lex(&lexer);

    assert(lexer.tokens->length == 4);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_COLON, ":");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 3);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 6);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_COLON_ASSIGN, ":=");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 3; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_punctuations()
{
    printf("\tPunctuations / grouping / separators...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "( ) [ ] { } , ;");
    lex(&lexer);

    assert(lexer.tokens->length == 9);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LEFT_PARENTHESIS, "(");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 3);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_RIGHT_PARENTHESIS, ")");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 5);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LEFT_BRACKET, "[");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 7, 1, 7);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_RIGHT_BRACKET, "]");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 9, 1, 9);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LEFT_CURLYBRACE, "{");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 11, 1, 11);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_RIGHT_CURLYBRACE, "}");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 13, 1, 13);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_COMMA, ",");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 15, 1, 15);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_SEMICOLON, ";");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 16, 1, 16);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 8; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_misc_operators()
{
    printf("\tMisc operators...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "=>");
    lex(&lexer);

    assert(lexer.tokens->length == 2);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_ARROW, "=>");
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 1; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_integer_literals()
{
    printf("\tInteger literals...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "7 42");
    lex(&lexer);

    assert(lexer.tokens->length == 3);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 1);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_INTEGER_LITERAL, "7");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 3, 1, 4);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_INTEGER_LITERAL, "42");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 5);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 2; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_boolean_literals()
{
    printf("\tBoolean literals...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "true false");
    lex(&lexer);

    assert(lexer.tokens->length == 3);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 4);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_BOOLEAN_LITERAL, "true");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 6, 1, 10);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_BOOLEAN_LITERAL, "false");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 11, 1, 11);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 2; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_keywords()
{
    printf("\tKeywords / reserved words...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "if then else while do break continue return and or not int bool");
    lex(&lexer);

    assert(lexer.tokens->length == 14);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 2);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IF, "if");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 4, 1, 7);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_THEN, "then");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 9, 1, 12);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_ELSE, "else");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 14, 1, 18);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_WHILE, "while");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 20, 1, 21);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_DO, "do");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 23, 1, 27);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_BREAK, "break");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 29, 1, 36);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_CONTINUE, "continue");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 38, 1, 43);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_RETURN, "return");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 45, 1, 47);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_AND, "and");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 49, 1, 50);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_OR, "or");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 52, 1, 54);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_NOT, "not");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 56, 1, 58);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_INT, "int");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 60, 1, 63);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_BOOL, "bool");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 64, 1, 64);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 13; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_identifiers()
{
    printf("\tIdentifiers...");
    not_error = true;

    Lexer lexer;

    lexer_init(&lexer, "foo _bar FOOBAR");
    lex(&lexer);

    assert(lexer.tokens->length == 4);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 3);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "foo");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 5, 1, 8);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "_bar");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 10, 1, 15);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "FOOBAR");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 16, 1, 16);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 3; // unwinding of the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_identifier_edge_cases()
{
    printf("\tIdentifier edge cases...");
    not_error = true;

    Lexer lexer;

    //TODO(timo): Here the 17if should probably cause an error
    lexer_init(&lexer, "_while doo integer 17if");
    lex(&lexer);

    assert(lexer.tokens->length == 6);
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 1, 1, 6);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "_while");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 8, 1, 10);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "doo");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 12, 1, 18);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IDENTIFIER, "integer");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 20, 1, 21);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_INTEGER_LITERAL, "17");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 22, 1, 23);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IF, "if");
    assert_position(((Token*)(*lexer.tokens->items))->position, 1, 24, 1, 24);
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 5; // unwind the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_sequential_arithmetic_operators()
{
    printf("\tSequential arithmetic operators...");
    not_error = true;

    Lexer lexer;
    // TODO(timo): These tests show that with the operators, the lexer doesn't 
    // actually follow the maximum munching rule when it comes to operators.
    lexer_init(&lexer, "++ -- ** // +-*/");
    lex(&lexer);

    assert(lexer.tokens->length == 13);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_PLUS, "+");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MINUS, "-");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_MULTIPLY, "*");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_DIVIDE, "/");
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 12; // unwind the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_sequential_comparison_operators()
{
    printf("\tSequential comparison operators...");
    not_error = true;

    Lexer lexer;

    // TODO(timo): These tests show that with the operators, the lexer doesn't 
    // actually follow the maximum munching rule when it comes to operators.
    lexer_init(&lexer, "=== ==== << >> <== ==> =!=");
    lex(&lexer);

    assert(lexer.tokens->length == 15);
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LESS_THAN, "<");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LESS_THAN, "<");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_IS_EQUAL, "==");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_GREATER_THAN, ">");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_EQUAL, "=");
    assert_token((Token*)*lexer.tokens->items++, TOKEN_NOT_EQUAL, "!=");
    assert_token((Token*)*lexer.tokens->items, TOKEN_EOF, "<EoF>");

    lexer.tokens->items -= 14; // unwind the array
    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_diagnose_invalid_character()
{
    printf("\tDiagnose invalid character...");

    Lexer lexer;
    char* message;
    Diagnostic* diagnostic;
    
    // general
    lexer_init(&lexer, "foo.bar");
    lex(&lexer);

    assert(lexer.tokens->length == 3);
    assert(lexer.diagnostics->length == 1);

    diagnostic = lexer.diagnostics->items[0];
    
    message = ":LEXER - SyntaxError: Invalid character '.'";

    assert(strcmp(diagnostic->message, message) == 0);
    assert_position(diagnostic->position, 1, 4, 1, 4);

    lexer_free(&lexer);

    // expecting '=' after '!'
    lexer_init(&lexer, "foo ! 1");
    lex(&lexer);

    assert(lexer.tokens->length == 3);
    assert(lexer.diagnostics->length == 1);

    diagnostic = lexer.diagnostics->items[0];
    
    message = ":LEXER - SyntaxError: Invalid character ' ', expected '='";

    assert(strcmp(diagnostic->message, message) == 0);
    assert_position(diagnostic->position, 1, 5, 0, 0);

    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


static void test_multiple_diagnostics()
{
    printf("\tDiagnosing multiple diagnostics...");
    not_error = true;

    Lexer lexer;
    char* message;
    Diagnostic* diagnostic;

    lexer_init(&lexer, "foo.bar ! foo&baz");
    lex(&lexer);

    assert(lexer.tokens->length == 5);
    assert(lexer.diagnostics->length == 3);

    diagnostic = lexer.diagnostics->items[0];
    message = ":LEXER - SyntaxError: Invalid character '.'";
    assert(strcmp(diagnostic->message, message) == 0);
    assert_position(diagnostic->position, 1, 4, 0, 0);

    diagnostic = lexer.diagnostics->items[1];
    message = ":LEXER - SyntaxError: Invalid character ' ', expected '='";
    assert(strcmp(diagnostic->message, message) == 0);
    assert_position(diagnostic->position, 1, 9, 0, 0);

    diagnostic = lexer.diagnostics->items[2];
    message = ":LEXER - SyntaxError: Invalid character '&'";
    assert(strcmp(diagnostic->message, message) == 0);
    assert_position(diagnostic->position, 1, 14, 0, 0);

    lexer_free(&lexer);

    if (not_error) printf("PASSED\n");
    else printf("\n");
}


void test_lexer()
{
    printf("Running lexer tests...\n");

    test_skip_whitespace();
    test_skip_comments();
    test_comparison_operators();
    test_assignment_operators();
    test_punctuations();
    test_misc_operators();
    test_integer_literals();
    test_boolean_literals();
    test_keywords();
    test_identifiers();
    test_identifier_edge_cases();
    test_sequential_arithmetic_operators();
    test_sequential_comparison_operators();

    test_diagnose_invalid_character();
    test_multiple_diagnostics();
}
