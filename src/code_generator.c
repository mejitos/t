// Code generator generates the target machine's code from the intermediate
// representation generated by the IR generator. Target architecture in this
// case the x86-64 architecture.
//
// The implementation is really straightforward but it is also pretty error
// prone and there is no recovery in error situations. I really tried to aim
// for something that is easy to read and understand. Therefore most of the
// responsibility of error checking is on the previous stages. We can use this
// really straightforward style because pretty much everything is saved on a
// temporary to the symbol table excluding constants. They are being handled
// differently.
//
// Even though there is functionality for using scratch registers, they are
// not utilized at all. That decision was made to make things simpler in the
// beginning so everything is just kept in the stack and registers are used
// only for moving things around in the stack. That results in ugly and really
// slow code, but it was necessary to do to get things started. That is also
// the first thing that should be done with the code generator: utilize the
// registers for better and faster code. That would also mean that the IR
// code should also be optimized at least in a way that all dead code is
// eliminated and some basic liveness analysis is made so the IR instructions
// are reduced to somewhat minimum number of instructions.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


// Mark register from the register_list either free or taken. 1 marks free 
// register and 0 marks taken.
static int registers[] = { 1, 1, 1, 1, 1, 1 };
// List of usable general purpose registers.
// TODO(timo): Separate Linux kernel argument registers and general purpose registers?
// TODO(timo): How about 8-, 16- and 32-bit registers?
static const char* register_list[] = { "r10", "r11", "r12", "r13", "r14", "r15" };


// Frees all the registers by setting every value in registers to 1.
static void free_registers()
{
    for (int i = 0; i < 6; i++)
        registers[i] = 1;
}


// Frees a single register by setting it's value to 1.
//
// Arguments
//      generator: Initialized code generator.
//      _register: Register to be freed.
static void free_register(Code_Generator* generator, int _register)
{
    if (registers[_register] != 0)
    {
        Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
            ":CODE_GENERATOR - Error: Trying to free already free'd register");
        array_push(generator->diagnostics, _diagnostic);
        return;
    }

    registers[_register] = 1;
}


// Allocates new register by marking it 0 and returning the index of the 
// newly taken register.
//
// Arguments
//      generator: Initialized code generator.
// Returns
//      Allocated register ready for use.
static int allocate_register(Code_Generator* generator)
{
    for (int i = 0; i < 6; i++)
    {
        if (registers[i])
        {
            registers[i] = 0;
            return i;
        }
    }

    Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
        ":CODE_GENERATOR - Error: Out of free registers");
    array_push(generator->diagnostics, _diagnostic);
}


void code_generator_init(Code_Generator* generator, Scope* global, array* instructions)
{
    *generator = (Code_Generator) { .global = global,
                                    .diagnostics = array_init(sizeof (Diagnostic*)),
                                    .instructions = instructions };

    generator->local = generator->global;
}


void code_generator_free(Code_Generator* generator)
{
    for (int i = 0; i < generator->diagnostics->length; i++)
    {
        Diagnostic* diagnostic = generator->diagnostics->items[i];

        free((char*)diagnostic->message);
        diagnostic->message = NULL;

        free(diagnostic);
        diagnostic = NULL;
    }

    // NOTE(timo): Array will be set to NULL in the function.
    array_free(generator->diagnostics);

    // NOTE(timo): Generator is not being freed because it is in the stack at 
    // the top level.
}


void code_generate_instruction(Code_Generator* generator, Instruction* instruction)
{
    switch (instruction->operation)
    {
        case OP_COPY:
        {
            // TODO(timo): Assignment to global variable

            // This result will always be set, since it is temporary
            Symbol* result = scope_lookup(generator->local, instruction->result);
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);

            // Arg can be NULL, if the operand of the copy is a constant
            if (arg != NULL)
            {
                // Global values has to be dereferenced before they can be used
                if (arg->scope == generator->global)
                {
                    fprintf(generator->output,
                        "    mov    rax, [%s]               ; move global variable to register\n"
                        "    mov    [rbp-%d], rax           ; move global variable from register to stack\n", 
                        arg->identifier, result->offset);
                }
                // If the argument is parameter, it will have positive offset
                // relative to the base pointer of the stack frame
                else if (arg->kind == SYMBOL_PARAMETER)
                {
                    fprintf(generator->output,
                        "    mov    rax, [rbp+%d]           ; move parameter from stack to rax\n"
                        "    mov    qword [rbp-%d], rax     ; move parameter from rax to stack\n", 
                        arg->offset, result->offset);
                }
                else
                {
                    fprintf(generator->output,
                        "    mov    rax, [rbp-%d]           ; --\n"
                        "    mov    qword [rbp-%d], rax     ; --\n", 
                        arg->offset, result->offset);
                }
            }
            else // This is used when constants are being copied
            {
                if (result->type->kind == TYPE_INTEGER)
                    fprintf(generator->output,
                        "    mov    qword [rbp-%d], %s      ; move integer constant to stack\n", 
                        result->offset, instruction->arg1);
                else if (result->type->kind == TYPE_BOOLEAN)
                    fprintf(generator->output,
                        "    mov    rax, [%s]               ; dereference the boolean value to rax\n"
                        "    mov    qword [rbp-%d], rax     ; move the integer value of the boolean to the stack\n",
                        instruction->arg1, result->offset);
            }

            break;
        }
        case OP_ADD: // https://www.felixcloutier.com/x86/add
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            int temp_reg = allocate_register(generator);
            
            fprintf(generator->output,
                "    mov    %s, [rbp-%d]            ; move first operand from the stack to the register\n"
                "    add    %s, [rbp-%d]            ; add the values together\n"
                "    mov    qword [rbp-%d], %s      ; move the result to the stack\n", 
                register_list[temp_reg], destination->offset,
                register_list[temp_reg], source->offset,
                result->offset, register_list[temp_reg]);

            free_register(generator, temp_reg);
            break;
        }
        case OP_SUB: // https://www.felixcloutier.com/x86/sub
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            int temp_reg = allocate_register(generator);

            fprintf(generator->output,
                "    mov    %s, [rbp-%d]            ; move the variable from the stack to the register\n"
                "    sub    %s, [rbp-%d]                  ; subtract two values from each other\n"
                "    mov    qword [rbp-%d], %s      ; move the result to the stack\n", 
                register_list[temp_reg], destination->offset,
                register_list[temp_reg], source->offset,
                result->offset, register_list[temp_reg]);

            free_register(generator, temp_reg);
            break;
        }
        case OP_MUL: // https://www.felixcloutier.com/x86/mul
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; move the variable being multiplied from the stack to the rax\n"
                "    mul    qword [rbp-%d]          ; multiply the value in rax with the operand\n"
                "    mov    qword [rbp-%d], rax     ; move the result to the stack\n", 
                destination->offset,
                source->offset,
                result->offset);

            break;
        }
        case OP_DIV: // https://www.felixcloutier.com/x86/div
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            fprintf(generator->output,
                "    mov    rdx, 0                  ; setting rdx to 0 because it represents the top bits of the divident\n"
                "    mov    rax, [rbp-%d]           ; move the dividend to rax\n"
                "    idiv   qword [rbp-%d]          ; divide the rax with the operand (divisor)\n"
                "    mov    qword [rbp-%d], rax     ; move the result to the stack\n", 
                destination->offset,
                source->offset,
                result->offset);

            break;
        }
        case OP_MINUS: // https://www.felixcloutier.com/x86/neg
        {
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            int temp_reg = allocate_register(generator);

            fprintf(generator->output,
                "    mov    %s, [rbp-%d]            ; --\n"
                "    neg    %s                      ; --\n"
                "    mov    [rbp-%d], %s            ; --\n",
                register_list[temp_reg], arg->offset,
                register_list[temp_reg],
                result->offset, register_list[temp_reg]);

            free_register(generator, temp_reg);
            break;
        }
        case OP_EQ:
        case OP_NEQ:
        case OP_LT:
        case OP_LTE:
        case OP_GT:
        case OP_GTE:
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            int temp_reg_1 = allocate_register(generator);
            int temp_reg_2 = allocate_register(generator);

            // https://www.felixcloutier.com/x86/setcc
            // https://www.felixcloutier.com/x86/movzx
            switch (instruction->operation) 
            {
                case OP_EQ:
                    fprintf(generator->output,
                        "    mov    %s, [rbp-%d]            ; --\n"
                        "    cmp    %s, [rbp-%d]            ; --\n"
                        "    sete   al                      ; --\n"
                        "    movzx  %s, al                  ; --\n"
                        "    mov    [rbp-%d], %s            ; --\n",
                        register_list[temp_reg_1], destination->offset,
                        register_list[temp_reg_1], source->offset,
                        register_list[temp_reg_2], 
                        result->offset, register_list[temp_reg_2]);
                    break;
                case OP_NEQ:
                    fprintf(generator->output,
                        "    mov    %s, [rbp-%d]            ; --\n"
                        "    cmp    %s, [rbp-%d]            ; --\n"
                        "    setne  al                      ; --\n"
                        "    movzx  %s, al                  ; --\n"
                        "    mov    [rbp-%d], %s            ; --\n",
                        register_list[temp_reg_1], destination->offset,
                        register_list[temp_reg_1], source->offset,
                        register_list[temp_reg_2], 
                        result->offset, register_list[temp_reg_2]);
                    break;
                case OP_LT:
                    fprintf(generator->output,
                        "    mov    %s, [rbp-%d]            ; --\n"
                        "    cmp    %s, [rbp-%d]            ; --\n"
                        "    setl   al                      ; --\n"
                        "    movzx  %s, al                  ; --\n"
                        "    mov    [rbp-%d], %s            ; --\n",
                        register_list[temp_reg_1], destination->offset,
                        register_list[temp_reg_1], source->offset,
                        register_list[temp_reg_2], 
                        result->offset, register_list[temp_reg_2]);
                    break;
                case OP_LTE:
                    fprintf(generator->output,
                        "    mov    %s, [rbp-%d]            ; --\n"
                        "    cmp    %s, [rbp-%d]            ; --\n"
                        "    setle  al                      ; --\n"
                        "    movzx  %s, al                  ; --\n"
                        "    mov    [rbp-%d], %s            ; --\n",
                        register_list[temp_reg_1], destination->offset,
                        register_list[temp_reg_1], source->offset,
                        register_list[temp_reg_2], 
                        result->offset, register_list[temp_reg_2]);
                    break;
                case OP_GT:
                    fprintf(generator->output,
                        "    mov    %s, [rbp-%d]            ; --\n"
                        "    cmp    %s, [rbp-%d]            ; --\n"
                        "    setg   al                      ; --\n"
                        "    movzx  %s, al                  ; --\n"
                        "    mov    [rbp-%d], %s            ; --\n", 
                        register_list[temp_reg_1], destination->offset,
                        register_list[temp_reg_1], source->offset,
                        register_list[temp_reg_2], 
                        result->offset, register_list[temp_reg_2]);
                    break;
                case OP_GTE:
                    fprintf(generator->output,
                        "    mov    %s, [rbp-%d]            ; --\n"
                        "    cmp    %s, [rbp-%d]            ; --\n"
                        "    setge  al                      ; --\n"
                        "    movzx  %s, al                  ; --\n"
                        "    mov    [rbp-%d], %s            ; --\n",
                        register_list[temp_reg_1], destination->offset,
                        register_list[temp_reg_1], source->offset,
                        register_list[temp_reg_2], 
                        result->offset, register_list[temp_reg_2]);
                    break;
            }

            free_register(generator, temp_reg_1);
            free_register(generator, temp_reg_2);
            break;
        }
        case OP_NOT: // https://www.felixcloutier.com/x86/not
        {
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; \n"
                "    xor    rax, 1                  ; \n"
                "    mov    [rbp-%d], rax           ; \n", 
                arg->offset, 
                result->offset);

            break;
        }
        case OP_AND: // https://www.felixcloutier.com/x86/and
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; \n"
                "    and    rax, [rbp-%d]           ; \n"
                "    mov    [rbp-%d], rax           ; \n", 
                destination->offset, 
                source->offset, 
                result->offset);

            break;
        }
        case OP_OR: // https://www.felixcloutier.com/x86/or
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; \n"
                "    or     rax, [rbp-%d]           ; \n"
                "    mov    [rbp-%d], rax           ; \n", 
                destination->offset, 
                source->offset, 
                result->offset);

            break;
        }
        case OP_GOTO_IF_FALSE: 
        {
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);
            int temp_reg = allocate_register(generator);

            fprintf(generator->output,
                "    mov    %s, [rbp-%d]            ; --\n"
                "    cmp    %s, 0                   ; --\n"
                "    je     %s                      ; --\n", 
                register_list[temp_reg], arg->offset,
                register_list[temp_reg], 
                instruction->label);

            free_register(generator, temp_reg);
            break;
        }
        case OP_LABEL:
        {
            fprintf(generator->output, "%s:\n", instruction->label);
            break;
        }
        case OP_GOTO:
        {
            fprintf(generator->output, "    jmp    %s\n", instruction->label);
            break;
        }
        case OP_FUNCTION_BEGIN:
        {
            // Change the scope to the functions scope
            Symbol* symbol = scope_lookup(generator->local, instruction->label);
            generator->local = symbol->type->function.scope;
            
            // NOTE(timo): Even if the user does not use the command line
            // arguments, we will save them.
            if (strcmp(generator->local->name, "main") == 0)
            {
                fprintf(generator->output,
                    "    add    rsi, 8                  ; increment argv pointer and skip the program name\n"
                    "    push   rsi                     ; push argv to stack before starting main program\n"
                    "    sub    rdi, 1                  ; decrement argument count by 1 to skip the program name\n"
                    "    push   rdi                     ; push argc to stack before starting main program\n");
            }

            fprintf(generator->output, 
                "    push   rbp\n"
                "    mov    rbp, rsp                ; started stack frame\n"
                "    sub    rsp, %d                 ; allocate memory for local variables from stack\n", 
                instruction->size);

            // NOTE(timo): There is no need to save the callee saved registers
            // since we don't utilize the registers at all.

            break;
        }
        case OP_PARAM_PUSH:
        {
            Symbol* parameter = scope_lookup(generator->local, instruction->arg1);

            fprintf(generator->output,
                "    push   qword[rbp-%d]           ; pushing function parameter\n", 
                parameter->offset);

            break;
        }
        case OP_PARAM_POP:
        {
            Symbol* parameter = scope_lookup(generator->local, instruction->arg1);
            
            fprintf(generator->output,
                "    pop    qword [rbp-%d]          ; popping function parameter\n", 
                parameter->offset);

            break;
        }
        case OP_FUNCTION_END:
        {
            fprintf(generator->output, "%s_epilogue:\n", instruction->label);

            // NOTE(timo): There is no need to restore the callee saved registers
            // since we don't utilize the registers at all.

            // Restore the stack frame and return to the caller. The stack frame
            // is restored with the `leave` instruction which handles the restoring
            // automatically.
            // TODO(timo): Hacky solution to get things work for now. The main 
            // function is handled at the code_generate() function. Main function
            // is different because it prints the result message for the user.
            // Maybe there is a better way to handle this.
            if (strcmp(generator->local->name, "main") != 0)
            {
                fprintf(generator->output,
                    "    leave\n"
                    "    ret\n");
            }

            // Set the current scope to the enclosing scope
            generator->local = generator->local->enclosing;

            // TODO(timo): This should be removed when more flexible scoping is 
            // added to the language.
            assert(generator->local == generator->global);
            break;
        }
        case OP_RETURN:
        {
            Symbol* value = scope_lookup(generator->local, instruction->arg1);
            
            // NOTE(timo): Jump to function epilogue is really not necessary 
            // right now since we only allow one return per function but so
            // this is in here purely for future updates.
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; put return value to rax\n"
                "    jmp    %s_epilogue             ; run the function epilogue\n", 
                value->offset, 
                generator->local->name);

            // NOTE(timo): This is not needed to anything at the moment since
            // we don't actually utilize the registers and all the used registers
            // are free'd right after they are used.
            // free_registers();
            break;
        }
        case OP_CALL:
        {
            Symbol* value = scope_lookup(generator->local, instruction->arg1);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            fprintf(generator->output,
                "    call   %s                      ; Calling the function\n"
                "    mov    [rbp-%d], rax           ; Moving the function result to the result address\n",
                value->identifier,
                result->offset);

            // NOTE(timo): The parameters are popped one by one with the OP_PARAM_POP
            // instruction. That could also be done with single instruction by
            // just adding n bytes to the stack.

            // NOTE(timo): Also there is no need to restore the caller saved
            // registers, since we don't actually utilize them to anything.

            break;
        }
        case OP_DEREFERENCE:
        {
            // NOTE(timo): This operation is specialized to dereferencing
            // elements from array of integers, since the only place where
            // we have something to dereference like this, is the argv in
            // the main program. Later this should of course be changed.

            Symbol* variable = scope_lookup(generator->local, instruction->arg1);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; \n"
                "    mov    rdi, [rax]              ; \n"
                // TODO(timo): Type check to make sure the argument is a digit/number 
                // before calling atoll. If argument is not a number, show error and exit
                "    call   atoll                   ; \n"
                "    mov    [rbp-%d], rax           ; \n", 
                variable->offset, 
                result->offset);

            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
                ":CODE_GENERATOR - Unreachable: Invalid operation in code_generate_instruction() '%s' / '%d'", 
                operation_str(instruction->operation), instruction->operation);
            array_push(generator->diagnostics, _diagnostic);
            break;
        }
    }
}


void code_generate(Code_Generator* generator)
{
    // Open the file handle to the ASM file.
    if ((generator->output = fopen(generator->asm_file, "w")) == NULL)
    {
        Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
            "Unable to open the assembly file");
        array_push(generator->diagnostics, _diagnostic);
        return;
    }
    
    // Generate template start with externs and the initial global variables.
    fprintf(generator->output,
        "; ----------------------------------------\n"
        "; %s\n"
        "; ----------------------------------------\n"
        "\n"
        "    global main\n"
        "    extern printf\n"
        "    extern atoll\n"
        "\n"
        "    section .data\n"
        "true:          dq 1                    ; Represents boolean true\n"
        "false:         dq 0                    ; Represents boolean false\n"
        "true_str:      db 'true', 0            ; String representation for true\n"
        "false_str:     db 'false', 0           ; String representation for false\n",
            generator->asm_file);
    
    // Generate the global variables from the symbol table
    for (int i = 0; i < generator->global->symbols->capacity; i++)
    {
        Symbol* symbol = generator->global->symbols->entries[i].value;

        if (symbol && symbol->kind == SYMBOL_VARIABLE)
            fprintf(generator->output,
                "%s:    dq %d\n", symbol->identifier, symbol->value.integer);
    }

    // Start generating the code itself
    fprintf(generator->output,
        "\n"
        "    section .text\n");
    
    // Generate the instructions
    for (int i = 0; i < generator->instructions->length; i++)
        code_generate_instruction(generator, generator->instructions->items[i]);
    
    // TODO(timo):
    // Should the main function has its own beginning and end operations?
    // We should probably have some way to declare builtin functions etc.

    // Generate the end of the main function and end of the program as a whole.
    // The end is generated based on the return type of the program. Since it
    // is possible to return booleans as well, the booleans will be represented
    // in their canonical form in the return message and not just by 0 and 1.
    Symbol* main = scope_lookup(generator->global, "main");

    if (type_is_integer(main->type->function.return_type))
    {
        fprintf(generator->output,
            "    mov    rdi, return_message_success ; first argument of printf - format\n"
            "    mov    rsi, rax                    ; second argument of printf - formatted value\n"
            "    xor    rax, rax                    ; because the varargs printf uses\n"
            "    call   printf                      ; print the return value - printf(format, value)\n"
            "    mov    rax, 0                      ; exit success\n"
            "    leave                              ; leave the current stack frame without manually setting it\n"
            "    pop rsi                            ; pop the argv from the stack\n"
            "    pop rdi                            ; pop the argc from the stack\n"
            "    ret                                ; exit the program\n"
            "\n"
            "return_message_success:\n"
            "    db 'Program exited with the value %s', 10, 0", "%d");
    }
    else if (type_is_boolean(main->type->function.return_type))
    {
        fprintf(generator->output,
            "    mov    rdi, return_message_success ; first argument of printf - format\n"
            "    cmp    rax, [true]                 ; check if the return value is true\n"
            "    jne    end_false\n"
            "    mov    rsi, true_str               ; second argument of printf - formatted value\n"
            "    jmp    end_print\n"
            "end_false:\n"
            "    mov    rsi, false_str              ; second argument of printf - formatted value\n"
            "end_print:\n"
            "    xor    rax, rax                    ; because the varargs printf uses\n"
            "    call   printf                      ; print the return value - printf(format, value)\n"
            "    mov    rax, 0                      ; exit success\n"
            "    leave                              ; leave the current stack frame without manually setting it\n"
            "    pop rsi                            ; pop the argv from the stack\n"
            "    pop rdi                            ; pop the argc from the stack\n"
            "    ret                                ; exit the program\n"
            "\n"
            "return_message_success:\n"
            "    db 'Program exited with the value %s', 10, 0", "%s");
    }

    fclose(generator->output);
}
