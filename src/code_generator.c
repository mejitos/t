#include "t.h"


static int registers[] = { 1, 1, 1, 1, 1, 1 };
static const char* register_list[] = { "r10", "r11", "r12", "r13", "r14", "r15" };

static void free_registers()
{
    for (int i = 0; i < 6; i++)
        registers[i] = 1;
}


static void free_register(int _register)
{
    if (registers[_register] != 0)
    {
        printf("[CODE_GENERATOR] - Error: Trying to free already free'd register\n");
        exit(1);
    }

    registers[_register] = 1;
}


static int allocate_register()
{
    for (int i = 0; i < 6; i++)
    {
        if (registers[i])
        {
            registers[i] = 0;
            return i;
        }
    }

    printf("[CODE_GENERATOR] - Error: Out of free registers\n");
    exit(1);
}


void code_generator_init(Code_Generator* generator, Scope* global, array* instructions)
{
    *generator = (Code_Generator) { .global = global,
                                    .index = 0,
                                    .instructions = instructions };

    generator->local = generator->global;
}


void code_generate_instruction(Code_Generator* generator, Instruction* instruction)
{
    switch (instruction->operation)
    {
        case OP_ADD:
        {
            // Adds two registers together
            // dest <- (dest + src)
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            if (destination != NULL)
            {
                destination->_register = allocate_register();
                fprintf(generator->output,
                        "\tmov\t%s, [rbp-%d]\t\t; --\n", register_list[destination->_register], destination->offset);
            }
            if (source != NULL)
            {
                source->_register = allocate_register();
                fprintf(generator->output,
                        "\tmov\t%s, [rbp-%d]\t\t; --\n", register_list[source->_register], source->offset);
            }

            if (result != NULL)
            {
                fprintf(generator->output,
                    "\tadd\t%s, %s\n", register_list[destination->_register], register_list[source->_register]);
                fprintf(generator->output,
                    "\tmov qword\t[rbp-%d], %s\t\t; --\n", result->offset, register_list[destination->_register]);
            }
            else
            {
                result->_register = destination->_register;

                fprintf(generator->output,
                    "\tadd\t%s, %s\n", register_list[destination->_register], register_list[source->_register]);
            }

            free_register(source->_register);
            source->_register = -1;

            // NOTE(timo): This uses straight up registers for the values
            /*
            fprintf(generator->output,
                "\tadd\t%s, %s\n", register_list[destination->_register], register_list[source->_register]);

            free_register(source->_register);

            result->_register = destination->_register;
            */
            break;
        }
        case OP_SUB:
        {
            // Subtracts source from destination
            // dest <- (dest - src)
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            if (destination != NULL)
            {
                destination->_register = allocate_register();
                fprintf(generator->output,
                        "\tmov\t%s, [rbp-%d]\t\t; --\n", register_list[destination->_register], destination->offset);
            }
            if (source != NULL)
            {
                source->_register = allocate_register();
                fprintf(generator->output,
                        "\tmov\t%s, [rbp-%d]\t\t; --\n", register_list[source->_register], source->offset);
            }

            if (result != NULL)
            {
                fprintf(generator->output,
                    "\tsub\t%s, %s\n", register_list[destination->_register], register_list[source->_register]);
                fprintf(generator->output,
                    "\tmov qword\t[rbp-%d], %s\t\t; --\n", result->offset, register_list[destination->_register]);
            }
            else
            {
                result->_register = destination->_register;

                fprintf(generator->output,
                    "\tsub\t%s, %s\n", register_list[destination->_register], register_list[source->_register]);
            }

            free_register(source->_register);
            source->_register = -1;

            // NOTE(timo): This uses straight up registers for the values
            /*
            fprintf(generator->output,
                "\tsub\t%s, %s\n", register_list[destination->_register], register_list[source->_register]);

            free_register(source->_register);

            result->_register = destination->_register;
            */
            break;
        }
        case OP_MUL:
        {
            // Multiplies value in the rax register with the given argument. Result is saved into rax.
            // rax <- (rax * src)
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            if (destination != NULL)
            {
                fprintf(generator->output,
                        "\tmov\trax, [rbp-%d]\t\t; --\n", destination->offset);
            }
            if (source != NULL)
            {
                fprintf(generator->output,
                        "\tmul qword\t[rbp-%d]\t\t; --\n", source->offset);
            }

            if (result != NULL)
            {
                fprintf(generator->output,
                    "\tmov qword\t[rbp-%d], rax\t\t; --\n", result->offset);
            }
            else
            {
                result->_register = destination->_register;

                fprintf(generator->output,
                    "\tmov\trax, %s\n"
                    "\tmul\t%s\t\t; multiply the value in the rax with the source\n"
                    "\tmov\t%s, rax\n", register_list[destination->_register], register_list[source->_register], register_list[result->_register]);

                free_register(source->_register);
                source->_register = -1;
            }

            /*
            result->_register = destination->_register;

            fprintf(generator->output,
                "\tmov\trax, %s\n"
                "\tmul\t%s\t\t; multiply the value in the rax with the source\n"
                "\tmov\t%s, rax\n", register_list[destination->_register], register_list[source->_register], register_list[result->_register]);

            free_register(source->_register);
            */
            break;
        }
        case OP_DIV:
        {
            // rax <- (rax / src)
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            fprintf(generator->output,
                    "\tmov\trdx, 0\t\t; setting rdx to 0 because it represents the top bits of the divident\n");

            if (destination != NULL)
            {
                fprintf(generator->output,
                        "\tmov\trax, [rbp-%d]\t\t; --\n", destination->offset);
            }
            if (source != NULL)
            {
                fprintf(generator->output,
                        "\tdiv qword\t[rbp-%d]\t\t; --\n", source->offset);
            }

            if (result != NULL)
            {
                fprintf(generator->output,
                    "\tmov qword\t[rbp-%d], rax\t\t; --\n", result->offset);
            }
            else
            {
                result->_register = destination->_register;

                fprintf(generator->output,
                    "\tmov\trdx, 0\t\t; setting rdx to 0 because it represents the top bits of the input divident\n"
                    "\tmov\trax, %s\n"
                    "\tdiv\t%s\t\t; divide the rax with the source\n"
                    "\tmov\t%s, rax\n", register_list[destination->_register], register_list[source->_register], register_list[result->_register]);

                free_register(source->_register);
                source->_register = -1;
            }

            /*
            result->_register = destination->_register;

            fprintf(generator->output,
                "\tmov\trdx, 0\t\t; setting rdx to 0 because it represents the top bits of the input divident\n"
                "\tmov\trax, %s\n"
                "\tdiv\t%s\t\t; divide the rax with the source\n"
                "\tmov\t%s, rax\n", register_list[destination->_register], register_list[source->_register], register_list[result->_register]);

            free_register(source->_register);
            */
            break;
        }
        case OP_EQ:
        case OP_NEQ:
        case OP_LT:
        case OP_LTE:
        case OP_GT:
        case OP_GTE:
        {
            // Just put things into the registers and compare?
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            if (destination != NULL)
            {
                destination->_register = allocate_register();
                fprintf(generator->output,
                        "\tmov\t%s, [rbp-%d]\t\t; --\n", register_list[destination->_register], destination->offset);
            }
            if (source != NULL)
            {
                source->_register = allocate_register();
                fprintf(generator->output,
                        "\tmov\t%s, [rbp-%d]\t\t; --\n", register_list[source->_register], source->offset);
            }

            fprintf(generator->output,
                "\tcmp\t%s, %s\t\t; --\n", register_list[destination->_register], register_list[source->_register]);
            
            /*
            fprintf(generator->output,
                "\tcmp\tSF, OF\t\t; --\n");
            */

            /*
            fprintf(generator->output,
                "\tmov\t[rbp-%d], CF");
            */

            // TODO(timo): What to save into the result? I mean we could save the correct jump
            // instruction at this point already and just print it at the GOTO_IF_FALSE

            free_register(destination->_register);
            free_register(source->_register);
            
            break;
        }
        case OP_GOTO_IF_FALSE:
        {
            // TODO(timo): This is pretty hacky solution just to get things to work
            // just for this we added the index/cursor for the generator instructions
            // and do all this switch jumping to get correct jumps. Like Raymond Hettinger
            // would say: 'There must be a better way!'.

            // Here we can access the result of the condition
            // Symbol* condition = scope_lookup(generator->local, instruction->arg1);
            
            Instruction* preceding = generator->instructions->items[generator->index - 1];

            // NOTE(timo): Since the operation is goto if false, we need to use the
            // opposite conditions for the jumps
            switch (preceding->operation)
            {
                case OP_EQ:
                    fprintf(generator->output,
                        "\tjne %s\t\t; --\n", instruction->value.string);
                    break;
                case OP_NEQ:
                    fprintf(generator->output,
                        "\tje %s\t\t; --\n", instruction->value.string);
                    break;
                case OP_LT:
                    fprintf(generator->output,
                        "\tjge %s\t\t; --\n", instruction->value.string);
                    break;
                case OP_LTE:
                    fprintf(generator->output,
                        "\tjg %s\t\t; --\n", instruction->value.string);
                    break;
                case OP_GT:
                    fprintf(generator->output,
                        "\tjle %s\t\t; --\n", instruction->value.string);
                    break;
                case OP_GTE:
                    fprintf(generator->output,
                        "\tjl %s\t\t; --\n", instruction->value.string);
                    break;
                default:
                    printf("[CODE_GENERATOR] - Invalid preceding instruction in OP_GOTO_IF_FALSE\n");
                    exit(1);
            }
            
            /*
            // jz label
            fprintf(generator->output,
                "\tjz %s\t\t; --\n", instruction->value.string);
            */
            break;
        }
        case OP_COPY:
        {
            Symbol* result = scope_lookup(generator->local, instruction->result);
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);

            // NOTE(timo): If argument symbol is found, argument is a variable, else it is probably constant
            if (arg != NULL)
            {
                fprintf(generator->output,
                        "\tmov\trax, [rbp-%d]\t\t; --\n", arg->offset);
                fprintf(generator->output,
                        "\tmov\tqword [rbp-%d], rax\t\t; --\n", result->offset);
            }
            else
            {
                // NOTE(timo): Is this actually needed? I mean at this point we save pretty much everything to
                // stack at this point, so there is probably no need for register allocation here
                // result->_register = allocate_register();

                fprintf(generator->output,
                        "\tmov\tqword [rbp-%d], %s\t\t; asd\n", result->offset, instruction->arg1);
            }

            break;
        }
        case OP_LABEL:
        {
            // If the label is function symbol -> change the scope
            Symbol* symbol = scope_lookup(generator->local, instruction->value.string);

            if (symbol != NULL && symbol->type->kind == TYPE_FUNCTION)
            {
                generator->local = symbol->type->function.scope;
            }

            fprintf(generator->output, "%s:\n", instruction->value.string);
            break;
        }
        case OP_GOTO:
            fprintf(generator->output,
                "\tjmp %s\t\t; --\n", instruction->value.string);
            break;
        case OP_FUNCTION_BEGIN:
            // TODO(timo): That scope change should probably happen in here and not in OP_LABEL

            // TODO(timo): start a stack frame and allocate memory for it from the stack
            // the amount needed should be in the symbol table after getting
            // all the temporary variables etc.
            fprintf(generator->output, 
                    "\tpush\trbp\n"
                    "\tmov\trbp, rsp\t\t; started stack frame\n");

            fprintf(generator->output,
                    "\tsub\trsp, %d\t\t; allocate memory for local variables from stack\n", instruction->value.integer);

            // Push all the parameters
            // TODO(timo): So I should actually resolve the params normally
            // for the main, but they are "special params" mapped in assembly
            break;
        case OP_PARAM_PUSH:

            break;
        case OP_FUNCTION_END:
            // What to do with the function end?
            // Set the current scope to the enclosing (global) scope
            if (generator->local->enclosing != NULL)
            {
                generator->local = generator->global;
                // generator->local = generator->local->enclosing;
            }
            break;
        case OP_RETURN:
        {
            Symbol* value = scope_lookup(generator->local, instruction->arg1);
            
            if (value != NULL)
            {
                fprintf(generator->output,
                        "\tmov\trax, [rbp-%d]\t\t; put return value to rax\n", value->offset);
            }
            else
            {
                fprintf(generator->output,
                        "\tmov\trax, %s\t\t; put return value to rax\n", register_list[value->_register]);
            }

            free_registers();
            break;
        }
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
    {
        code_generate_instruction(generator, generator->instructions->items[i]);
        generator->index++;
    }

    // Should the main function has its own being and end operations?
    // We should probably have some way to declare builtin functions etc.

    // Template end
    fprintf(generator->output,
            "\n"
            "; preparing to exit from the program\n"
            "\tpush rax\t\t; caller save register for printf\n"
            "\tpush rcx\t\t; caller save register for printf\n"
            "\tmov\trdi, return_message\t\t; first argument of printf - format\n"
            "\tmov\trsi, rax\t\t ;second argument of printf - formatted value\n"
            "\txor\trax, rax\t\t; because the varargs printf uses\n"
            "\tcall printf\t\t; print the return value - printf(format, value)\n"
            "\tpop\trcx\t\t; restore caller save register of printf\n"
            "\tpop\trax\t\t; restore caller save register of printf\n"
            "\tmov\trax, 0\t\t; exit success\n"
            // "\tpop rbp ; leave the stack frame\n" // this could be used if we restore the stackpointer by adding the offset back
            "\tleave\t\t; leave the current stack frame without manually setting it\n"
            "\tret\t\t; exit the program\n"
            "\n"
            "return_message:\n"
            "\tdb 'Program exited with the value %s', 10, 0\n", "%d");

    fclose(generator->output);
}
