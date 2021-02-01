#include "../src/t.h"


static bool not_error = true;


void assert_token(Token* token, Token_Kind kind)
{
    if (token->kind == kind) return;
    else 
    {
        printf("\tInvalid token kind\n");
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


    if (not_error) printf("OK\n");
    else printf("\n");

    return 0;
}
