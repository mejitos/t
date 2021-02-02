#include "../src/t.h"


static bool not_error = true;


void assert_token(Token* token, Token_Kind kind)
{
    if (token->kind == kind) return;
    else 
    {
        printf("\n\tInvalid token kind '%d', expected '%d'", token->kind, kind);
        not_error = false;
    }
}


void assert_token_integer(Token* token, int value)
{
    assert_token(token, TOKEN_INTEGER_LITERAL);

    if(token->integer_value == value) return;
    else 
    {
        printf("\n\tInvalid integer value '%d', expected '%d'", token->integer_value, value);
        not_error = false;
    }
}


void assert_token_boolean(Token* token, bool value)
{
    assert_token(token, TOKEN_BOOLEAN_LITERAL);

    if (token->boolean_value == value) return;
    else
    {
        printf("\n\tInvalid boolean value");
        not_error = false;
    }
}


void assert_token_identifier(Token* token, const char* identifier)
{
    assert_token(token, TOKEN_IDENTIFIER);

    if (strcmp(token->identifier, identifier) == 0) return;
    else
    {
        printf("\n\tInvalid identifier '%s', expected '%s'", token->identifier, identifier);
    }
}


int main(int argc, char** argv)
{
    printf("Running lexer tests...");

    Lexer lexer;

    // Test for white space
    lexer = (Lexer){ .stream = "  \n    \t   \n\r    " };
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens);

    // Test for comments
    lexer = (Lexer){ .stream = "# asdasdasdasdasd" };
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens);

    lexer = (Lexer){ .stream = "# asdasdasdasdasd\n   \t \n" };
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens);

    // Test for basic arithmetic operators
    lexer = (Lexer){ .stream = "+ - * /" };
    lex(&lexer);

    assert(lexer.tokens_length == 5);
    assert_token(*(lexer.tokens), TOKEN_PLUS);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_MINUS);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_MULTIPLY);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_DIVIDE);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 4);

    // Test for equality and comparison operators
    lexer = (Lexer){ .stream = "== != < <= > >=" };
    lex(&lexer);

    assert(lexer.tokens_length == 7);
    assert_token(*(lexer.tokens), TOKEN_IS_EQUAL);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_NOT_EQUAL);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_LESS_THAN);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_LESS_THAN_EQUAL);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_GREATER_THAN);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_GREATER_THAN_EQUAL);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 6);

    // Test for assignment operators and related symbols
    lexer = (Lexer){ .stream = ": = :=" };
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_token(*(lexer.tokens), TOKEN_COLON);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EQUAL);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_COLON_ASSIGN);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 3);

    // Test for grouping / separators
    lexer = (Lexer){ .stream = "( ) [ ] { } , ;" };
    lex(&lexer);

    assert(lexer.tokens_length == 9);
    assert_token(*(lexer.tokens), TOKEN_LEFT_PARENTHESIS);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_RIGHT_PARENTHESIS);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_LEFT_BRACKET);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_RIGHT_BRACKET);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_LEFT_CURLYBRACE);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_RIGHT_CURLYBRACE);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_COMMA);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_SEMICOLON);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 8);

    // Test for misc operators
    lexer = (Lexer){ .stream = "=>" };
    lex(&lexer);

    assert(lexer.tokens_length == 2);
    assert_token(*(lexer.tokens), TOKEN_ARROW);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 1);

    // Test for lexing integer literals
    lexer = (Lexer){ .stream = "7 42" };
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_token_integer(*(lexer.tokens), 7);
    free(*(lexer.tokens++));
    assert_token_integer(*(lexer.tokens), 42);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 2);

    // Test for boolean literals
    lexer = (Lexer){ .stream = "true false" };
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_token_boolean(*(lexer.tokens), true);
    free(*(lexer.tokens++));
    assert_token_boolean(*(lexer.tokens), false);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 2);

    // Test for keywords / reserved words
    lexer = (Lexer){ .stream = "if then else while do break continue return and or not int bool" };
    lex(&lexer);

    assert(lexer.tokens_length == 14);
    assert_token(*(lexer.tokens), TOKEN_IF);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_THEN);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_ELSE);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_WHILE);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_DO);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_BREAK);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_CONTINUE);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_RETURN);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_AND);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_OR);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_NOT);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_INT);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_BOOL);
    free(*(lexer.tokens++));
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*(lexer.tokens));
    free(lexer.tokens - 13);

    // Test for identifiers
    lexer = (Lexer){ .stream = "foo _bar FOOBAR" };
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_token_identifier(*(lexer.tokens), "foo");
    free((char*)(*lexer.tokens)->identifier);
    free(*lexer.tokens++);
    assert_token_identifier(*(lexer.tokens), "_bar");
    free((char*)(*lexer.tokens)->identifier);
    free(*lexer.tokens++);
    assert_token_identifier(*(lexer.tokens), "FOOBAR");
    free((char*)(*lexer.tokens)->identifier);
    free(*lexer.tokens++);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    free(*lexer.tokens);
    free(lexer.tokens - 3);

    if (not_error) printf("OK\n");
    else printf("\n");

    return 0;
}
