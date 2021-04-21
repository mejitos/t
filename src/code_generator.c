#include "t.h"


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
                                    .index = 0,
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

            free_register(generator, source->_register);
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

            if (destination != NULL)
            {
                destination->_register = allocate_register(generator);
                fprintf(generator->output,
                    "    mov    %s, [rbp-%d]            ; --\n", 
                    register_list[destination->_register], destination->offset);
            }
            if (source != NULL)
            {
                source->_register = allocate_register(generator);
                fprintf(generator->output,
                    "    mov    %s, [rbp-%d]            ; --\n", 
                    register_list[source->_register], source->offset);
            }

            fprintf(generator->output,
                "    cmp    %s, %s                  ; --\n", 
                register_list[destination->_register], register_list[source->_register]);

            // TODO(timo): What to save into the result? I mean we could save the correct jump
            // instruction at this point already and just print it at the GOTO_IF_FALSE

            free_register(generator, destination->_register);
            free_register(generator, source->_register);
            break;
        }
        case OP_NOT:
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
                arg->offset, result->offset);
            // NOTE(timo): There is no need to save a reference to created boolean variables,
            // since the value will be checked at the end so if it is 0, it will be false and
            // if it is 1, it will be true.
            break;
        }
        case OP_AND:
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]       ; \n"
                "    and    rax, [rbp-%d]       ; \n"
                "    mov    [rbp-%d], rax       ; \n", 
                destination->offset, source->offset, result->offset);
            break;
        }
        case OP_OR:
        {
            Symbol* destination = scope_lookup(generator->local, instruction->arg1);
            Symbol* source = scope_lookup(generator->local, instruction->arg2);
            Symbol* result = scope_lookup(generator->local, instruction->result);
            
            fprintf(generator->output,
                "    mov    rax, [rbp-%d]       ; \n"
                "    or     rax, [rbp-%d]       ; \n"
                "    mov    [rbp-%d], rax       ; \n", 
                destination->offset, source->offset, result->offset);
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
                        "    jne    %s\n", 
                        instruction->label);
                    break;
                case OP_NEQ:
                    fprintf(generator->output,
                        "    je      %s\n", 
                        instruction->label);
                    break;
                case OP_LT:
                    fprintf(generator->output,
                        "    jge    %s\n", 
                        instruction->label);
                    break;
                case OP_LTE:
                    fprintf(generator->output,
                        "    jg      %s\n", 
                        instruction->label);
                    break;
                case OP_GT:
                    fprintf(generator->output,
                        "    jle    %s\n", 
                        instruction->label);
                    break;
                case OP_GTE:
                    fprintf(generator->output,
                        "    jl     %s\n", 
                        instruction->label);
                    break;
                case OP_COPY:
                {
                    // NOTE(timo): The preceding was a boolean copied to variable?
                    Symbol* arg = scope_lookup(generator->local, instruction->arg1);
                    
                    if (arg->type->kind == TYPE_BOOLEAN)
                    {
                        // If the value is true, compare against 0
                        // If the value is false, compare against 1
                        // TODO(timo): Allocate the registers through the functions
                        fprintf(generator->output,
                            "    mov    r10, %d\n", 
                            arg->value.boolean ? 0 : 1);
                        fprintf(generator->output,
                            "    mov    r11, [rbp-%d]\n", 
                            arg->offset);
                        fprintf(generator->output,
                            "    cmp    r10, r11\n");
                        fprintf(generator->output,
                            "    jne    %s\n", 
                            instruction->label);
                    }
                    break;
                }
                default:
                    // NOTE(timo): If the previous condition was something else than relational
                    // operation, there is no need for the preceding operation. Not sure how it
                    // will be with the logical expressions
                    // TODO(timo): Create diagnostic
                    printf("[CODE_GENERATOR] - Invalid preceding instruction in OP_GOTO_IF_FALSE '%s'\n", operation_str(preceding->operation));
                    exit(1);
            }
            break;
        }
        case OP_COPY:
        {
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
                        "    mov    rax, [rbp+%d]           ; move parameter from stack to rax\n", 
                        arg->offset);
                    fprintf(generator->output,
                        "    mov    qword [rbp-%d], rax     ; move parameter from rax to stack\n", 
                        result->offset);
                }
                else
                {
                    fprintf(generator->output,
                        "    mov    rax, [rbp-%d]           ; --\n", 
                        arg->offset);
                    fprintf(generator->output,
                        "    mov    qword [rbp-%d], rax     ; --\n", 
                        result->offset);
                }
            }
            else // This is used when literal constants are being copied
            {
                // NOTE(timo): Is this actually needed? I mean at this point we save pretty much everything to
                // stack at this point, so there is probably no need for register allocation here
                // result->_register = allocate_register(generator);
                
                if (result->type->kind == TYPE_INTEGER)
                    fprintf(generator->output,
                        "    mov    qword [rbp-%d], %s      ; --\n", result->offset, instruction->arg1);
                else if (result->type->kind == TYPE_BOOLEAN)
                {
                    fprintf(generator->output,
                        "    mov    rax, [%s]               ; --\n", instruction->arg1);
                    fprintf(generator->output,
                        "    mov    qword [rbp-%d], rax     ; --\n", result->offset);
                }
            }

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
                "    jmp    %s                      ; --\n", instruction->label);
            break;
        }
        case OP_FUNCTION_BEGIN:
        {
            // TODO(timo): That scope change should probably happen in here and not in OP_LABEL
            // but we do need the name of the function. Should we save it into the function begin instruction?
            
            // TODO(timo): Handle the main function separately somewhere else so we don't have to do this
            // check every time there is a function
            if (strcmp(generator->local->name, "main") == 0)
            {
                Symbol* argc = scope_lookup(generator->local, "argc");
                Symbol* argv = scope_lookup(generator->local, "argv");

                if (argv != NULL)
                    fprintf(generator->output,
                        "    add    rsi, 8                  ; -- \n"
                        "    push   rsi                     ; push argv to stack before starting main program\n");

                if (argc != NULL)
                    fprintf(generator->output,
                        "    sub    rdi, 1                  ; -- \n"
                        "    push   rdi                     ; push argc to stack before starting main program\n");

                fprintf(generator->output,
                    "    push   rbp\n"
                    "    mov    rbp, rsp                ; started stack frame\n");
                fprintf(generator->output,
                    "    sub    rsp, %d                 ; allocate memory for local variables from stack\n", instruction->size);

                /*
                if (argc != NULL)
                {
                    fprintf(generator->output,
                        "    mov    rax, [rbp+16]            ; decrement argument count by 1 to skip the program name\n"
                        "    mov    [rbp-%d], rax           ; save argc to the stack\n", argc->offset);
                        // "    sub    rdi, 1                  ; decrement argument count by 1 to skip the program name\n"
                        // "    mov    [rbp-%d], rdi           ; save argc to the stack\n", argc->offset);
                }
                if (argv != NULL)
                {
                    fprintf(generator->output,
                        "    add    rsi, 8                  ; increment the argv pointer and skip the program name\n"
                        "    mov    [rbp-%d], rsi           ; save pointer to argv to the stack\n", argv->offset);
                }
                */
            }
            else
            {
                // These instructions are same as instruction "enter, N" where enter sets the stackframe 
                // and if N is given, N bytes of memory will be reserved from the stack
                fprintf(generator->output, 
                    "    push   rbp\n"
                    "    mov    rbp, rsp                ; started stack frame\n");

                fprintf(generator->output,
                    "    sub    rsp, %d                 ; allocate memory for local variables from stack\n", instruction->size);
            }
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
                    "    push   qword[rbp-%d]          ; pushing function parameter\n", parameter->offset);
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
                    "    pop    qword [rbp-%d]          ; popping function parameter\n", parameter->offset);
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
                    // "    add    rsp, 8\n"
                    // "    leave\n"
                    "    mov    rsp, rbp                ; restore stack frame\n"
                    "    pop    rbp\n"
                    "    ret\n");
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
                    "    mov    rax, [rbp-%d]           ; put return value to rax\n", value->offset);
            }
            else
            {
                fprintf(generator->output,
                    "    mov    rax, %s                 ; put return value to rax\n", register_list[value->_register]);
            }

            // TODO(timo): Make jump to function epilogue where the function will be teared down
            // Even though the jump is not necessary right now since we only have one return per function
            fprintf(generator->output,
                "    jmp    %s_epilogue             ; run the function epilogue\n", generator->local->name);

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
                    "    call   %s                     ; calling the function %s\n", value->identifier, value->identifier);

                /*
                fprintf(generator->output,
                    "    add    rsp, 16\n");
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
                    "    mov    [rbp-%d], rax           ; Moving the function result to the result address\n", result->offset);
            }
            else
            {
                // TODO(timo): Just use the registers or error
            }

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
                "    call   atoll                  ; \n"
                "    mov    [rbp-%d], rax           ; \n", variable->offset, result->offset);

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
    
    for (int i = 0; i < generator->instructions->length; i++)
    {
        code_generate_instruction(generator, generator->instructions->items[i]);
        generator->index++;
    }
    
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
