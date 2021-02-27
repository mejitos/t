#include "t.h"




static char* temp(IR_Generator* generator)
{
    char* t = malloc(sizeof (char) * 12 + 1);
    snprintf(t, 13, "_t%d", generator->temp++);

    return t;
}


static char* label(IR_Generator* generator)
{
    char* l = malloc(sizeof (char) * 12 + 1);
    snprintf(l, 13, "_l%d", generator->label++);

    return l;
}


static void emit_literal_expression(IR_Generator* generator, AST_Expression* expression)
{
    switch (expression->literal.literal->kind)
    {
        case TOKEN_INTEGER_LITERAL:
            // fprintf(generator->output_file, "%d", value.integer); 
            // printf("%s := %d\n", temp(), expression->literal.literal->lexeme); 
            // printf("%s := %s\n", temp(), expression->literal.literal->lexeme); 
            printf("_t%d := %s\n", generator->temp++, expression->literal.literal->lexeme); 
            break;
        default:
            // TODO(timo): Error
            break;
    }
}


// void emit_expression(IR_Generator* generator, AST_Expression* expression)
char* emit_expression(IR_Generator* generator, AST_Expression* expression)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            // printf("%s := %s\n", temp(), expression->literal.literal->lexeme);
            // emit_literal_expression(generator, expression);
            return (char*)expression->literal.literal->lexeme;
            // printf("%s", expression->literal.literal->lexeme);
            // break;
        case EXPRESSION_BINARY:
        {
            // printf("%s := ", temp());
            // printf("_t%d := ", generator->temp++); 

            char* left = emit_expression(generator, expression->binary.left); 
            char* right = emit_expression(generator, expression->binary.right);
            char* operator;

            // NOTE(timo): When we move this into here, we get the correct ordering of the temp labels
            char* t = temp(generator);

            // emit_expression(generator, expression->binary.left); 

            switch (expression->binary._operator->kind)
            {
                case TOKEN_PLUS:
                    operator = " + ";
                    // printf(" + ");
                    break;
                case TOKEN_MINUS:
                    operator = " - ";
                    // printf(" - ");
                    break;
                case TOKEN_MULTIPLY:
                    operator = " * ";
                    // printf(" * ");
                    break;
                case TOKEN_DIVIDE:
                    operator = " / ";
                    // printf(" / ");
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }

            printf("%s := %s %s %s\n", t, left, operator, right);
            // emit_expression(generator, expression->binary.right);

            return t;
        }
        break;
        default:
            // TODO(timo): Error
            break;
    }
}


void generate(IR_Generator* generator)
{
    /*
    if ((generator->output_file = fopen("ir.tac", "w")) == NULL)
    {
        printf("Unable to open file 'ir.tac'\n");
        exit(1);
    }

    fclose(output_file);
    */
}
