#include "t.h"


void ir_generator_init(IR_Generator* generator, Scope* global)
{
    *generator = (IR_Generator) { .temp = 0,
                                  .label = 0,
                                  .global = global,
                                  .not_in_loop = true,
                                  .instructions = array_init(sizeof (Instruction*)) };

    generator->local = generator->global;
}


void ir_generator_free(IR_Generator* generator)
{
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
            scope_declare(generator->local, symbol_temp(instruction->result, expression->type));
            free(temp);

            // printf("\t%s := %s\n", instruction->result, arg);

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
                    instruction = instruction_neg(operand, temp);
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(instruction->result, expression->type));
            free(temp);

            // printf("\t%s := %s %s\n", instruction->result, operator, operand);

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
                default:
                    // TODO(timo): Error
                    break;
            }
            
            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(instruction->result, expression->type));
            free(temp);

            // printf("\t%s := %s %s %s\n", result, left, operator, right);

            return instruction->result;
        }
        case EXPRESSION_VARIABLE:
        {
            char* arg = (char*)expression->literal->lexeme;
            char* temp = temp_label(generator);

            Instruction* instruction = instruction_copy(arg, temp);

            array_push(generator->instructions, instruction);
            scope_declare(generator->local, symbol_temp(instruction->result, expression->type));
            free(temp);

            // printf("\t%s := %s\n", result, arg);

            return instruction->result;
        }
        case EXPRESSION_ASSIGNMENT:
        {
            char* arg = ir_generate_expression(generator, expression->assignment.value);
            char* result = (char*)expression->assignment.variable->identifier->lexeme;

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);

            //printf("\t%s := %s\n", result, arg);

            return result;
        }
        case EXPRESSION_INDEX:
        {
            break;
        }
        case EXPRESSION_FUNCTION:
        {
            Instruction* instruction;
            // TODO(timo): Does the function declaration handle the beginnning and ending?
            // It probably is job of the expression, if we think about a little into the
            // future and to anonymous functions
            
            // -----===== The function prologue =====-----

            // 
            //printf("\tfunction_begin N\n");

            // TODO(timo): Change the scope to function scope
            
            // What functions begins?
            instruction = instruction_function_begin();
            array_push(generator->instructions, instruction);

            // TODO(timo): Params?!?

            ir_generate_statement(generator, expression->function.body);
            
            // TODO(timo): Leave the function scope

            // NOTE(timo): At this point, the scope has been already set to the function scope
            // so therefore we already know the size of the function via the scope
            // We could also take this info at the code generation stage from the scope
            instruction->size = generator->local->offset;
            
            // -----===== The function epilogue =====-----
            //printf("\tfunction_end\n");
            
            // What functions ends?
            instruction = instruction_function_end();
            array_push(generator->instructions, instruction);
            break;
        }
        case EXPRESSION_CALL:
        {
            Instruction* instruction;

            // Push the params to the stack/registers
            array* arguments = expression->call.arguments;
            char* args[arguments->length];

            // TODO(timo): Could we just straight up push the parameters to the
            // stack without using temporary variables as middle man? Like we 
            // can when we pop the params.
            for (int i = arguments->length - 1; i >= 0; i--)
            {
                AST_Expression* argument = (AST_Expression*)arguments->items[i];
                char* arg = ir_generate_expression(generator, argument);

                instruction = instruction_param_push(arg);
                array_push(generator->instructions, instruction);
                args[i] = arg;

                //printf("\tparameter_push %s\n", arg);
            }

            // Call instruction itself
            char* arg = (char*)expression->call.variable->identifier->lexeme;
            char* temp = temp_label(generator);
            // NOTE(timo): According to the Dragon Book the number of arguments is
            // important info to pass on in case of nested function calls
            instruction = instruction_call(arg, temp, arguments->length);

            scope_declare(generator->local, symbol_temp(instruction->result, expression->type));
            array_push(generator->instructions, instruction);
            free(temp);

            //printf("\t%s := call %s, %d\n", instruction->result, arg, arguments->length);
            
            // Pop the params from the stack after the call has returned
            for (int i = 0; i < arguments->length; i++)
            {
                Instruction* instruction = instruction_param_pop(args[i]);
                array_push(generator->instructions, instruction);

                //printf("\tparameter_pop %s\n", args[i]);
            }

            return instruction->result;
        }
        default:
            // TODO(timo): Error
            break;
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
            // "label" + L1 + condition_code + "label" + L2 + statement_code_body

            // Local labels
            char* label_condition = label(generator); // condition
            char* label_exit = label(generator); // exit

            // TODO(timo): Quick hacky hack solution for break statements. There might be a better way.
            generator->not_in_loop = false;
            generator->while_exit = label_exit;

            //printf("%s:\n", label_condition);

            instruction = instruction_label(label_condition);
            array_push(generator->instructions, instruction);

            char* condition = ir_generate_expression(generator, statement->_while.condition);

            // if false, jump to label 2
            //printf("\tif_false %s goto %s\n", condition, label_exit);

            instruction = instruction_goto_if_false(condition, label_exit);
            array_push(generator->instructions, instruction);
            
            // print the body
            ir_generate_statement(generator, statement->_while.body);
            
            // go back to the start of the loop to test the condition again
            //printf("\tgoto %s\n", label_condition);

            instruction = instruction_goto(label_condition);
            array_push(generator->instructions, instruction);
            
            // exit
            //printf("%s:\n", label_exit);

            instruction = instruction_label(label_exit);
            array_push(generator->instructions, instruction);
            
            // TODO(timo): Quick hacky hack solution for break statements. There might be a better way.
            generator->not_in_loop = true;
            generator->while_exit = NULL;

            free(label_condition);
            free(label_exit);
            break;
        }
        case STATEMENT_IF:
        {
            // condition_code + then + label + else  + label
            
            // if-then-else
            if (statement->_if._else != NULL)
            {
                Instruction* instruction;
                // Local labels
                char* label_else = label(generator);
                char* label_exit = label(generator);
                char* condition = ir_generate_expression(generator, statement->_if.condition);
                
                //
                //printf("\tif_false %s goto %s\n", condition, label_else);

                instruction = instruction_goto_if_false(condition, label_else);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if.then);
                
                //
                //printf("\tgoto %s\n", label_exit);

                instruction = instruction_goto(label_exit);
                array_push(generator->instructions, instruction);
                
                //
                //printf("%s:\n", label_else);
                instruction = instruction_label(label_else);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if._else);
                
                //
                //printf("%s:\n", label_exit);

                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);

                free(label_else);
                free(label_exit);
            }
            // if-then
            else
            {
                Instruction* instruction;

                // Local labels
                char* label_exit = label(generator);
                char* condition = ir_generate_expression(generator, statement->_if.condition);
                
                //
                //printf("\tif_false %s goto %s\n", condition, label_exit);

                instruction = instruction_goto_if_false(condition, label_exit);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if.then);
                
                // 
                //printf("%s:\n", label_exit);

                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);

                free(label_exit);
            }
            break;
        }
        case STATEMENT_RETURN:
        {
            char* value = ir_generate_expression(generator, statement->_return.value);
            //printf("\treturn %s\n", value);

            Instruction* instruction = instruction_return(value);
            array_push(generator->instructions, instruction);
            break;
        }
        case STATEMENT_BREAK:
        {
            // TODO(timo): Is this actually needed? Since the invalid loop
            // context should be resolved already
            if (generator->not_in_loop)
            {
                // TODO(timo): Add diagnostic
                printf("[IR_GENERATOR] - Error: Trying to create break instruction when not in loop\n");
                exit(1);
            }
            else
            {
                Instruction* instruction = instruction_goto(generator->while_exit);
                array_push(generator->instructions, instruction);
            }
        }
        default:
            // TODO(timo): Error
            break;
    }
}


void ir_generate_declaration(IR_Generator* generator, AST_Declaration* declaration)
{
    switch (declaration->kind)
    {
        case DECLARATION_VARIABLE:
        {
            char* value = ir_generate_expression(generator, declaration->initializer);

            Instruction* instruction;

            instruction = instruction_copy(value, (char*)declaration->identifier->lexeme);
            array_push(generator->instructions, instruction);
            break;
        }
        case DECLARATION_FUNCTION:
        {
            Instruction* instruction;

            char* label = (char*)declaration->identifier->lexeme;
            //printf("_%s:\n", label);

            instruction = instruction_label(label);
            array_push(generator->instructions, instruction);
            
            // Set the scope to the function scope
            Symbol* function = scope_lookup(generator->local, declaration->identifier->lexeme);
            generator->local = function->type->function.scope;
            
            // Generate the body of the function (=initializer)
            ir_generate_expression(generator, declaration->initializer);
            
            // Restore the scope to the enclosing scope
            // NOTE(timo): These two are basically the same thing in our case
            // generator->local = generator->local->enclosing;
            generator->local = generator->global;
            break;
        }
        default:
            // TODO(timo): Error
            break;
    }
}


void ir_generate(IR_Generator* generator, array* declarations)
{
    for (int i = 0; i < declarations->length; i++)
        ir_generate_declaration(generator, declarations->items[i]);
}
