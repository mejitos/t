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
        not_error = false;
    }
}


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


int main(int argc, char** argv)
{
    printf("Running lexer tests...");

    Lexer lexer;

    // Test for white space
    lexer_init(&lexer, "  \n    \t   \n    ");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_position((*lexer.tokens)->position, 3, 5, 3, 5);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer_free(&lexer);

    // Test for comments
    lexer_init(&lexer, "# asdasdasdasdasd");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_position((*lexer.tokens)->position, 1, 18, 1, 18);
    assert_token(*lexer.tokens, TOKEN_EOF);
    lexer_free(&lexer);

    lexer_init(&lexer, "# asdasdasdasdasd\n   \t \n");
    lex(&lexer);

    assert(lexer.tokens_length == 1);
    assert_position((*lexer.tokens)->position, 3, 1, 3, 1);
    assert_token(*(lexer.tokens), TOKEN_EOF);
    lexer_free(&lexer);

    // Test for basic arithmetic operators
    lexer_init(&lexer, "+ - * /");
    lex(&lexer);

    assert(lexer.tokens_length == 5);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_PLUS);
    assert_position((*lexer.tokens)->position, 1, 3, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_MINUS);
    assert_position((*lexer.tokens)->position, 1, 5, 1, 5);
    assert_token(*lexer.tokens++, TOKEN_MULTIPLY);
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_DIVIDE);
    assert_position((*lexer.tokens)->position, 1, 8, 1, 8);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 4; // "unwinding" of the array
    lexer_free(&lexer);

    // Test for equality and comparison operators
    lexer_init(&lexer, "== != < <= > >=");
    lex(&lexer);

    assert(lexer.tokens_length == 7);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 2);
    assert_token(*lexer.tokens++, TOKEN_IS_EQUAL);
    assert_position((*lexer.tokens)->position, 1, 4, 1, 5);
    assert_token(*lexer.tokens++, TOKEN_NOT_EQUAL);
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN);
    assert_position((*lexer.tokens)->position, 1, 9, 1, 10);
    assert_token(*lexer.tokens++, TOKEN_LESS_THAN_EQUAL);
    assert_position((*lexer.tokens)->position, 1, 12, 1, 12);
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN);
    assert_position((*lexer.tokens)->position, 1, 14, 1, 15);
    assert_token(*lexer.tokens++, TOKEN_GREATER_THAN_EQUAL);
    assert_position((*lexer.tokens)->position, 1, 16, 1, 16);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 6; // unwinding of the array
    lexer_free(&lexer);

    // Test for assignment operators and related symbols
    lexer_init(&lexer, ": = :=");
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_COLON);
    assert_position((*lexer.tokens)->position, 1, 3, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_EQUAL);
    assert_position((*lexer.tokens)->position, 1, 5, 1, 6);
    assert_token(*lexer.tokens++, TOKEN_COLON_ASSIGN);
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 3; // unwinding of the array
    lexer_free(&lexer);

    // Test for grouping / separators
    lexer_init(&lexer, "( ) [ ] { } , ;");
    lex(&lexer);

    assert(lexer.tokens_length == 9);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token(*lexer.tokens++, TOKEN_LEFT_PARENTHESIS);
    assert_position((*lexer.tokens)->position, 1, 3, 1, 3);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_PARENTHESIS);
    assert_position((*lexer.tokens)->position, 1, 5, 1, 5);
    assert_token(*lexer.tokens++, TOKEN_LEFT_BRACKET);
    assert_position((*lexer.tokens)->position, 1, 7, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_BRACKET);
    assert_position((*lexer.tokens)->position, 1, 9, 1, 9);
    assert_token(*lexer.tokens++, TOKEN_LEFT_CURLYBRACE);
    assert_position((*lexer.tokens)->position, 1, 11, 1, 11);
    assert_token(*lexer.tokens++, TOKEN_RIGHT_CURLYBRACE);
    assert_position((*lexer.tokens)->position, 1, 13, 1, 13);
    assert_token(*lexer.tokens++, TOKEN_COMMA);
    assert_position((*lexer.tokens)->position, 1, 15, 1, 15);
    assert_token(*lexer.tokens++, TOKEN_SEMICOLON);
    assert_position((*lexer.tokens)->position, 1, 16, 1, 16);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 8; // unwinding of the array
    lexer_free(&lexer);

    // Test for misc operators
    lexer_init(&lexer, "=>");
    lex(&lexer);

    assert(lexer.tokens_length == 2);
    assert_token(*lexer.tokens++, TOKEN_ARROW);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 1; // unwinding of the array
    lexer_free(&lexer);

    // Test for lexing integer literals
    lexer_init(&lexer, "7 42");
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 1);
    assert_token_integer(*lexer.tokens++, 7);
    assert_position((*lexer.tokens)->position, 1, 3, 1, 4);
    assert_token_integer(*lexer.tokens++, 42);
    assert_position((*lexer.tokens)->position, 1, 5, 1, 5);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 2; // unwinding of the array
    lexer_free(&lexer);

    // Test for boolean literals
    lexer_init(&lexer, "true false");
    lex(&lexer);

    assert(lexer.tokens_length == 3);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 4);
    assert_token_boolean(*lexer.tokens++, true);
    assert_position((*lexer.tokens)->position, 1, 6, 1, 10);
    assert_token_boolean(*lexer.tokens++, false);
    assert_position((*lexer.tokens)->position, 1, 11, 1, 11);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 2; // unwinding of the array
    lexer_free(&lexer);

    // Test for keywords / reserved words
    lexer_init(&lexer, "if then else while do break continue return and or not int bool");
    lex(&lexer);

    assert(lexer.tokens_length == 14);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 2);
    assert_token(*lexer.tokens++, TOKEN_IF);
    assert_position((*lexer.tokens)->position, 1, 4, 1, 7);
    assert_token(*lexer.tokens++, TOKEN_THEN);
    assert_position((*lexer.tokens)->position, 1, 9, 1, 12);
    assert_token(*lexer.tokens++, TOKEN_ELSE);
    assert_position((*lexer.tokens)->position, 1, 14, 1, 18);
    assert_token(*lexer.tokens++, TOKEN_WHILE);
    assert_position((*lexer.tokens)->position, 1, 20, 1, 21);
    assert_token(*lexer.tokens++, TOKEN_DO);
    assert_position((*lexer.tokens)->position, 1, 23, 1, 27);
    assert_token(*lexer.tokens++, TOKEN_BREAK);
    assert_position((*lexer.tokens)->position, 1, 29, 1, 36);
    assert_token(*lexer.tokens++, TOKEN_CONTINUE);
    assert_position((*lexer.tokens)->position, 1, 38, 1, 43);
    assert_token(*lexer.tokens++, TOKEN_RETURN);
    assert_position((*lexer.tokens)->position, 1, 45, 1, 47);
    assert_token(*lexer.tokens++, TOKEN_AND);
    assert_position((*lexer.tokens)->position, 1, 49, 1, 50);
    assert_token(*lexer.tokens++, TOKEN_OR);
    assert_position((*lexer.tokens)->position, 1, 52, 1, 54);
    assert_token(*lexer.tokens++, TOKEN_NOT);
    assert_position((*lexer.tokens)->position, 1, 56, 1, 58);
    assert_token(*lexer.tokens++, TOKEN_INT);
    assert_position((*lexer.tokens)->position, 1, 60, 1, 63);
    assert_token(*lexer.tokens++, TOKEN_BOOL);
    assert_position((*lexer.tokens)->position, 1, 64, 1, 64);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 13; // unwinding of the array
    lexer_free(&lexer);

    // Test for identifiers
    lexer_init(&lexer, "foo _bar FOOBAR");
    lex(&lexer);

    assert(lexer.tokens_length == 4);
    assert_position((*lexer.tokens)->position, 1, 1, 1, 3);
    assert_token_identifier(*lexer.tokens++, "foo");
    assert_position((*lexer.tokens)->position, 1, 5, 1, 8);
    assert_token_identifier(*lexer.tokens++, "_bar");
    assert_position((*lexer.tokens)->position, 1, 10, 1, 15);
    assert_token_identifier(*lexer.tokens++, "FOOBAR");
    assert_position((*lexer.tokens)->position, 1, 16, 1, 16);
    assert_token(*lexer.tokens, TOKEN_EOF);

    lexer.tokens -= 3; // unwinding of the array
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
