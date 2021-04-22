#include "t.h"


void ir_generator_init(IR_Generator* generator, Scope* global)
{
    *generator = (IR_Generator) { .temp = 0,
                                  .label = 0,
                                  .global = global,
                                  .diagnostics = array_init(sizeof (Diagnostic*)),
                                  .instructions = array_init(sizeof (Instruction*)) };

    generator->local = generator->global;
}


void ir_generator_free(IR_Generator* generator)
{
    // Free diagnostics
    for (int i = 0; i < generator->diagnostics->length; i++)
    {
        Diagnostic* diagnostic = generator->diagnostics->items[i];

        free((char*)diagnostic->message);
        diagnostic->message = NULL;

        free(diagnostic);
        diagnostic = NULL;
    }

    array_free(generator->diagnostics);

    // Free instructions
    for (int i = 0; i < generator->instructions->length; i++)
        instruction_free(generator->instructions->items[i]);

    array_free(generator->instructions);

    // NOTE(timo): The global scope will be freed by the resolver

    // NOTE(timo): Generator itself is not being freed since it is
    // initialized in the stack in top level function
}


static char* temp_label(IR_Generator* generator)
{
    char* t = xmalloc(sizeof (char) * 14 + 1);
    snprintf(t, 15, "_t%d", generator->temp++);

    return t;
}


static char* label(IR_Generator* generator)
{
    char* l = xmalloc(sizeof (char) * 14 + 1);
    snprintf(l, 15, "_l%d", generator->label++);

    return l;
}


char* ir_generate_expression(IR_Generator* generator, AST_Expression* expression)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
        {
            char* arg = (char*)expression->literal->lexeme;
            char* temp = temp_label(generator);

            Instruction* instruction = instruction_copy(arg, temp);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->type));
            free(temp);

            return instruction->result;
        }
        case EXPRESSION_UNARY:
        {
            char* operator = (char*)expression->unary._operator->lexeme;
            char* operand = ir_generate_expression(generator, expression->unary.operand);
            char* temp = temp_label(generator);

            Instruction* instruction;

            switch (expression->unary._operator->kind)
            {
                case TOKEN_MINUS:
                    instruction = instruction_minus(operand, temp);
                    break;
                case TOKEN_NOT:
                    instruction = instruction_not(operand, temp);
                    break;
            }

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->type));
            free(temp);

            return instruction->result;
        }
        case EXPRESSION_BINARY:
        {
            char* operator = (char*)expression->binary._operator->lexeme;
            char* left = ir_generate_expression(generator, expression->binary.left); 
            char* right = ir_generate_expression(generator, expression->binary.right);
            char* temp = temp_label(generator);

            Instruction* instruction;

            switch (expression->binary._operator->kind)
            {
                case TOKEN_PLUS:
                    instruction = instruction_add(left, right, temp);
                    break;
                case TOKEN_MINUS:
                    instruction = instruction_sub(left, right, temp);
                    break;
                case TOKEN_MULTIPLY:
                    instruction = instruction_mul(left, right, temp);
                    break;
                case TOKEN_DIVIDE:
                    instruction = instruction_div(left, right, temp);
                    break;
                case TOKEN_IS_EQUAL:
                    instruction = instruction_eq(left, right, temp);
                    break;
                case TOKEN_NOT_EQUAL:
                    instruction = instruction_neq(left, right, temp);
                    break;
                case TOKEN_LESS_THAN:
                    instruction = instruction_lt(left, right, temp);
                    break;
                case TOKEN_LESS_THAN_EQUAL:
                    instruction = instruction_lte(left, right, temp);
                    break;
                case TOKEN_GREATER_THAN:
                    instruction = instruction_gt(left, right, temp);
                    break;
                case TOKEN_GREATER_THAN_EQUAL:
                    instruction = instruction_gte(left, right, temp);
                    break;
                // TODO(timo): Should these have labels and jumps and stuff
                // to get the short circuiting to work properly
                // Yep. Thats why need more context when handling these things
                // and thats why we really have to handle the condition expression
                // in its own function
                case TOKEN_AND:
                    instruction = instruction_and(left, right, temp);
                    // TODO(timo): The context here has to be condition of while/if?
                    // evaluate left
                    // if left false goto exit (condition := false)
                    // evaluate right
                    // if right false goto exit (condition := false)
                    // continue (if/while body) (condition := true)
                    // exit (if/while exit)
                    break;
                case TOKEN_OR:
                    instruction = instruction_or(left, right, temp);
                    // TODO(timo): The context here has to be condition of while/if?
                    // evaluate left
                    // if left true goto continue (condition := true)
                    // evaluate right
                    // if right true goto continue (condition := true)
                    // goto exit (condition := false)
                    // continue (if/while body)
                    // exit (if/while exit)
                    break;
            }
            
            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->type));
            free(temp);

            return instruction->result;
        }
        case EXPRESSION_VARIABLE:
        {
            char* arg = (char*)expression->literal->lexeme;
            char* temp = temp_label(generator);
            
            Instruction* instruction = instruction_copy(arg, temp);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->type));
            free(temp);

            return instruction->result;
        }
        case EXPRESSION_ASSIGNMENT:
        {
            char* arg = ir_generate_expression(generator, expression->assignment.value);
            char* result = (char*)expression->assignment.variable->identifier->lexeme;

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);

            return result;
        }
        case EXPRESSION_INDEX:
        {
            // TODO(timo): This case is great example of why we probably should have
            // functions to emit each of the operations, so we don't produce messy
            // things like this right here.
            Instruction* instruction;
            char* temp;

            // Generate the total offset for the accessed element
            char* subscript = ir_generate_expression(generator, expression->index.value);
            char* element_size = temp_label(generator); 
            // NOTE(timo): All types are 8 bytes wide for now
            instruction = instruction_copy("8", element_size);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->index.variable->type->array.element_type)); // TODO(timo): remove

            temp = temp_label(generator);
            instruction = instruction_mul(subscript, element_size, temp);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->index.variable->type->array.element_type));
            free(element_size);
            free(temp);
            
            // TODO(timo): Basically we should also copy the result of the multiplication to temp variable

            // Add the offset to the base pointer
            char* arg = ir_generate_expression(generator, expression->index.variable);
            temp = temp_label(generator);
            instruction = instruction_add(arg, instruction->result, temp);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->index.variable->type->array.element_type));
            free(temp);
        
            // Defererence the accessed element
            temp = temp_label(generator);
            instruction = instruction_dereference(instruction->result, temp, -1);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->index.variable->type->array.element_type));
            free(temp);

            return instruction->result;
        }
        case EXPRESSION_FUNCTION:
        {
            Instruction* instruction;
            // TODO(timo): Generate temp label where the return value is being put?
            // That is the thing this function should return. That label is probably
            // needed to have in the return statement so it can be set to the correct one?
            // OR the return statement sets the return address.
            
            // Function prologue

            // TODO(timo): Change the scope to function scope. For now it is handled
            // at the function declaration.
            
            instruction = instruction_function_begin((char*)generator->local->name);
            array_push(generator->instructions, instruction);

            // Function body 

            ir_generate_statement(generator, expression->function.body);
            
            // TODO(timo): Leave the function scope. For now it is handled
            // at the function declaration

            // NOTE(timo): At this point, the scope has been already set to the function scope
            // so therefore we already know the size of the function via the scope
            // We could also take this info at the code generation stage from the scope
            instruction->size = generator->local->offset;
            
            // Function epilogue
            
            instruction = instruction_function_end((char*)generator->local->name);
            array_push(generator->instructions, instruction);
            break;
        }
        case EXPRESSION_CALL:
        {
            Instruction* instruction;

            // Push the arguments to the stack/registers and save the argument
            // addresses to pop them later in correct order
            array* arguments = expression->call.arguments;
            char* args[arguments->length];

            for (int i = arguments->length - 1; i >= 0; i--)
            {
                AST_Expression* argument = (AST_Expression*)arguments->items[i];
                char* arg = ir_generate_expression(generator, argument);

                instruction = instruction_param_push(arg);
                array_push(generator->instructions, instruction);
                args[i] = arg;
            }

            // Call instruction itself
            char* arg = (char*)expression->call.variable->identifier->lexeme;
            char* temp = temp_label(generator);

            instruction = instruction_call(arg, temp, arguments->length);

            scope_declare(generator->local, symbol_temp(generator->local, instruction->result, expression->type));
            array_push(generator->instructions, instruction);
            free(temp);
            
            // Pop the params from the stack after the call has returned
            for (int i = 0; i < arguments->length; i++)
            {
                Instruction* instruction = instruction_param_pop(args[i]);
                array_push(generator->instructions, instruction);
            }

            return instruction->result;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
                ":IR_GENERATOR - Unreachable: Unexpected expression in ir_generate_expression()");
            array_push(generator->diagnostics, _diagnostic);
            // TODO(timo): What to return in error situation?
            break;
        }
    }
}


void ir_generate_statement(IR_Generator* generator, AST_Statement* statement)
{
    switch (statement->kind)
    {
        case STATEMENT_DECLARATION:
            ir_generate_declaration(generator, statement->declaration);
            break;
        case STATEMENT_EXPRESSION:
            ir_generate_expression(generator, statement->expression);
            break;
        case STATEMENT_BLOCK:
            for (int i = 0; i < statement->block.statements->length; i++)
                ir_generate_statement(generator, (AST_Statement*)statement->block.statements->items[i]);
            break;
        case STATEMENT_WHILE:
        {
            Instruction* instruction;

            // Local labels
            char* label_condition = label(generator); // condition
            char* label_exit = label(generator); // exit

            // TODO(timo): Quick hacky hack solution for break statements. There might be a better way.
            // TODO(timo): But what if there is nested while loops?
            generator->while_exit = label_exit;

            instruction = instruction_label(label_condition);
            array_push(generator->instructions, instruction);

            // if false, jump to label 2
            char* condition = ir_generate_expression(generator, statement->_while.condition);

            instruction = instruction_goto_if_false(condition, label_exit);
            array_push(generator->instructions, instruction);
            
            // Generate the body
            ir_generate_statement(generator, statement->_while.body);
            
            // Go back to the start of the loop to test the condition again
            instruction = instruction_goto(label_condition);
            array_push(generator->instructions, instruction);
            
            // Exit Label
            instruction = instruction_label(label_exit);
            array_push(generator->instructions, instruction);

            // TODO(timo): Quick hacky hack solution for break statements. There might be a better way.
            // TODO(timo): But what if there is nested while loops?
            generator->while_exit = NULL;

            free(label_condition);
            free(label_exit);
            break;
        }
        case STATEMENT_IF:
        {
            if (statement->_if._else != NULL) // if-then-else
            {
                Instruction* instruction;

                // Local labels
                char* label_else = label(generator);
                char* label_exit = label(generator);
                char* condition = ir_generate_expression(generator, statement->_if.condition);
                
                // Condition
                instruction = instruction_goto_if_false(condition, label_else);
                array_push(generator->instructions, instruction);

                // Generate the body
                ir_generate_statement(generator, statement->_if.then);
                
                // Goto exit
                instruction = instruction_goto(label_exit);
                array_push(generator->instructions, instruction);

                // Else label
                instruction = instruction_label(label_else);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if._else);
                
                // Exit label
                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);

                free(label_else);
                free(label_exit);
            }
            else // if-then
            {
                Instruction* instruction;

                // Local labels
                char* label_exit = label(generator);
                char* condition = ir_generate_expression(generator, statement->_if.condition);
                
                // Condition
                instruction = instruction_goto_if_false(condition, label_exit);
                array_push(generator->instructions, instruction);

                // Generate the body
                ir_generate_statement(generator, statement->_if.then);
                
                // Exit label
                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);

                free(label_exit);
            }
            break;
        }
        case STATEMENT_RETURN:
        {
            char* value = ir_generate_expression(generator, statement->_return.value);

            Instruction* instruction = instruction_return(value);
            array_push(generator->instructions, instruction);
            break;
        }
        case STATEMENT_BREAK:
        {
            Instruction* instruction = instruction_goto(generator->while_exit);
            array_push(generator->instructions, instruction);
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
                ":IR_GENERATOR - Unreachable: Unexpected statement in ir_generate_statement()");
            array_push(generator->diagnostics, _diagnostic);
            break;
        }
    }
}


void ir_generate_declaration(IR_Generator* generator, AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
        {
            // NOTE(timo): We don't do anything with global variable
            // declarations. They are put to .data section in assembly.
            if (generator->local != generator->global)
            {
                char* value = ir_generate_expression(generator, declaration->initializer);

                Instruction* instruction = instruction_copy(value, (char*)declaration->identifier->lexeme);
                array_push(generator->instructions, instruction);
            }
            break;
        }
        case DECLARATION_FUNCTION:
        {
            Instruction* instruction;

            char* label = (char*)declaration->identifier->lexeme;

            instruction = instruction_label(label);
            array_push(generator->instructions, instruction);
            
            // Set the scope to the function scope
            Symbol* function = scope_lookup(generator->local, declaration->identifier->lexeme);
            generator->local = function->type->function.scope;
            
            // Generate the body of the function (=initializer)
            // TODO(timo): Should this actually return the place where the
            // result of the function is saved?
            ir_generate_expression(generator, declaration->initializer);
            
            // Restore the scope to the enclosing scope
            generator->local = generator->local->enclosing;
            
            // TODO(timo): This should be removed when more flexible scoping is added
            assert(generator->local == generator->global);
            break;
        }
        default:
        {
            Diagnostic* _diagnostic = diagnostic(DIAGNOSTIC_ERROR, (Position){0},
                ":IR_GENERATOR - Unreachable: Unexpected declaration in ir_generate_declaration()");
            array_push(generator->diagnostics, _diagnostic);
            break;
        }
    }
}


void ir_generate(IR_Generator* generator, array* declarations)
{
    for (int i = 0; i < declarations->length; i++)
        ir_generate_declaration(generator, declarations->items[i]);
}
