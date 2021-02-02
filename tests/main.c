#include "../src/t.h"


static bool not_error = true;


void assert_token(Token* token, Token_Kind kind)
{
    if (token->kind == kind) return;
    else 
    {
        printf("\n\tInvalid token kind\n");
        not_error = false;
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

    if (not_error) printf("OK\n");
    else printf("\n");

    return 0;
}
