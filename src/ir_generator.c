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


// void generate_expression(IR_Generator* generator, AST_Expression* expression)
char* generate_expression(IR_Generator* generator, AST_Expression* expression)
{
    // TODO(timo): This should probably return the instruction structures
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
        {
            // generate_literal_expression(generator, expression);
            
            // TODO(timo): Is this much actually needed? In my opinion setting all the literals
            // into their own temporary variables is just too much.
            /*
            char* t= temp(generator);
            char* value = (char*)expression->literal.literal->lexeme;
            printf("\t%s := %s\n", t, value);
            return t;
            */

            return (char*)expression->literal.literal->lexeme;
        }
        case EXPRESSION_UNARY:
        {
            char* operand = generate_expression(generator, expression->unary.operand);
            char* operator;
            char* t = temp(generator);

            switch (expression->unary._operator->kind)
            {
                case TOKEN_MINUS:
                    operator = " -";
                    break;
                case TOKEN_NOT:
                    operator = " not ";
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }

            printf("\t%s := %s%s\n", t, operator, operand);

            return t;
        }
        case EXPRESSION_BINARY:
        {
            char* left = generate_expression(generator, expression->binary.left); 
            char* right = generate_expression(generator, expression->binary.right);
            char* operator = (char*)expression->binary._operator->lexeme;
            // char* operator;

            // NOTE(timo): When we move this into here, we get the correct ordering of the temp labels
            char* t = temp(generator);
            
            /*
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
                case TOKEN_LESS_THAN:
                    operator = " < ";

                    // add the if false goto _Ln

                    break;
                default:
                    // TODO(timo): Error
                    break;
            }
            */

            printf("\t%s := %s %s %s\n", t, left, operator, right);

            // return instruction_binary_add(operator, left, right);
            // return instruction_binary_sub(operator, left, right);
            // return instruction_binary_mul(operator, left, right);
            // return instruction_binary_div(operator, left, right);
            
            return t;
        }
        case EXPRESSION_VARIABLE:
        {
            // TODO(timo): Is this much actually needed? In my opinion setting even the variables 
            // into their own temporary variables is just too much.
            /*
            char* t= temp(generator);
            char* value = (char*)expression->literal.literal->lexeme;
            printf("\t%s := %s\n", t, value);
            return t;
            */

            char* variable = (char*)expression->identifier->lexeme;
            // TODO(timo): Do I have to return or do anything else with the variable expressions
            return variable;
        }
        case EXPRESSION_ASSIGNMENT:
        {
            // This is pretty much same as the copy instruction since we just access
            // the variable and save it to the target variable or send it to the target
            //
            // Assignment happens pretty much only to the named variable so there is no
            // need for temp variables
            char* variable = (char*)expression->assignment.variable->identifier->lexeme;
            // char* variable = generate_expression(generator, expression->assignment.variable);
            char* value = generate_expression(generator, expression->assignment.value);
            printf("\t%s := %s\n", variable, value);

            return variable;
        }
        case EXPRESSION_INDEX:
        {
            break;
        }
        case EXPRESSION_FUNCTION:
        {
            // TODO(timo): Does the function declaration handle the beginnning and ending?
            // It probably is job of the expression, if we think about a little into the
            // future and to anonymous functions
            printf("\tfunction_begin N\n");
            generate_statement(generator, expression->function.body);
            printf("\tfunction_end\n");
            break;
        }
        case EXPRESSION_CALL:
        {
            // Push the params to the stack/registers
            array* arguments = expression->call.arguments;
            int release = 0;

            for (int i = arguments->length - 1; i >= 0; i--)
            {
                AST_Expression* argument = (AST_Expression*)arguments->items[i];
                release += argument->type->size;
                printf("\tparameter_push %s\n", generate_expression(generator, argument));
            }

            // Call instruction
            char* label = generate_expression(generator, expression->call.variable);
            char* t = temp(generator);
            // NOTE(timo): According to the Dragon Book the number of arguments is
            // important info to pass on
            printf("\t%s := call %s, %d\n", t, label, arguments->length);

            // Pop the params from the stack by popping the number of bytes being popped
            // => reclaim the space used by the parameters
            printf("\tparameter_pop %d\n", release);

            // return label;
            return t;
        }
        default:
            // TODO(timo): Error
            break;
    }
}


// TODO(timo): This should return a pointer to Instruction generated with the function
void generate_statement(IR_Generator* generator, AST_Statement* statement)
{
    switch (statement->kind)
    {
        case STATEMENT_EXPRESSION:
            generate_expression(generator, statement->expression);
            break;
        case STATEMENT_BLOCK:
            for (int i = 0; i < statement->block.statements->length; i++)
                generate_statement(generator, (AST_Statement*)statement->block.statements->items[i]);
            break;
        case STATEMENT_WHILE:
        {
            // "label" + L1 + condition_code + "label" + L2 + statement_code_body

            // Local labels
            char* label1 = label(generator); // before
            char* label2 = label(generator); // after

            printf("%s:\n", label1);

            char* condition = generate_expression(generator, statement->_while.condition);

            // if false, jump to label 2
            printf("\tif_false %s goto %s\n", condition, label2);
            
            // print the body
            generate_statement(generator, statement->_while.body);
            
            // go back to the start of the loop to test the condition again
            printf("\tgoto %s\n", label1);

            // char* body = generate_statement(generator, statement->_while.body);
            printf("%s:\n", label2);
            
            // printf("label%s");
            break;
        }
        case STATEMENT_IF:
        {
            // condition_code + then + label + else  + label
            
            // if-then-else
            if (statement->_if._else != NULL)
            {
                // Local labels
                char* label1 = label(generator);
                char* label2 = label(generator);

                char* condition = generate_expression(generator, statement->_if.condition);

                printf("\tif_false %s goto %s\n", condition, label1);

                generate_statement(generator, statement->_if.then);

                printf("\tgoto %s\n", label2);

                printf("%s:\n", label1);
                generate_statement(generator, statement->_if._else);

                printf("%s:\n", label2);
            }
            // if-then-else
            else
            {
                // Local labels
                char* label1 = label(generator);

                char* condition = generate_expression(generator, statement->_if.condition);

                printf("\tif_false %s goto %s\n", condition, label1);

                generate_statement(generator, statement->_if.then);

                printf("%s:\n", label1);
            }

            break;
        }
        case STATEMENT_RETURN:
        {
            char* value = generate_expression(generator, statement->_return.value);
            printf("\treturn %s\n", value);
            break;
        }
        default:
            // TODO(timo): Error
            break;
    }
}


void generate_declaration(IR_Generator* generator, AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
        {
            // TODO(timo): How should I handle the global variable declarations?
            // Or do they just live in the symbol table and we can use them freely
            // from the functions when necessary and therefore access them when necessary.
            // TODO(timo): Or are these actually loaded into the stack at the 
            // beginning of the program
            break;
        }
        case DECLARATION_FUNCTION:
        {
            // TODO(timo): This is probably the point where we need the abstraction for routines
            printf("_%s:\n", declaration->identifier->lexeme);
            // TODO(timo): Get the size of the function somehow. It is probably
            // a lot easier when we abstract things into structs and use some other
            // functionality to print things. But for now, we keep things simple like this
            // int N = 324;
            
            // This function beginnning and ending is handled in function expression for the time being
            // printf("\tfunction_begin %d\n", N);
            
            generate_expression(generator, declaration->initializer);

            // printf("\tfunction_end\n");
            break;
        }
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
