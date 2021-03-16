#include "t.h"


void instruction_free(Instruction* instruction)
{
    // TODO(timo): I should somehow free the allocated memory for the temps and labels
    free(instruction);
    instruction = NULL;
}


void ir_generator_init(IR_Generator* generator, Scope* global)
{
    *generator = (IR_Generator) { .temp = 0,
                                  .label = 0,
                                  .global = global,
                                  .instructions = array_init(sizeof (Instruction*)) };

    generator->local = generator->global;
}


void ir_generator_free(IR_Generator* generator)
{
    for (int i = 0; i < generator->instructions->length; i++)
        instruction_free(generator->instructions->items[i]);

    array_free(generator->instructions);

    // NOTE(timo): Generator itself is not being freed since it is
    // initialized in the stack in top level function
}


static char* temp(IR_Generator* generator)
{
    char* t = malloc(sizeof (char) * 12 + 1);
    snprintf(t, 13, "_t%d", generator->temp++);

    return t;
}


static char* label(IR_Generator* generator)
{
    char* l = malloc(sizeof (char) * 12 + 1);
    snprintf(l, 13, "_l%d", generator->label++);

    return l;
}


char* ir_generate_expression(IR_Generator* generator, AST_Expression* expression)
{
    // TODO(timo): This should probably return the instruction structures
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
        {
            // NOTE(timo): The situation probably calls for generating the temp variables
            // for all the literals/constants etc. and they just get optimized away later
            // TODO(timo): I think we could just return the string and we can skip the
            // unnecessary extra assign/store/move step
            char* result = temp(generator);
            char* arg = (char*)expression->literal->lexeme;

            scope_declare(generator->local, symbol_temp(result, expression->type));

            printf("\t%s := %s\n", result, arg);

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);
            
            return result;

            // Generate nothing, just return the literal
            // return (char*)expression->literal->lexeme;
        }
        case EXPRESSION_UNARY:
        {
            char* operand = ir_generate_expression(generator, expression->unary.operand);
            char* operator = (char*)expression->unary._operator->lexeme;
            char* result = temp(generator);

            scope_declare(generator->local, symbol_temp(result, expression->type));

            printf("\t%s := %s %s\n", result, operator, operand);

            Instruction* instruction;

            switch (expression->unary._operator->kind)
            {
                case TOKEN_MINUS:
                    instruction = instruction_minus(operand, result);
                    break;
                case TOKEN_NOT:
                    instruction = instruction_neg(operand, result);
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }

            array_push(generator->instructions, instruction);

            return result;
        
        }
        case EXPRESSION_BINARY:
        {
            char* left = ir_generate_expression(generator, expression->binary.left); 
            char* right = ir_generate_expression(generator, expression->binary.right);
            char* operator = (char*)expression->binary._operator->lexeme;
            // NOTE(timo): When we move this into here, we get the correct ordering of the temp labels
            char* result = temp(generator);

            scope_declare(generator->local, symbol_temp(result, expression->type));

            printf("\t%s := %s %s %s\n", result, left, operator, right);
            
            Instruction* instruction;

            switch (expression->binary._operator->kind)
            {
                case TOKEN_PLUS:
                    instruction = instruction_add(left, right, result);
                    break;
                case TOKEN_MINUS:
                    instruction = instruction_sub(left, right, result);
                    break;
                case TOKEN_MULTIPLY:
                    instruction = instruction_mul(left, right, result);
                    break;
                case TOKEN_DIVIDE:
                    instruction = instruction_div(left, right, result);
                    break;
                case TOKEN_IS_EQUAL:
                    instruction = instruction_eq(left, right, result);
                    break;
                case TOKEN_NOT_EQUAL:
                    instruction = instruction_neq(left, right, result);
                    break;
                case TOKEN_LESS_THAN:
                    instruction = instruction_lt(left, right, result);
                    break;
                case TOKEN_LESS_THAN_EQUAL:
                    instruction = instruction_lte(left, right, result);
                    break;
                case TOKEN_GREATER_THAN:
                    instruction = instruction_gt(left, right, result);
                    break;
                case TOKEN_GREATER_THAN_EQUAL:
                    instruction = instruction_gte(left, right, result);
                    break;
                default:
                    // TODO(timo): Error
                    break;
            }
            
            array_push(generator->instructions, instruction);

            return result;
        }
        case EXPRESSION_VARIABLE:
        {
            // TODO(timo): I think we could just return the string and we can skip the
            // unnecessary extra assign/store/move step
            char* result = temp(generator);
            char* arg = (char*)expression->literal->lexeme;

            scope_declare(generator->local, symbol_temp(result, expression->type));

            printf("\t%s := %s\n", result, arg);

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);

            return result;

            // Generate nothing, just return the variable
            return (char*)expression->identifier->lexeme;
        }
        case EXPRESSION_ASSIGNMENT:
        {
            // This is pretty much same as the copy instruction since we just access
            // the variable and save it to the target variable or send it to the target
            //
            // Assignment happens pretty much only to the named variable so there is no
            // need for temp variables
            char* result = (char*)expression->assignment.variable->identifier->lexeme;
            char* arg = ir_generate_expression(generator, expression->assignment.value);

            printf("\t%s := %s\n", result, arg);

            Instruction* instruction = instruction_copy(arg, result);
            array_push(generator->instructions, instruction);

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

            printf("\tfunction_begin N\n");

            // TODO(timo): Change the scope to function scope
            
            instruction = instruction_function_begin();
            array_push(generator->instructions, instruction);

            // TODO(timo): Params?!?

            ir_generate_statement(generator, expression->function.body);
            
            // TODO(timo): Leave the function scope

            // NOTE(timo): At this point, the scope has been already set to the function scope
            // so therefore we already know the size of the function via the scope
            // We could also take this info at the code generation stage from the scope
            instruction->value.integer = generator->local->offset;

            printf("\tfunction_end\n");

            instruction = instruction_function_end();
            array_push(generator->instructions, instruction);
            break;
        }
        case EXPRESSION_CALL:
        {
            Instruction* instruction;

            // Push the params to the stack/registers
            array* arguments = expression->call.arguments;

            for (int i = arguments->length - 1; i >= 0; i--)
            {
                AST_Expression* argument = (AST_Expression*)arguments->items[i];
                char* arg = ir_generate_expression(generator, argument);

                printf("\tparameter_push %s\n", arg);

                instruction = instruction_param_push(arg);
                array_push(generator->instructions, instruction);
            }

            // Call instruction
            char* arg = ir_generate_expression(generator, expression->call.variable);
            char* result = temp(generator);

            scope_declare(generator->local, symbol_temp(result, expression->type));

            // NOTE(timo): According to the Dragon Book the number of arguments is
            // important info to pass on in case of nested function calls
            printf("\t%s := call %s, %d\n", result, arg, arguments->length);

            instruction = instruction_call(arg, result, arguments->length);
            array_push(generator->instructions, instruction);
            
            // Pop the params from the stack
            for (int i = 0; i < arguments->length; i++)
            {
                AST_Expression* argument = (AST_Expression*)arguments->items[i];
                char* arg = ir_generate_expression(generator, argument);

                printf("\tparameter_pop %s\n", arg);

                instruction = instruction_param_pop(arg);
                array_push(generator->instructions, instruction);
            }

            return result;
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

            printf("%s:\n", label_condition);

            instruction = instruction_label(label_condition);
            array_push(generator->instructions, instruction);

            char* condition = ir_generate_expression(generator, statement->_while.condition);

            // if false, jump to label 2
            printf("\tif_false %s goto %s\n", condition, label_exit);

            instruction = instruction_goto_if_false(condition, label_exit);
            array_push(generator->instructions, instruction);
            
            // print the body
            ir_generate_statement(generator, statement->_while.body);
            
            // go back to the start of the loop to test the condition again
            printf("\tgoto %s\n", label_condition);

            instruction = instruction_goto(label_condition);
            array_push(generator->instructions, instruction);
            
            // exit
            printf("%s:\n", label_exit);

            instruction = instruction_label(label_exit);
            array_push(generator->instructions, instruction);
            
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
                printf("\tif_false %s goto %s\n", condition, label_else);

                instruction = instruction_goto_if_false(condition, label_else);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if.then);
                
                //
                printf("\tgoto %s\n", label_exit);

                instruction = instruction_goto(label_exit);
                array_push(generator->instructions, instruction);
                
                //
                printf("%s:\n", label_else);
                instruction = instruction_label(label_else);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if._else);
                
                //
                printf("%s:\n", label_exit);

                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);
            }
            // if-then
            else
            {
                Instruction* instruction;

                // Local labels
                char* label_exit = label(generator);
                char* condition = ir_generate_expression(generator, statement->_if.condition);
                
                //
                printf("\tif_false %s goto %s\n", condition, label_exit);

                instruction = instruction_goto_if_false(condition, label_exit);
                array_push(generator->instructions, instruction);

                ir_generate_statement(generator, statement->_if.then);
                
                // 
                printf("%s:\n", label_exit);

                instruction = instruction_label(label_exit);
                array_push(generator->instructions, instruction);
            }
            break;
        }
        case STATEMENT_RETURN:
        {
            char* value = ir_generate_expression(generator, statement->_return.value);
            printf("\treturn %s\n", value);

            Instruction* instruction = instruction_return(value);
            array_push(generator->instructions, instruction);
            break;
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
            // TODO(timo): How should I handle the global variable declarations?
            // Or do they just live in the symbol table and we can use them freely
            // from the functions when necessary and therefore access them when necessary.
            // TODO(timo): Or are these actually loaded into the stack at the 
            // beginning of the program
            
            char* value = ir_generate_expression(generator, declaration->initializer);

            Instruction* instruction;

            instruction = instruction_copy(value, (char*)declaration->identifier->lexeme);
            array_push(generator->instructions, instruction);

            break;
        }
        case DECLARATION_FUNCTION:
        {
            Instruction* instruction;
            // TODO(timo): This is probably the point where we need the abstraction for routines
            char* label = (char*)declaration->identifier->lexeme;
            printf("_%s:\n", label);

            instruction = instruction_label(label);
            array_push(generator->instructions, instruction);
            
            /*
            printf("\tfunction_begin %d\n", N);

            instruction = instruction_function_begin();
            array_push(generator->instructions, instruction);
            */

            generator->local = (scope_lookup(generator->local, declaration->identifier->lexeme))->type->function.scope;
            
            ir_generate_expression(generator, declaration->initializer);

            // NOTE(timo): These two are basically the same thing in our case
            // generator->local = generator->local->enclosing;
            generator->local = generator->global;
            
            /*
            printf("\tfunction_end\n");

            instruction = instruction_function_end();
            array_push(generator->instructions, instruction);
            */
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
