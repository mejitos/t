#include "t.h"


void code_generator_init(Code_Generator* generator)
{
    *generator = (Code_Generator) {0};
}


void code_generate(Code_Generator* generator)
{
    if ((generator->output = fopen("main.asm", "w")) == NULL)
    {
        printf("Unable to create assembly file\n");
        return;
    }
    
    // Basic template?
    fprintf(generator->output,
            "; ----------------------------------------\n"
            "; main.asm\n"
            "; ----------------------------------------\n"
            "\n"
            "\tglobal main\n"
            "\textern printf\n"
            "\textern atoi\n"
            "\n"
            "\tsection .data\n"
            "message db 'Hello world', 10, 0\n"
            "\n"
            "\tsection .text\n"
            "main:\n"
            "\tpush\trbp ; push the bottom pointer to stack\n"
            "\tmov\trbp, rsp ; set the stack pointer to bottom pointer\n"
            "\tmov\trdi, message\n"
            "\tmov\trax, 0\n"
            "\tcall printf\n"
            "\tleave\n"
            "\tret\n");

    fclose(generator->output);
}
