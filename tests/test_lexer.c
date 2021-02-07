#include "../src/t.h"
#include "tests.h"


static bool not_error = true;


void assert_token(Token* token, Token_Kind kind, const char* lexeme)
{
    if (token->kind != kind)
    {
        printf("\n\tInvalid token kind '%d', expected '%d'", token->kind, kind);
        not_error = false;
    }
    if (strcmp(token->lexeme, lexeme) != 0)
    {
        printf("\n\tInvalid lexeme '%s', expected '%s'", token->lexeme, lexeme);
        not_error = false;
    }
}


// TODO(timo): This should actually take the token as an argument so we can
// give better information about WHAT is being positioned wrong when testing.
// Though this means that there should be position assertions for everything
// that has positions but so be it.
void assert_position(Position position, int line_start, int column_start, int line_end, int column_end)
{
    if (line_start)
    {
        if (position.line_start != line_start)
        {
            printf("\n\tInvalid starting line '%d', expected '%d'", position.line_start, line_start);
            not_error = false;
        }
    }
    if (column_start)
    {
        if (position.column_start != column_start)
        {
            printf("\n\tInvalid starting column '%d', expected '%d'", position.column_start, column_start);
            not_error = false;
        }
    }
    if (line_end)
    {
        if (position.line_end != line_end)
        {
            printf("\n\tInvalid ending line '%d', expected '%d'", position.line_end, line_end);
            not_error = false;
        }
    }
    if (column_end)
    {
        if (position.column_end != column_end)
        {
            printf("\n\tInvalid ending column '%d', expected '%d'", position.column_end, column_end);
            not_error = false;
        }
    }
}


void test_lexer()
{
    printf("Running lexer tests...");

    Lexer lexer;

    // Test for white space
    lexer_init(&lexer, "  \n    \t   \n    ");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_position((*lexer.tokens)->position, 3, 5, 3, 5);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer_free(&lexer);

    // Test for comments
    lexer_init(&lexer, "# asdasdasdasdasd");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_position((*lexer.tokens)->position, 1, 18, 1, 18);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");
    lexer_free(&lexer);
    
    // ----
    lexer_init(&lexer, "# asdasdasdasdasd\n   \t \n");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_position((*lexer.tokens)->position, 3, 1, 3, 1);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");
    lexer_free(&lexer);

    // Test for basic arithmetic operators
    lexer_init(&lexer, "+ - * /");
    lex(&lexer);

    assert(lexer.tokens_length == 5);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_PLUS, "+");
    assert_position((*lexer.tokens)->position, 1, 3, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_MINUS, "-");
    assert_position((*lexer.tokens)->position, 1, 5, 1, 5);
    assert_token(*lexer.tokens++, TOKEN_MULTIPLY, "*");
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_DIVIDE, "/");
    assert_position((*lexer.tokens)->position, 1, 8, 1, 8);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 4; // "unwinding" of the array
    lexer_free(&lexer);
    
    // Test for equality and comparison operators
    lexer_init(&lexer, "== != < <= > >=");
    lex(&lexer);

    assert(lexer.tokens_length == 7);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 2);
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL, "==");
    assert_position((*lexer.tokens)->position, 1, 4, 1, 5);
    assert_token(*lexer.tokens++, TOKEN_NOT_EQUAL, "!=");
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN, "<");
    assert_position((*lexer.tokens)->position, 1, 9, 1, 10);
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_position((*lexer.tokens)->position, 1, 12, 1, 12);
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN, ">");
    assert_position((*lexer.tokens)->position, 1, 14, 1, 15);
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN_EQUAL, ">=");
    assert_position((*lexer.tokens)->position, 1, 16, 1, 16);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 6; // unwinding of the array
    lexer_free(&lexer);

    // Test for assignment operators and related symbols
    lexer_init(&lexer, ": = :=");
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_COLON, ":");
    assert_position((*lexer.tokens)->position, 1, 3, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_EQUAL, "=");
    assert_position((*lexer.tokens)->position, 1, 5, 1, 6);
    assert_token(*lexer.tokens++, TOKEN_COLON_ASSIGN, ":=");
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 3; // unwinding of the array
    lexer_free(&lexer);

    // Test for grouping / separators
    // printf("\n\tTesting separators");
    lexer_init(&lexer, "( ) [ ] { } , ;");
    lex(&lexer);

    assert(lexer.tokens_length == 9);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_LEFT_PARENTHESIS, "(");
    assert_position((*lexer.tokens)->position, 1, 3, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_PARENTHESIS, ")");
    assert_position((*lexer.tokens)->position, 1, 5, 1, 5);
    assert_token(*lexer.tokens++, TOKEN_LEFT_BRACKET, "[");
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_BRACKET, "]");
    assert_position((*lexer.tokens)->position, 1, 9, 1, 9);
    assert_token(*lexer.tokens++, TOKEN_LEFT_CURLYBRACE, "{");
    assert_position((*lexer.tokens)->position, 1, 11, 1, 11);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_CURLYBRACE, "}");
    assert_position((*lexer.tokens)->position, 1, 13, 1, 13);
    assert_token(*lexer.tokens++, TOKEN_COMMA, ",");
    assert_position((*lexer.tokens)->position, 1, 15, 1, 15);
    assert_token(*lexer.tokens++, TOKEN_SEMICOLON, ";");
    assert_position((*lexer.tokens)->position, 1, 16, 1, 16);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 8; // unwinding of the array
    lexer_free(&lexer);

    // Test for misc operators
    lexer_init(&lexer, "=>");
    lex(&lexer);

    assert(lexer.tokens_length == 2);
    assert_token(*lexer.tokens++, TOKEN_ARROW, "=>");
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 1; // unwinding of the array
    lexer_free(&lexer);

    // Test for lexing integer literals
    // printf("\n\tTesting integer literals");
    lexer_init(&lexer, "7 42");
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_INTEGER_LITERAL, "7");
    assert_position((*lexer.tokens)->position, 1, 3, 1, 4);
    assert_token(*lexer.tokens++, TOKEN_INTEGER_LITERAL, "42");
    assert_position((*lexer.tokens)->position, 1, 5, 1, 5);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 2; // unwinding of the array
    lexer_free(&lexer);

    // Test for boolean literals
    // printf("\n\tTesting boolean literals");
    lexer_init(&lexer, "true false");
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 4);
    assert_token(*lexer.tokens++, TOKEN_BOOLEAN_LITERAL, "true");
    assert_position((*lexer.tokens)->position, 1, 6, 1, 10);
    assert_token(*lexer.tokens++, TOKEN_BOOLEAN_LITERAL, "false");
    assert_position((*lexer.tokens)->position, 1, 11, 1, 11);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 2; // unwinding of the array
    lexer_free(&lexer);

    // Test for keywords / reserved words
    // printf("\n\tTesting keywords / reserved words");
    lexer_init(&lexer, "if then else while do break continue return and or not int bool");
    lex(&lexer);

    assert(lexer.tokens_length == 14);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 2);
    assert_token(*lexer.tokens++, TOKEN_IF, "if");
    assert_position((*lexer.tokens)->position, 1, 4, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_THEN, "then");
    assert_position((*lexer.tokens)->position, 1, 9, 1, 12);
    assert_token(*lexer.tokens++, TOKEN_ELSE, "else");
    assert_position((*lexer.tokens)->position, 1, 14, 1, 18);
    assert_token(*lexer.tokens++, TOKEN_WHILE, "while");
    assert_position((*lexer.tokens)->position, 1, 20, 1, 21);
    assert_token(*lexer.tokens++, TOKEN_DO, "do");
    assert_position((*lexer.tokens)->position, 1, 23, 1, 27);
    assert_token(*lexer.tokens++, TOKEN_BREAK, "break");
    assert_position((*lexer.tokens)->position, 1, 29, 1, 36);
    assert_token(*lexer.tokens++, TOKEN_CONTINUE, "continue");
    assert_position((*lexer.tokens)->position, 1, 38, 1, 43);
    assert_token(*lexer.tokens++, TOKEN_RETURN, "return");
    assert_position((*lexer.tokens)->position, 1, 45, 1, 47);
    assert_token(*lexer.tokens++, TOKEN_AND, "and");
    assert_position((*lexer.tokens)->position, 1, 49, 1, 50);
    assert_token(*lexer.tokens++, TOKEN_OR, "or");
    assert_position((*lexer.tokens)->position, 1, 52, 1, 54);
    assert_token(*lexer.tokens++, TOKEN_NOT, "not");
    assert_position((*lexer.tokens)->position, 1, 56, 1, 58);
    assert_token(*lexer.tokens++, TOKEN_INT, "int");
    assert_position((*lexer.tokens)->position, 1, 60, 1, 63);
    assert_token(*lexer.tokens++, TOKEN_BOOL, "bool");
    assert_position((*lexer.tokens)->position, 1, 64, 1, 64);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 13; // unwinding of the array
    lexer_free(&lexer);

    // Test for identifiers
    lexer_init(&lexer, "foo _bar FOOBAR");
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_IDENTIFIER, "foo");
    assert_position((*lexer.tokens)->position, 1, 5, 1, 8);
    assert_token(*lexer.tokens++, TOKEN_IDENTIFIER, "_bar");
    assert_position((*lexer.tokens)->position, 1, 10, 1, 15);
    assert_token(*lexer.tokens++, TOKEN_IDENTIFIER, "FOOBAR");
    assert_position((*lexer.tokens)->position, 1, 16, 1, 16);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 3; // unwinding of the array
    lexer_free(&lexer);
    
    // Test case for correctly scanning identifiers
    lexer_init(&lexer, "_while doo integer 17if");
    lex(&lexer);

    assert(lexer.tokens_length == 6);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 6);
    assert_token(*lexer.tokens++, TOKEN_IDENTIFIER, "_while");
    assert_position((*lexer.tokens)->position, 1, 8, 1, 10);
    assert_token(*lexer.tokens++, TOKEN_IDENTIFIER, "doo");
    assert_position((*lexer.tokens)->position, 1, 12, 1, 18);
    assert_token(*lexer.tokens++, TOKEN_IDENTIFIER, "integer");
    assert_position((*lexer.tokens)->position, 1, 20, 1, 21);
    assert_token(*lexer.tokens++, TOKEN_INTEGER_LITERAL, "17");
    assert_position((*lexer.tokens)->position, 1, 22, 1, 23);
    assert_token(*lexer.tokens++, TOKEN_IF, "if");
    assert_position((*lexer.tokens)->position, 1, 24, 1, 24);
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 5;
    lexer_free(&lexer);

    // Test case for sequencial arithmetic operators
    // TODO(timo): These tests show that with the operators, the lexer doesn't 
    // actually follow the maximum munching rule when it comes to operators.
    lexer_init(&lexer, "++ -- ** // +-*/");
    lex(&lexer);

    assert(lexer.tokens_length == 13);
    assert_token(*lexer.tokens++, TOKEN_PLUS, "+");
    assert_token(*lexer.tokens++, TOKEN_PLUS, "+");
    assert_token(*lexer.tokens++, TOKEN_MINUS, "-");
    assert_token(*lexer.tokens++, TOKEN_MINUS, "-");
    assert_token(*lexer.tokens++, TOKEN_MULTIPLY, "*");
    assert_token(*lexer.tokens++, TOKEN_MULTIPLY, "*");
    assert_token(*lexer.tokens++, TOKEN_DIVIDE, "/");
    assert_token(*lexer.tokens++, TOKEN_DIVIDE, "/");
    assert_token(*lexer.tokens++, TOKEN_PLUS, "+");
    assert_token(*lexer.tokens++, TOKEN_MINUS, "-");
    assert_token(*lexer.tokens++, TOKEN_MULTIPLY, "*");
    assert_token(*lexer.tokens++, TOKEN_DIVIDE, "/");
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 12;
    lexer_free(&lexer);

    // Test case for sequencial comparison operators
    // TODO(timo): These tests show that with the operators, the lexer doesn't 
    // actually follow the maximum munching rule when it comes to operators.
    lexer_init(&lexer, "=== ==== << >> <== ==> =!=");
    lex(&lexer);

    assert(lexer.tokens_length == 15);
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL, "==");
    assert_token(*lexer.tokens++, TOKEN_EQUAL, "=");
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL, "==");
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL, "==");
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN, "<");
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN, "<");
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN, ">");
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN, ">");
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN_EQUAL, "<=");
    assert_token(*lexer.tokens++, TOKEN_EQUAL, "=");
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL, "==");
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN, ">");
    assert_token(*lexer.tokens++, TOKEN_EQUAL, "=");
    assert_token(*lexer.tokens++, TOKEN_NOT_EQUAL, "!=");
    assert_token(*lexer.tokens, TOKEN_EOF, "<EoF>");

    lexer.tokens -= 14;
    lexer_free(&lexer);

    // TODO(timo): Testing of error diagnosing
    //      - unknown character

    if (not_error) printf("OK\n");
    else printf("\n");
}
