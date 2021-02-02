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
    lexer_init(&lexer, "  \n    \t   \n\r    ");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);

    lexer_free(&lexer);

    // Test for comments
    lexer_init(&lexer, "# asdasdasdasdasd");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    lexer_free(&lexer);

    lexer_init(&lexer, "# asdasdasdasdasd\n   \t \n");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    lexer_free(&lexer);

    // Test for basic arithmetic operators
    lexer_init(&lexer, "+ - * /");
    lex(&lexer);

    assert(lexer.tokens_length == 5);
    assert_token(*lexer.tokens++, TOKEN_PLUS);
    assert_token(*lexer.tokens++, TOKEN_MINUS);
    assert_token(*lexer.tokens++, TOKEN_MULTIPLY);
    assert_token(*lexer.tokens++, TOKEN_DIVIDE);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 4; // "unwinding" of the array
    lexer_free(&lexer);

    // Test for equality and comparison operators
    lexer_init(&lexer, "== != < <= > >=");
    lex(&lexer);

    assert(lexer.tokens_length == 7);
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL);
    assert_token(*lexer.tokens++, TOKEN_NOT_EQUAL);
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN);
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN_EQUAL);
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN);
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN_EQUAL);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 6; // unwinding of the array
    lexer_free(&lexer);

    // Test for assignment operators and related symbols
    lexer_init(&lexer, ": = :=");
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_token(*lexer.tokens++, TOKEN_COLON);
    assert_token(*lexer.tokens++, TOKEN_EQUAL);
    assert_token(*lexer.tokens++, TOKEN_COLON_ASSIGN);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 3;
    lexer_free(&lexer);

    // Test for grouping / separators
    lexer_init(&lexer, "( ) [ ] { } , ;");
    lex(&lexer);

    assert(lexer.tokens_length == 9);
    assert_token(*lexer.tokens++, TOKEN_LEFT_PARENTHESIS);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_PARENTHESIS);
    assert_token(*lexer.tokens++, TOKEN_LEFT_BRACKET);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_BRACKET);
    assert_token(*lexer.tokens++, TOKEN_LEFT_CURLYBRACE);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_CURLYBRACE);
    assert_token(*lexer.tokens++, TOKEN_COMMA);
    assert_token(*lexer.tokens++, TOKEN_SEMICOLON);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 8;
    lexer_free(&lexer);

    // Test for misc operators
    lexer_init(&lexer, "=>");
    lex(&lexer);

    assert(lexer.tokens_length == 2);
    assert_token(*lexer.tokens++, TOKEN_ARROW);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 1;
    lexer_free(&lexer);

    // Test for lexing integer literals
    lexer_init(&lexer, "7 42");
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_token_integer(*lexer.tokens++, 7);
    assert_token_integer(*lexer.tokens++, 42);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 2;
    lexer_free(&lexer);

    // Test for boolean literals
    lexer_init(&lexer, "true false");
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_token_boolean(*lexer.tokens++, true);
    assert_token_boolean(*lexer.tokens++, false);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 2;
    lexer_free(&lexer);

    // Test for keywords / reserved words
    lexer_init(&lexer, "if then else while do break continue return and or not int bool");
    lex(&lexer);

    assert(lexer.tokens_length == 14);
    assert_token(*lexer.tokens++, TOKEN_IF);
    assert_token(*lexer.tokens++, TOKEN_THEN);
    assert_token(*lexer.tokens++, TOKEN_ELSE);
    assert_token(*lexer.tokens++, TOKEN_WHILE);
    assert_token(*lexer.tokens++, TOKEN_DO);
    assert_token(*lexer.tokens++, TOKEN_BREAK);
    assert_token(*lexer.tokens++, TOKEN_CONTINUE);
    assert_token(*lexer.tokens++, TOKEN_RETURN);
    assert_token(*lexer.tokens++, TOKEN_AND);
    assert_token(*lexer.tokens++, TOKEN_OR);
    assert_token(*lexer.tokens++, TOKEN_NOT);
    assert_token(*lexer.tokens++, TOKEN_INT);
    assert_token(*lexer.tokens++, TOKEN_BOOL);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 13;
    lexer_free(&lexer);

    // Test for identifiers
    lexer_init(&lexer, "foo _bar FOOBAR");
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_token_identifier(*lexer.tokens++, "foo");
    assert_token_identifier(*lexer.tokens++, "_bar");
    assert_token_identifier(*lexer.tokens++, "FOOBAR");
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 3;
    lexer_free(&lexer);

    // TODO(timo): Test case for sequencial operators. Do we allow them
    // of give error from them. e.g. is '<<' allowed and we just scan two
    // less than tokens, or do we give error for that? Probably we should
    // just allow that, since it isn't really lexers job to differentiate
    // these things. Parser checks for the correct syntax
    
    // TODO(timo): Test case for correctly scanning identifiers e.g. "_while"
    // and "integer" are both allowed identifiers according to maximum munch rule

    // TODO(timo): Testing of error 

    if (not_error) printf("OK\n");
    else printf("\n");

    return 0;
}
