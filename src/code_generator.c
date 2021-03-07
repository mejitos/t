#include "t.h"


void code_generator_init(Code_Generator* generator, array* instructions)
{
    *generator = (Code_Generator) { .instructions = instructions };
}


void code_generate_instruction(Code_Generator* generator, Instruction* instruction)
{
    switch (instruction->operation)
    {
        case OP_LABEL:
            fprintf(generator->output, "%s:\n", instruction->value.string);
            break;
        case OP_FUNCTION_BEGIN:
            // start a stack frame and allocate memory for it from the stack
            // the amount needed should be in the symbol table after getting
            // all the temporary variables etc.
            // NOTE(timo): For now we use this general function as main program
            // but we probably should create the own commands 
            fprintf(generator->output, 
                    "\tpush\trbp\n"
                    "\tmov\trbp, rsp ; started stack frame\n"
                    "\tmov\t[rbp-4], rdi ; argc\n"
                    "\tmov\t[rbp-16], rsi ; argv\n");
            // Push all the parameters
            // TODO(timo): So I should actually resolve the params normally
            // for the main, but they are "special params" mapped in assembly
        case OP_FUNCTION_END:
            // What to do with the function end?
            break;
        case OP_RETURN:
            fprintf(generator->output,
                    "\tmov\trax, %s ; put return value to rax\n", instruction->arg1);
        default:
            // TODO(timo): Error
            break;
    }
}


void code_generate(Code_Generator* generator)
{
    if ((generator->output = fopen("main.asm", "w")) == NULL)
    {
        printf("Unable to create assembly file\n");
        return;
    }
    
    
    // Template start
    fprintf(generator->output,
            "; ----------------------------------------\n"
            "; main.asm\n"
            "; ----------------------------------------\n"
            "\n"
            "\tglobal main\n"
            "\textern printf\n"
            "\textern atoi\n"
            "\textern itoa\n"
            "\n"
            "\tsection .text\n");
    
    for (int i = 0; i < generator->instructions->length; i++)
        code_generate_instruction(generator, generator->instructions->items[i]);

    // Should the main function has its own being and end operations?
    // We should probably have some way to declare builtin functions etc.

    // Template end
    fprintf(generator->output,
            "\n"
            "; preparing to exit from the program\n"
            "\tmov\trdi, return_message ; setting the return message\n"
            "\tmov\trsi, rax ; return value for printf\n"
            "\tcall printf ; print the return value\n"
            "\tmov\trax, 0 ; exit success\n"
            // "\tpop rbp ; leave the stack frame\n"
            "\tleave ; leave the current stack frame without manually setting it\n"
            "\tret ; exit the program\n"
            "\n"
            "return_message:\n"
            "\tdb 'Program exited with the value %s', 10, 0\n", "%d");

    fclose(generator->output);
}
