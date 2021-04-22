#include "t.h"


// TODO(timo): Separate Linux kernel argument registers and general purpose registers?
// TODO(timo): How about 8-, 16- and 32-bit registers?
static int registers[] = { 1, 1, 1, 1, 1, 1 };
static const char* register_list[] = { "r10", "r11", "r12", "r13", "r14", "r15" };


static void free_registers()
{
    for (int i = 0; i < 6; i++)
        registers[i] = 1;
}


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

    array_free(generator->diagnostics);

    // NOTE(timo): Generator is not being freed because it is in the stack at the top level
}


void code_generate_instruction(Code_Generator* generator, Instruction* instruction)
{
    switch (instruction->operation)
    {
        case OP_ADD: // https://www.felixcloutier.com/x86/add
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            if (destination != NULL) // get variable from stack to register
            {
                destination->_register = allocate_register(generator);
                fprintf(generator->output,
                    "    mov    %s, [rbp-%d]            ; move variable from the stack to the register\n", 
                    register_list[destination->_register], destination->offset);
            }

            if (source != NULL) // get variable from stack to register
            {
                source->_register = allocate_register(generator);
                fprintf(generator->output,
                    "    mov    %s, [rbp-%d]            ; move variable from the stack to the register\n", 
                    register_list[source->_register], source->offset);
            }

            if (result != NULL) // add two registers together and save the result to the stack
            {
                fprintf(generator->output,
                    "    add    %s, %s                  ; add the values together\n", 
                    register_list[destination->_register], register_list[source->_register]);
                fprintf(generator->output,
                    "    mov    qword [rbp-%d], %s      ; move the result to the stack\n", 
                    result->offset, register_list[destination->_register]);
            }
            else // add two registers together and set the result register
            {
                result->_register = destination->_register;
                fprintf(generator->output,
                    "    add    %s, %s                  ; add the values together\n", 
                    register_list[destination->_register], register_list[source->_register]);
            }

            // TODO(timo): I'm constantly running out of registers, so maybe I shouldnt play around
            // with them at all until I actually know what I'm doing. KISS = Keep It Stack Simple
            // free_register(generator, source->_register);
            free_registers();
            source->_register = -1;
            break;
        }
        case OP_SUB: // https://www.felixcloutier.com/x86/sub
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            if (destination != NULL) // get variable from stack to register
            {
                destination->_register = allocate_register(generator);
                fprintf(generator->output,
                    "    mov    %s, [rbp-%d]            ; move the variable from the stack to the register\n", 
                    register_list[destination->_register], destination->offset);
            }

            if (source != NULL) // get variable from stack to register
            {
                source->_register = allocate_register(generator);
                fprintf(generator->output,
                    "    mov    %s, [rbp-%d]            ; move the variable from the stack to the register\n", 
                    register_list[source->_register], source->offset);
            }

            if (result != NULL) // subtract two registers from each other and save the result to the stack
            {
                fprintf(generator->output,
                    "    sub    %s, %s                  ; subtract two values from each other\n", 
                    register_list[destination->_register], register_list[source->_register]);
                fprintf(generator->output,
                    "    mov    qword [rbp-%d], %s      ; move the result to the stack\n", 
                    result->offset, register_list[destination->_register]);
            }
            else // subtract two registers from each other and set the result register
            {
                result->_register = destination->_register;
                fprintf(generator->output,
                    "    sub    %s, %s                  ; subtract two values from each other\n", 
                    register_list[destination->_register], register_list[source->_register]);
            }

            free_register(generator, source->_register);
            source->_register = -1;
            break;
        }
        case OP_MUL: // https://www.felixcloutier.com/x86/mul
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            if (destination != NULL) // source 1
            {
                fprintf(generator->output,
                    "    mov    rax, [rbp-%d]           ; move the variable being multiplied from the stack to the rax\n", 
                    destination->offset);
            }

            if (source != NULL) // source 2
            {
                fprintf(generator->output,
                    "    mul    qword [rbp-%d]          ; multiply the value in rax with the argument\n", 
                    source->offset);
            }

            if (result != NULL) // rdx:rax
            {
                fprintf(generator->output,
                    "    mov    qword [rbp-%d], rax     ; move the result to the stack\n", 
                    result->offset);
            }
            else
            {
                result->_register = destination->_register;

                fprintf(generator->output,
                    "    mov    rax, %s                 ; move the value to be multiplied to rax\n"
                    "    mul    %s                      ; multiply the value in the rax with the argument\n"
                    "    mov    %s, rax                 ; move the result to the result register\n", 
                    register_list[destination->_register], register_list[source->_register], register_list[result->_register]);

                free_register(generator, source->_register);
                source->_register = -1;
            }

            break;
        }
        case OP_DIV: // https://www.felixcloutier.com/x86/div
        {
            // rax <- (rax / src)
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);

            fprintf(generator->output,
                "    mov    rdx, 0                  ; setting rdx to 0 because it represents the top bits of the divident\n");

            if (destination != NULL)
            {
                fprintf(generator->output,
                    "    mov    rax, [rbp-%d]           ; --\n", 
                    destination->offset);
            }
            if (source != NULL)
            {
                fprintf(generator->output,
                    "    div     qword [rbp-%d]         ; --\n", 
                    source->offset);
            }

            if (result != NULL)
            {
                fprintf(generator->output,
                    "    mov    qword [rbp-%d], rax     ; --\n", 
                    result->offset);
            }
            else
            {
                result->_register = destination->_register;

                fprintf(generator->output,
                    "    mov    rdx, 0                  ; setting rdx to 0 because it represents the top bits of the input divident\n"
                    "    mov    rax, %s                 ; \n"
                    "    div    %s                      ; divide the rax with the source\n"
                    "    mov    %s, rax                 ; move the result to the result register\n", 
                    register_list[destination->_register], register_list[source->_register], register_list[result->_register]);

                free_register(generator, source->_register);
                source->_register = -1;
            }

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
            
            // NOTE(timo): Bitwise not is not the same as straight logical not, since
            // bitwise not will flip all the bits, so there has to be and 1 afterwards
            // TODO(timo): Actually xor 1 could make this a one instruction operation
            // instead of two with not + and
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; \n"
                "    not    rax                     ; \n"
                "    and    rax, 1                  ; \n"
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
                "    mov    rax, [rbp-%d]       ; \n"
                "    and    rax, [rbp-%d]       ; \n"
                "    mov    [rbp-%d], rax       ; \n", 
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
                "    mov    rax, [rbp-%d]       ; \n"
                "    or     rax, [rbp-%d]       ; \n"
                "    mov    [rbp-%d], rax       ; \n", 
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
        case OP_COPY:
        {
            // TODO(timo): Assignment to global variable

            // NOTE(timo): This result will always be set, since it is temporary
            Symbol* result = scope_lookup(generator->local, instruction->result);
            Symbol* arg = scope_lookup(generator->local, instruction->arg1);

            // NOTE(timo): If argument symbol is found, argument is a variable, else it is probably constant
            if (arg != NULL)
            {
                if (arg->scope == generator->global)
                {
                    fprintf(generator->output,
                        "    mov    rax, [%s]               ; move global variable to register\n"
                        "    mov    [rbp-%d], rax           ; move global variable from register to stack\n", 
                        arg->identifier, result->offset);
                }
                // TODO(timo): For now this is just a super quick hacky solution to get things working
                // TODO(timo): I should fixt the assigning of the offsets so I can get rid of alot of
                // these conditions
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
            else // This is used when literal constants are being copied
            {
                // NOTE(timo): Is this actually needed? I mean at this point we save pretty much everything to
                // stack at this point, so there is probably no need for register allocation here
                // result->_register = allocate_register(generator);
                
                if (result->type->kind == TYPE_INTEGER)
                    fprintf(generator->output,
                        "    mov    qword [rbp-%d], %s      ; --\n", 
                        result->offset, instruction->arg1);
                else if (result->type->kind == TYPE_BOOLEAN)
                    fprintf(generator->output,
                        "    mov    rax, [%s]               ; --\n"
                        "    mov    qword [rbp-%d], rax     ; --\n",
                        instruction->arg1, result->offset);
            }

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

            if (symbol != NULL && symbol->type->kind == TYPE_FUNCTION)
                generator->local = symbol->type->function.scope;
            
            // TODO(timo): Handle the main function separately somewhere else so 
            // we don't have to do this check every time there is a function
            // TODO(timo): Is this really even needed. Since if the user does not 
            // use the command line arguments, so what? They can still be pushed 
            // to the stack normally even if they are not used. That way I can 
            // remove this unnecessary code?
            if (strcmp(generator->local->name, "main") == 0)
            {
                Symbol* argc = scope_lookup(generator->local, "argc");
                Symbol* argv = scope_lookup(generator->local, "argv");

                if (argv != NULL)
                    fprintf(generator->output,
                        "    add    rsi, 8                  ; increment argv pointer and skip the program name\n"
                        "    push   rsi                     ; push argv to stack before starting main program\n");

                if (argc != NULL)
                    fprintf(generator->output,
                        "    sub    rdi, 1                  ; decrement argument count by 1 to skip the program name\n"
                        "    push   rdi                     ; push argc to stack before starting main program\n");
            }

            fprintf(generator->output, 
                "    push   rbp\n"
                "    mov    rbp, rsp                ; started stack frame\n"
                "    sub    rsp, %d                 ; allocate memory for local variables from stack\n", 
                instruction->size);

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

            // Restore the stack
            // NOTE(timo): Parameters are popped one by one with OP_PARAM_POP

            // Restore the arguments/registers
            // TODO(timo): This should be implemented when caller/callee saved registers
            // are taken into account. Now everything is being passed through stack.

            // Return to the caller
            // TODO(timo): Hacky solution to get things work for now. Main function is
            // handled at the main code_generate() function. Maybe there is a better way.
            if (strcmp(generator->local->name, "main") != 0)
            {
                fprintf(generator->output,
                    "    leave\n"
                    "    ret\n");
            }

            // Set the current scope to the enclosing scope
            generator->local = generator->local->enclosing;

            // TODO(timo): This should be removed when more flexible scoping is added
            assert(generator->local == generator->global);
            break;
        }
        case OP_RETURN:
        {
            Symbol* value = scope_lookup(generator->local, instruction->arg1);
            
            // NOTE(timo): Jump to function epilogue is really not necessary right now 
            // since we only allow one return per function
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]           ; put return value to rax\n"
                "    jmp    %s_epilogue             ; run the function epilogue\n", 
                value->offset, 
                generator->local->name);

            free_registers();
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

            break;
        }
        case OP_DEREFERENCE:
        {
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
    if ((generator->output = fopen(generator->asm_file, "w")) == NULL)
    {
        Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
            "Unable to open the assembly file");
        array_push(generator->diagnostics, _diagnostic);
        return;
    }
    
    // Template start and externs
    fprintf(generator->output,
        "; ----------------------------------------\n"
        "; main.asm\n"
        "; ----------------------------------------\n"
        "\n"
        "    global main\n"
        "    extern printf\n"
        "    extern atoll\n"
        "\n");

    // Global variables
    fprintf(generator->output,
        "    section .data\n"
        "true:          dq 1                    ; Represents boolean true\n"
        "false:         dq 0                    ; Represents boolean false\n"
        "true_str:      db 'true', 0            ; String representation for true\n"
        "false_str:     db 'false', 0           ; String representation for false\n");

    for (int i = 0; i < generator->global->symbols->capacity; i++)
    {
        Symbol* symbol = generator->global->symbols->entries[i].value;

        if (symbol && symbol->kind == SYMBOL_VARIABLE)
            fprintf(generator->output,
                "%s:    dq %d\n", symbol->identifier, symbol->value.integer);
    }

    // Start the code itself
    fprintf(generator->output,
        "\n"
        "    section .text\n");
    
    // Generate the instructions
    for (int i = 0; i < generator->instructions->length; i++)
        code_generate_instruction(generator, generator->instructions->items[i]);
    
    // TODO(timo):
    // Should the main function has its own beginning and end operations?
    // We should probably have some way to declare builtin functions etc.

    // Template end
    Symbol* main = scope_lookup(generator->global, "main");
    Symbol* argc = scope_lookup(main->type->function.scope, "argc");
    Symbol* argv = scope_lookup(main->type->function.scope, "argv");

    if (main->type->function.return_type->kind == TYPE_INTEGER)
    {
        fprintf(generator->output,
            "    mov    rdi, return_message_success ; first argument of printf - format\n"
            "    mov    rsi, rax                    ; second argument of printf - formatted value\n"
            "    xor    rax, rax                    ; because the varargs printf uses\n"
            "    call   printf                      ; print the return value - printf(format, value)\n"
            "    mov    rax, 0                      ; exit success\n"
            "    leave                              ; leave the current stack frame without manually setting it\n");

        if (argv != NULL)
            fprintf(generator->output,
                "    pop rsi                            ; pop the argv from the stack\n");
        if (argc != NULL)
            fprintf(generator->output,
                "    pop rdi                            ; pop the argc from the stack\n");

        fprintf(generator->output,
            "    ret                                ; exit the program\n"
            "\n"
            "return_message_success:\n"
            "    db 'Program exited with the value %s', 10, 0", "%d");
    }
    else if (main->type->function.return_type->kind == TYPE_BOOLEAN)
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
            "    leave                              ; leave the current stack frame without manually setting it\n");

        if (argv != NULL)
            fprintf(generator->output,
                "    pop rsi                            ; pop the argv from the stack\n");
        if (argc != NULL)
            fprintf(generator->output,
                "    pop rdi                            ; pop the argc from the stack\n");
        
        fprintf(generator->output,
            "    ret                                ; exit the program\n"
            "\n"
            "return_message_success:\n"
            "    db 'Program exited with the value %s', 10, 0", "%s");
    }

    fclose(generator->output);
}
