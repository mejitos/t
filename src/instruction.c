#include "t.h"


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
