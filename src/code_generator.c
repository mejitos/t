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

            Instruction* preceding = generator->instructions->items[generator->index - 1];

            // NOTE(timo): Since the operation is goto if false, we need to use the
            // opposite conditions for the jumps
            switch (preceding->operation)
            {
                case OP_EQ:
                    fprintf(generator->output,
                        "\tjne %s\n", instruction->label);
                    break;
                case OP_NEQ:
                    fprintf(generator->output,
                        "\tje %s\n", instruction->label);
                    break;
                case OP_LT:
                    fprintf(generator->output,
                        "\tjge %s\n", instruction->label);
                    break;
                case OP_LTE:
                    fprintf(generator->output,
                        "\tjg %s\n", instruction->label);
                    break;
                case OP_GT:
                    fprintf(generator->output,
                        "\tjle %s\n", instruction->label);
                    break;
                case OP_GTE:
                    fprintf(generator->output,
                        "\tjl %s\n", instruction->label);
                    break;
                default:
                    // TODO(timo): Create diagnostic
                    printf("[CODE_GENERATOR] - Invalid preceding instruction in OP_GOTO_IF_FALSE\n");
                    exit(1);
            }

            break;
        }
        case OP_COPY:
        {
            Symbol* result = scope_lookup(generator->local, instruction->result);
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);

            // NOTE(timo): If argument symbol is found, argument is a variable, else it is probably constant
            if (arg != NULL)
            {
                // TODO(timo): For now this is just a super quick hacky solution to get things working
                if (arg->kind == SYMBOL_PARAMETER)
                {
                    // NOTE(timo): The offset has to be offset + 8 here, since the first spot in the
                    // previous stackframe, is always the return address and the parameters are behind it
                    fprintf(generator->output,
                            "\tmov\trax, [rbp+%d]\t\t; --\n", arg->offset + 8);
                    fprintf(generator->output,
                            "\tmov\tqword [rbp-%d], rax\t\t; --\n", result->offset);
                }
                else
                {
                    fprintf(generator->output,
                            "\tmov\trax, [rbp-%d]\t\t; --\n", arg->offset);
                    fprintf(generator->output,
                            "\tmov\tqword [rbp-%d], rax\t\t; --\n", result->offset);
                }
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
        /* TODO(timo): Not used right now. Need to think if this is even necessary
        case OP_STORE_GLOBAL:
        {
            fprintf(generator->output,
                "\t.quad %s\n", instruction->arg1);
            break;
        }
        */
        case OP_LOAD_GLOBAL:
        {
            Symbol* result = scope_lookup(generator->local, instruction->result);
            Symbol* global = scope_lookup(generator->global, instruction->arg1);

            fprintf(generator->output,
                "\tmov\trax, [%s]\n", global->identifier);
            fprintf(generator->output,
                "\tmov\t[rbp-%d], rax\n", result->offset);
            break;
        }
        case OP_LABEL:
        {
            // If the label is function symbol -> change the scope
            Symbol* symbol = scope_lookup(generator->local, instruction->label);

            if (symbol != NULL && symbol->type->kind == TYPE_FUNCTION)
            {
                generator->local = symbol->type->function.scope;
            }

            fprintf(generator->output, "%s:\n", instruction->label);
            break;
        }
        case OP_GOTO:
        {
            fprintf(generator->output,
                "\tjmp %s\t\t; --\n", instruction->label);
            break;
        }
        case OP_FUNCTION_BEGIN:
        {
            // TODO(timo): That scope change should probably happen in here and not in OP_LABEL
            // but we do need the name of the function. Should we save it into the function begin instruction?
            
            // These instructions are same as instruction "enter, N" where enter sets the stackframe 
            // and if N is given, N bytes of memory will be reserved from the stack
            fprintf(generator->output, 
                "\tpush\trbp\n"
                "\tmov\trbp, rsp\t\t; started stack frame\n");

            fprintf(generator->output,
                "\tsub\trsp, %d\t\t; allocate memory for local variables from stack\n", instruction->size);

            // TODO(timo): Save the parameters to the local variables in here

            // Push all the parameters

            // TODO(timo): So I should actually resolve the params normally
            // for the main, but they are "special params" mapped in assembly
            break;
        }
        case OP_PARAM_PUSH:
        {
            // arg1: the parameter being pushed into the stack
            Symbol* parameter = scope_lookup(generator->local, instruction->arg1);
            
            // Local variable saved in the stack
            if (parameter != NULL)
            {
                fprintf(generator->output,
                        "\tpush\tqword[rbp-%d]\t\t; pushing function parameter\n", parameter->offset);
            }
            else
            {
                // TODO(timo): Just use the registers or error
            }

            break;
        }
        case OP_PARAM_POP:
        {
            // arg1: the parameter being popped from the stack
            Symbol* parameter = scope_lookup(generator->local, instruction->arg1);
            
            // Local variable saved in the stack
            if (parameter != NULL)
            {
                fprintf(generator->output,
                        "\tpop\tqword [rbp-%d]\t\t; popping function parameter\n", parameter->offset);
            }
            else
            {
                // TODO(timo): Just use the registers or error
            }

            break;
        }
        case OP_FUNCTION_END:
        {
            // The function epilogue

            // Label
            fprintf(generator->output,
                "%s_epilogue:\n", generator->local->name);

            // Restore the stack
            // Restore the arguments/registers
            // Return to the caller
            // TODO(timo): Hacky solution to get things work for now
            if (strcmp(generator->local->name, "main") != 0)
            {
                fprintf(generator->output,
                    // "\tadd\trsp, 8\n"
                    // "\tleave\n"
                    "\tmov\trsp, rbp\t\t; restore stack frame\n"
                    "\tpop\trbp\n"
                    "\tret\n");
            }

            // Set the current scope to the enclosing (global) scope
            if (generator->local->enclosing != NULL)
            {
                // NOTE(timo): At this point, these two are equal
                generator->local = generator->global;
                // generator->local = generator->local->enclosing;
            }

            break;
        }
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

            // TODO(timo): Make jump to function epilogue where the function will be teared down
            // Even though the jump is not necessary right now since we only have one return per function
            fprintf(generator->output,
                "\tjmp %s_epilogue\t\t; run the function epilogue\n", generator->local->name);

            free_registers();
            break;
        }
        case OP_CALL:
        {
            // NOTE(timo): The call instruction itself contains the RIP push to the stack and jmp to the function

            // Instruction has arg1: what is being called
            //                 result: where the result is being saved
            //                 n: number of arguments the function was being called with
            Symbol* value = scope_lookup(generator->local, instruction->arg1);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            if (value != NULL)
            {
                fprintf(generator->output,
                        // "\txor\trax, rax\n"
                        "\tcall\t%s\t\t; calling the function %s\n", value->identifier, value->identifier);
                // TODO(timo): We have to reset the stack right away after the call and not to wait param pop operations

                /*
                fprintf(generator->output,
                        "\tadd\trsp, 16\n");
                */
            }
            else
            {
                // TODO(timo): Just use the registers or error
            }

            // Save the call result to result address
            if (result != NULL)
            {
                fprintf(generator->output,
                        "\tmov\t[rbp-%d], rax\t\t; Moving the function result to the result address\n", result->offset);
            }
            else
            {
                // TODO(timo): Just use the registers or error
            }

            break;
        }
        default:
        {
            // TODO(timo): Error
            break;
        }
    }
}


void code_generate(Code_Generator* generator)
{
    if ((generator->output = fopen(generator->asm_file, "w")) == NULL)
    {
        printf("Unable to create assembly file\n");
        return;
    }
    
    // Template start and externs
    fprintf(generator->output,
        "; ----------------------------------------\n"
        "; main.asm\n"
        "; ----------------------------------------\n"
        "\n"
        "\tglobal main\n"
        "\textern printf\n"
        "\textern atoi\n"
        "\textern itoa\n"
        "\n");

    // Global variables
    fprintf(generator->output,
        "\tsection .data\n");

    for (int i = 0; i < generator->global->symbols->capacity; i++)
    {
        Symbol* symbol = generator->global->symbols->entries[i].value;

        if (symbol && symbol->kind == SYMBOL_VARIABLE)
            fprintf(generator->output,
                "%s:\tdq %d\n", symbol->identifier, symbol->value.integer);
    }

    // Start the code itself
    fprintf(generator->output,
        "\n"
        "\tsection .text\n");
    
    for (int i = 0; i < generator->instructions->length; i++)
    {
        code_generate_instruction(generator, generator->instructions->items[i]);
        generator->index++;
    }
    
    // TODO(timo):
    // Should the main function has its own beginning and end operations?
    // We should probably have some way to declare builtin functions etc.

    // Template end
    fprintf(generator->output,
            // "\tmov\trbx, rax\t\t; save the return value into sys exit\n"
            "\tpush rax\t\t; caller save register for printf\n"
            "\tpush rcx\t\t; caller save register for printf\n"
            "\tmov\trdi, return_message\t\t; first argument of printf - format\n"
            "\tmov\trsi, rax\t\t ;second argument of printf - formatted value\n"
            "\txor\trax, rax\t\t; because the varargs printf uses\n"
            "\tcall printf\t\t; print the return value - printf(format, value)\n"
            "\tpop\trcx\t\t; restore caller save register of printf\n"
            "\tpop\trax\t\t; restore caller save register of printf\n"
            "\tmov\trax, 0\t\t; exit success\n"
            // "\tmov\trax, rbx\t\t; return value\n" // Return the value as a program exit code
            "\tleave\t\t; leave the current stack frame without manually setting it\n"
            "\tret\t\t; exit the program\n"
            "\n"
            "return_message:\n"
            "\tdb 'Program exited with the value %s', 10, 0", "%d");

    fclose(generator->output);
}
