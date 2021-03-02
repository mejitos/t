#include "t.h"


void instruction_free(Instruction* instruction)
{
    // TODO(timo): I should somehow free the allocated memory for the temps and labels
    free(instruction);
    instruction = NULL;
}


void generator_init(IR_Generator* generator)
{
    *generator = (IR_Generator) { .temp = 0,
                                  .label = 0,
                                  .instructions = array_init(sizeof (Instruction*)) };
}


void generator_free(IR_Generator* generator)
{
    for (int i = 0; i < generator->instructions->length; i++)
        instruction_free(generator->instructions->items[i]);

    array_free(generator->instructions);

    // NOTE(timo): Generator itself is not being freed since it is
    // initialized in the stack in top level function
}


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


void dump_instruction(Instruction* instruction)
{
    switch (instruction->operation)
    {
        case OP_ADD:
            printf("\t%s := %s + %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_SUB:
            printf("\t%s := %s - %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_MUL:
            printf("\t%s := %s * %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_DIV:
            printf("\t%s := %s / %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_EQ:
            printf("\t%s := %s == %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_NEQ:
            printf("\t%s := %s != %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_LT:
            printf("\t%s := %s < %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_LTE:
            printf("\t%s := %s <= %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_GT:
            printf("\t%s := %s > %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_GTE:
            printf("\t%s := %s >= %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_MINUS:
            printf("\t%s := -%s\n", instruction->result, instruction->arg1);
            break;
        case OP_NEG:
            printf("\t%s := not %s\n", instruction->result, instruction->arg1);
            break;
        case OP_COPY:
            printf("\t%s := %s\n", instruction->result, instruction->arg1);
            break;
        case OP_FUNCTION_BEGIN:
            printf("\tfunction_begin %d\n", instruction->value.integer);
            break;
        case OP_FUNCTION_END:
            printf("\tfunction_end\n");
            break;
        case OP_RETURN:
            printf("\treturn %s\n", instruction->arg1);
            break;
        case OP_PARAM_PUSH:
            printf("\tparameter_push %s\n", instruction->arg1);
            break;
        case OP_PARAM_POP:
            printf("\tparameter_pop %d\n", instruction->value.integer);
            break;
        case OP_CALL:
            printf("\t%s := call %s, %d\n", instruction->result, instruction->arg1, instruction->value.integer);
            break;
        case OP_LABEL:
            printf("%s:\n", instruction->value.string);
            break;
        case OP_GOTO:
            printf("\tgoto %s\n", instruction->value.string);
            break;
        case OP_GOTO_IF_FALSE:
            printf("\tif_false %s goto %s\n", instruction->arg1, instruction->value.string);
            break;
        default:
            // TODO(timo): Error
            break;
    }
}


void dump_instructions(array* instructions)
{
    printf("\n");
    printf("-----===== INSTRUCTION DUMP =====-----\n");

    for (int i = 0; i < instructions->length; i++)
    {
        printf("%d  ", i);
        dump_instruction(instructions->items[i]);
    }

    printf("-----=====||||||||||||||||||=====-----\n");
}


Instruction* instruction_copy(char* arg, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_COPY;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_add(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_ADD;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_sub(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_SUB;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_mul(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_MUL;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_div(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_DIV;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_eq(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_EQ;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_neq(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_NEQ;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_lt(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_LT;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_lte(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_LTE;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_gt(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GT;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_gte(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GTE;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->result = result;

    return instruction;
}

Instruction* instruction_minus(char* arg, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_MINUS;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_neg(char* arg, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_NEG;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = result;

    return instruction;
}


Instruction* instruction_function_begin()
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_FUNCTION_BEGIN;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->value.integer = 0;

    return instruction;
}


Instruction* instruction_function_end()
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_FUNCTION_END;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;

    return instruction;
}


Instruction* instruction_param_push(char* arg)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_PARAM_PUSH;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = NULL;

    return instruction;
}


Instruction* instruction_param_pop(int release)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_PARAM_POP;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->value.integer = release;

    return instruction;
}


Instruction* instruction_call(char* arg, char* result, int n)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_CALL;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = result;
    instruction->value.integer = n;

    return instruction;
}


Instruction* instruction_return(char* arg)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_RETURN;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = NULL;

    return instruction;
}


Instruction* instruction_label(char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_LABEL;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->value.string = label; // TODO(timo): Now that I think of it, this could be just a arg1

    return instruction;
}


Instruction* instruction_goto(char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GOTO;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->value.string = label; // TODO(timo): Now that I think of it, this could be just a arg1

    return instruction;
}


Instruction* instruction_goto_if_false(char* arg, char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GOTO_IF_FALSE;
    instruction->arg1 = arg;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->value.string = label; // TODO(timo): Now that I think of it, this could be just a arg1

    return instruction;
}


// Instruction* generate_expression(IR_Generator* generator, AST_Expression* expression)
char* generate_expression(IR_Generator* generator, AST_Expression* expression)
{
    // TODO(timo): This should probably return the instruction structures
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
        {
            // NOTE(timo): The situation probably calls for generating the temp variables
            // for all the literals/constants etc. and they just get optimized away later
            // TODO(timo): I think we could just return the string and we can skip the
            // unnecessary extra assign/store/move step
            /*
            char* result = temp(generator);
            char* arg = (char*)expression->literal.literal->lexeme;
            printf("\t%s := %s\n", result, arg);

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);
            
            return result;
            */
            // Generate nothing, just return the literal
            return (char*)expression->literal.literal->lexeme;
        }
        case EXPRESSION_UNARY:
        {
            char* operand = generate_expression(generator, expression->unary.operand);
            char* operator = (char*)expression->unary._operator->lexeme;
            char* result = temp(generator);

            printf("\t%s := %s %s\n", result, operator, operand);

            Instruction* instruction;

            switch (expression->unary._operator->kind)
            {
                case TOKEN_MINUS:
                    instruction = instruction_minus(operand, result);
                    break;
                case TOKEN_NOT:
                    instruction = instruction_neg(operand, result);
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }

            array_push(generator->instructions, instruction);

            return result;
        
        }
        case EXPRESSION_BINARY:
        {
            char* left = generate_expression(generator, expression->binary.left); 
            char* right = generate_expression(generator, expression->binary.right);
            char* operator = (char*)expression->binary._operator->lexeme;
            // NOTE(timo): When we move this into here, we get the correct ordering of the temp labels
            char* result = temp(generator);

            printf("\t%s := %s %s %s\n", result, left, operator, right);
            
            Instruction* instruction;

            switch (expression->binary._operator->kind)
            {
                case TOKEN_PLUS:
                    instruction = instruction_add(left, right, result);
                    break;
                case TOKEN_MINUS:
                    instruction = instruction_sub(left, right, result);
                    break;
                case TOKEN_MULTIPLY:
                    instruction = instruction_mul(left, right, result);
                    break;
                case TOKEN_DIVIDE:
                    instruction = instruction_div(left, right, result);
                    break;
                case TOKEN_IS_EQUAL:
                    instruction = instruction_eq(left, right, result);
                    break;
                case TOKEN_NOT_EQUAL:
                    instruction = instruction_neq(left, right, result);
                    break;
                case TOKEN_LESS_THAN:
                    instruction = instruction_lt(left, right, result);
                    break;
                case TOKEN_LESS_THAN_EQUAL:
                    instruction = instruction_lte(left, right, result);
                    break;
                case TOKEN_GREATER_THAN:
                    instruction = instruction_gt(left, right, result);
                    break;
                case TOKEN_GREATER_THAN_EQUAL:
                    instruction = instruction_gte(left, right, result);
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }
            
            array_push(generator->instructions, instruction);

            return result;
        }
        case EXPRESSION_VARIABLE:
        {
            // TODO(timo): I think we could just return the string and we can skip the
            // unnecessary extra assign/store/move step
            /*
            char* result = temp(generator);
            char* arg = (char*)expression->literal.literal->lexeme;
            printf("\t%s := %s\n", result, arg);

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);

            return result;
            */
            // Generate nothing, just return the variable
            return (char*)expression->literal.literal->lexeme;
        }
        case EXPRESSION_ASSIGNMENT:
        {
            // This is pretty much same as the copy instruction since we just access
            // the variable and save it to the target variable or send it to the target
            //
            // Assignment happens pretty much only to the named variable so there is no
            // need for temp variables
            char* result = (char*)expression->assignment.variable->identifier->lexeme;
            char* arg = generate_expression(generator, expression->assignment.value);
            printf("\t%s := %s\n", result, arg);

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);

            return result;
        }
        case EXPRESSION_INDEX:
        {
            break;
        }
        case EXPRESSION_FUNCTION:
        {
            Instruction* instruction;
            // TODO(timo): Does the function declaration handle the beginnning and ending?
            // It probably is job of the expression, if we think about a little into the
            // future and to anonymous functions
            printf("\tfunction_begin N\n");

            instruction = instruction_function_begin();
            array_push(generator->instructions, instruction);

            generate_statement(generator, expression->function.body);

            printf("\tfunction_end\n");

            instruction = instruction_function_end();
            array_push(generator->instructions, instruction);
            break;
        }
        case EXPRESSION_CALL:
        {
            Instruction* instruction;

            // Push the params to the stack/registers
            array* arguments = expression->call.arguments;
            int release = 0;

            for (int i = arguments->length - 1; i >= 0; i--)
            {
                AST_Expression* argument = (AST_Expression*)arguments->items[i];
                release += argument->type->size;
                char* arg = generate_expression(generator, argument);
                printf("\tparameter_push %s\n", arg);

                instruction = instruction_param_push(arg);
                array_push(generator->instructions, instruction);
            }

            // Call instruction
            char* arg = generate_expression(generator, expression->call.variable);
            char* result = temp(generator);
            // NOTE(timo): According to the Dragon Book the number of arguments is
            // important info to pass on in case of nested function calls
            printf("\t%s := call %s, %d\n", result, arg, arguments->length);

            instruction = instruction_call(arg, result, arguments->length);
            array_push(generator->instructions, instruction);

            // Pop the params from the stack by popping the number of bytes being popped
            // => reclaim the space used by the parameters
            printf("\tparameter_pop %d\n", release);

            instruction = instruction_param_pop(release);
            array_push(generator->instructions, instruction);

            return result;
        }
        default:
            // TODO(timo): Error
            break;
    }
}


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
            Instruction* instruction;
            // "label" + L1 + condition_code + "label" + L2 + statement_code_body

            // Local labels
            char* label_condition = label(generator); // condition
            char* label_exit = label(generator); // exit

            printf("%s:\n", label_condition);

            instruction = instruction_label(label_condition);
            array_push(generator->instructions, instruction);

            char* condition = generate_expression(generator, statement->_while.condition);

            // if false, jump to label 2
            printf("\tif_false %s goto %s\n", condition, label_exit);

            instruction = instruction_goto_if_false(condition, label_exit);
            array_push(generator->instructions, instruction);
            
            // print the body
            generate_statement(generator, statement->_while.body);
            
            // go back to the start of the loop to test the condition again
            printf("\tgoto %s\n", label_condition);

            instruction = instruction_goto(label_condition);
            array_push(generator->instructions, instruction);
            
            // exit
            printf("%s:\n", label_exit);

            instruction = instruction_label(label_exit);
            array_push(generator->instructions, instruction);
            
            break;
        }
        case STATEMENT_IF:
        {
            // condition_code + then + label + else  + label
            
            // if-then-else
            if (statement->_if._else != NULL)
            {
                Instruction* instruction;
                // Local labels
                char* label_else = label(generator);
                char* label_exit = label(generator);
                char* condition = generate_expression(generator, statement->_if.condition);
                
                //
                printf("\tif_false %s goto %s\n", condition, label_else);

                instruction = instruction_goto_if_false(condition, label_else);
                array_push(generator->instructions, instruction);

                generate_statement(generator, statement->_if.then);
                
                //
                printf("\tgoto %s\n", label_exit);

                instruction = instruction_goto(label_exit);
                array_push(generator->instructions, instruction);
                
                //
                printf("%s:\n", label_else);
                instruction = instruction_label(label_else);
                array_push(generator->instructions, instruction);

                generate_statement(generator, statement->_if._else);
                
                //
                printf("%s:\n", label_exit);

                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);
            }
            // if-then
            else
            {
                Instruction* instruction;
                // Local labels
                char* label_exit = label(generator);
                char* condition = generate_expression(generator, statement->_if.condition);
                
                //
                printf("\tif_false %s goto %s\n", condition, label_exit);

                instruction = instruction_goto_if_false(condition, label_exit);
                array_push(generator->instructions, instruction);

                generate_statement(generator, statement->_if.then);
                
                // 
                printf("%s:\n", label_exit);

                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);
            }
            break;
        }
        case STATEMENT_RETURN:
        {
            char* value = generate_expression(generator, statement->_return.value);
            printf("\treturn %s\n", value);

            Instruction* instruction = instruction_return(value);
            array_push(generator->instructions, instruction);
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
            Instruction* instruction;
            // TODO(timo): This is probably the point where we need the abstraction for routines
            char* label = declaration->identifier->lexeme;
            printf("_%s:\n", label);

            instruction = instruction_label(label);
            array_push(generator->instructions, instruction);

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
