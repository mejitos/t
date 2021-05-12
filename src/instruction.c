// Implementations for factorcy functions to create new instructions,
// freeing the memory allocated for them and to print the instructions.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


void instruction_free(Instruction* instruction)
{
    if (instruction->arg1)
    {
        free(instruction->arg1);
        instruction->arg1 = NULL;
    }
    if (instruction->arg2)
    {
        free(instruction->arg2);
        instruction->arg2 = NULL;
    }
    if (instruction->result)
    {
        free(instruction->result);
        instruction->result = NULL;
    }
    if (instruction->label)
    {
        free((char*)instruction->label);
        instruction->label = NULL;
    }

    free(instruction);
    instruction = NULL;
}


Instruction* instruction_copy(char* arg, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_COPY;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_add(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_ADD;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_sub(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_SUB;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_mul(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_MUL;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_div(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_DIV;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_eq(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_EQ;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_neq(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_NEQ;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_lt(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_LT;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_lte(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_LTE;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_gt(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GT;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_gte(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GTE;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_and(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_AND;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_or(char* arg1, char* arg2, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_OR;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = str_copy(arg2);
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_minus(char* arg, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_MINUS;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_not(char* arg, char* result)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_NOT;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = str_copy(result);

    return instruction;
}


Instruction* instruction_function_begin(char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_FUNCTION_BEGIN;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->label = str_copy(label);
    instruction->size = 0;

    return instruction;
}


Instruction* instruction_function_end(char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_FUNCTION_END;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->label = str_copy(label);

    return instruction;
}


Instruction* instruction_param_push(char* arg)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_PARAM_PUSH;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = NULL;

    return instruction;
}


Instruction* instruction_param_pop(char* arg)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_PARAM_POP;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = NULL;

    return instruction;
}


Instruction* instruction_call(char* arg, char* result, int n)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_CALL;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = str_copy(result);
    instruction->size = n;

    return instruction;
}


Instruction* instruction_return(char* arg)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_RETURN;
    instruction->arg1 = str_copy(arg);
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
    instruction->label = str_copy(label); // TODO(timo): Now that I think of it, this could be just a arg1

    return instruction;
}


Instruction* instruction_goto(char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GOTO;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->label = str_copy(label); // TODO(timo): Now that I think of it, this could be just a arg1

    return instruction;
}


Instruction* instruction_goto_if_false(char* arg, char* label)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_GOTO_IF_FALSE;
    instruction->arg1 = str_copy(arg);
    instruction->arg2 = NULL;
    instruction->result = NULL;
    instruction->label = str_copy(label); // TODO(timo): Now that I think of it, this could be just a arg1

    return instruction;
}


Instruction* instruction_dereference(char* arg1, char* result, int offset)
{
    Instruction* instruction = xcalloc(1, sizeof (Instruction));
    instruction->operation = OP_DEREFERENCE;
    instruction->arg1 = str_copy(arg1);
    instruction->arg2 = NULL;
    instruction->result = str_copy(result);

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
        case OP_NOT:
            printf("\t%s := not %s\n", instruction->result, instruction->arg1);
            break;
        case OP_AND:
            printf("\t%s := %s and %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_OR:
            printf("\t%s := %s or %s\n", instruction->result, instruction->arg1, instruction->arg2);
            break;
        case OP_COPY:
            printf("\t%s := %s\n", instruction->result, instruction->arg1);
            break;
        case OP_FUNCTION_BEGIN:
            printf("\tfunction_begin %d\n", instruction->size);
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
            printf("\tparameter_pop %s\n", instruction->arg1);
            break;
        case OP_CALL:
            printf("\t%s := call %s, %d\n", instruction->result, instruction->arg1, instruction->size);
            break;
        case OP_LABEL:
            printf("%s:\n", instruction->label);
            break;
        case OP_GOTO:
            printf("\tgoto %s\n", instruction->label);
            break;
        case OP_GOTO_IF_FALSE:
            printf("\tif_false %s goto %s\n", instruction->arg1, instruction->label);
            break;
        case OP_DEREFERENCE:
            printf("\t%s = *(%s)\n", instruction->result, instruction->arg1);
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


const char* operation_str(Operation operation)
{
    switch (operation)
    {
        case OP_COPY:               return "copy";
        case OP_ADD:                return "add";
        case OP_SUB:                return "sub";
        case OP_MUL:                return "mul";
        case OP_MINUS:              return "minus";
        case OP_DIV:                return "div";
        case OP_LT:                 return "lt";
        case OP_LTE:                return "lte";
        case OP_GT:                 return "gt";
        case OP_GTE:                return "gte";
        case OP_EQ:                 return "eq";
        case OP_NEQ:                return "neq";
        case OP_AND:                return "and";
        case OP_OR:                 return "or";
        case OP_NOT:                return "not";
        case OP_FUNCTION_BEGIN:     return "function begin";
        case OP_FUNCTION_END:       return "function end";
        case OP_PARAM_PUSH:         return "param push";
        case OP_PARAM_POP:          return "param pop";
        case OP_CALL:               return "call";
        case OP_RETURN:             return "return";
        case OP_LABEL:              return "label";
        case OP_GOTO:               return "goto";
        case OP_GOTO_IF_FALSE:      return "goto if false";
        case OP_DEREFERENCE:        return "dereference";
        default:                    return "invalid operation";
    }
}
