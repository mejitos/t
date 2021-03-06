// Implementations for AST related things.
//
// TODO(timo): The positions for all of the AST nodes should be set properly
// because now error messages are faulty when it comes to positions of some
// statements.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


AST_Declaration* function_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer)
{
    AST_Declaration* declaration = xmalloc(sizeof (AST_Declaration));
    declaration->kind = DECLARATION_FUNCTION;
    declaration->position = (Position) { .line_start = identifier->position.line_start,
                                         .column_start = identifier->position.column_start,
                                         .line_end = initializer->position.line_end,
                                         .column_end = initializer->position.column_end };
    declaration->identifier = identifier;
    declaration->specifier = specifier;
    declaration->initializer = initializer;

    return declaration;
}


AST_Declaration* variable_declaration(Token* identifier, Type_Specifier specifier, AST_Expression* initializer)
{
    AST_Declaration* declaration = xmalloc(sizeof (AST_Declaration));
    declaration->kind = DECLARATION_VARIABLE;
    declaration->position = (Position) { .line_start = identifier->position.line_start,
                                         .column_start = identifier->position.column_start,
                                         .line_end = initializer->position.line_end,
                                         .column_end = initializer->position.column_end };
    declaration->identifier = identifier;
    declaration->specifier = specifier;
    declaration->initializer = initializer;

    return declaration;
}


AST_Statement* expression_statement(AST_Expression* expression)
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->kind = STATEMENT_EXPRESSION;
    statement->position = (Position){0}; // TODO(timo)
    statement->expression = expression;

    return statement;
}


AST_Statement* block_statement(array* statements, int statements_length)
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->kind = STATEMENT_BLOCK;
    statement->position = (Position){0}; // TODO(timo)
    statement->block.statements = statements;
    statement->block.statements_length = statements_length;

    return statement;
}


AST_Statement* if_statement(AST_Expression* condition, AST_Statement* then, AST_Statement* _else)
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->kind = STATEMENT_IF;
    statement->position = (Position){0}; // TODO(timo)
    statement->_if.condition = condition;
    statement->_if.then = then;
    statement->_if._else = _else;

    return statement;
}


AST_Statement* while_statement(AST_Expression* condition, AST_Statement* body)
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->kind = STATEMENT_WHILE;
    statement->position = (Position){0}; // TODO(timo)
    statement->_while.condition = condition;
    statement->_while.body = body;

    return statement;
}


AST_Statement* break_statement()
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->position = (Position){0}; // TODO(timo)
    statement->kind = STATEMENT_BREAK;

    return statement;
}


AST_Statement* continue_statement()
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->position = (Position){0}; // TODO(timo)
    statement->kind = STATEMENT_CONTINUE;

    return statement;
}


AST_Statement* return_statement(AST_Expression* value)
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->kind = STATEMENT_RETURN;
    statement->position = (Position){0}; // TODO(timo)
    statement->_return.value = value;

    return statement;
}


AST_Statement* declaration_statement(AST_Declaration* declaration)
{
    AST_Statement* statement = xmalloc(sizeof (AST_Statement));
    statement->kind = STATEMENT_DECLARATION;
    statement->position = (Position){0}; // TODO(timo)
    statement->declaration = declaration;

    return statement;
}


AST_Expression* literal_expression(Token* literal)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_LITERAL;
    expression->position = literal->position;
    expression->literal = literal;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* unary_expression(Token* _operator, AST_Expression* operand)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_UNARY;
    expression->position = (Position) { .line_start = _operator->position.line_start,
                                        .column_start = _operator->position.column_start,
                                        .line_end = operand->position.line_end,
                                        .column_end = operand->position.column_end };
    expression->unary._operator = _operator;
    expression->unary.operand = operand;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* binary_expression(AST_Expression* left, Token* _operator, AST_Expression* right)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_BINARY;
    expression->position = (Position) { .line_start = left->position.line_start,
                                        .column_start = left->position.column_start,
                                        .line_end = right->position.line_end,
                                        .column_end = right->position.column_end };
    expression->binary.left = left;
    expression->binary._operator = _operator;
    expression->binary.right = right;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* variable_expression(Token* identifier)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_VARIABLE;
    expression->position = identifier->position;
    expression->identifier = identifier;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* assignment_expression(AST_Expression* variable, AST_Expression* value)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_ASSIGNMENT;
    expression->position = (Position) { .line_start = variable->position.line_start,
                                        .column_start = variable->position.column_start,
                                        .line_end = value->position.line_end,
                                        .column_end = value->position.column_end };
    expression->assignment.variable = variable;
    expression->assignment.value = value;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* index_expression(AST_Expression* variable, AST_Expression* value)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_INDEX;
    expression->position = (Position) { .line_start = variable->position.line_start,
                                        .column_start = variable->position.column_start,
                                        .line_end = value->position.line_end,
                                        .column_end = value->position.column_end };
    expression->index.variable = variable;
    expression->index.value = value;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


Parameter* function_parameter(Token* identifier, Type_Specifier specifier)
{
    Parameter* parameter = xcalloc(1, sizeof (Parameter));
    // TODO(timo): This position doesn't take into account the type specifier part
    parameter->position = identifier->position;
    parameter->identifier = identifier;
    parameter->specifier = specifier;

    return parameter;
}


AST_Expression* function_expression(array* parameters, int arity, AST_Statement* body)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_FUNCTION;
    // TODO(timo): This position doesn't take into account the parameter list part
    expression->position = body->position;
    expression->function.parameters = parameters;
    expression->function.arity = arity;
    expression->function.body = body;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* call_expression(AST_Expression* variable, array* arguments)
{
    AST_Expression* expression = xcalloc(1, sizeof (AST_Expression));
    expression->kind = EXPRESSION_CALL;
    // TODO(timo): This position doesn't take into account the argument list part
    expression->position = variable->position;
    expression->call.variable = variable;
    expression->call.arguments = arguments;
    expression->value = (Value){ .type = VALUE_NONE };

    return expression;
}


AST_Expression* error_expression()
{
    AST_Expression* expression = xmalloc(sizeof (AST_Expression));
    expression->kind = EXPRESSION_NONE;
    // TODO(timo): Should this get at least a position?

    return expression;
}


const char* expression_to_string(const AST_Expression* expression)
{
    stringbuilder* sb = sb_init();

    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
        {
            sb_append(sb, expression->literal->lexeme);
            break;
        }
        case EXPRESSION_BINARY:
        {
            sb_append(sb, "(");

            const char* left = expression_to_string(expression->binary.left);
            sb_append(sb, left);
            free((char*)left);

            sb_append(sb, expression->binary._operator->lexeme);

            const char* right = expression_to_string(expression->binary.right);
            sb_append(sb, right);
            free((char*)right);

            sb_append(sb, ")");
            break;
        }
        case EXPRESSION_UNARY:
        {
            sb_append(sb, "(");
            sb_append(sb, expression->unary._operator->lexeme);

            const char* operand = expression_to_string(expression->unary.operand);
            sb_append(sb, operand);
            free((char*)operand);

            sb_append(sb, ")");
            break;
        }
        default:
        {
            printf("Error in expression_to_string()\n");
            exit(1);
        }
    }

    const char* result = str_copy(sb_to_string(sb));
    sb_free(sb);

    return result;
}


const char* expression_str(const Expression_Kind kind)
{
    switch (kind)
    {
        case EXPRESSION_NONE:       return "none";
        case EXPRESSION_LITERAL:    return "literal expression";
        case EXPRESSION_UNARY:      return "unary expression";
        case EXPRESSION_BINARY:     return "binary expression";
        case EXPRESSION_VARIABLE:   return "variable expression";
        case EXPRESSION_ASSIGNMENT: return "assignment expression";
        case EXPRESSION_INDEX:      return "index expression";
        case EXPRESSION_FUNCTION:   return "function expression";
        case EXPRESSION_CALL:       return "call expression";
        default:                    return "invalid expression";
    }
}


const char* statement_str(const Statement_Kind kind)
{
    switch (kind)
    {
        case STATEMENT_NONE:        return "none";
        case STATEMENT_EXPRESSION:  return "expression statement";
        case STATEMENT_DECLARATION: return "declaration statement";
        case STATEMENT_WHILE:       return "while statement";
        case STATEMENT_IF:          return "if statement";
        case STATEMENT_BLOCK:       return "block statement";
        case STATEMENT_RETURN:      return "return statement";
        case STATEMENT_BREAK:       return "break statement";
        default:                    return "invalid statement";
    }
}


const char* declaration_str(const Declaration_Kind kind)
{
    switch (kind)
    {
        case DECLARATION_NONE:      return "none";
        case DECLARATION_VARIABLE:  return "variable declaration";
        case DECLARATION_FUNCTION:  return "function declaration";
        default:                    return "invalid declaration";
    }
}


const char* type_specifier_str(const Type_Specifier specifier)
{
    switch (specifier)
    {
        case TYPE_SPECIFIER_NONE:       return "none";
        case TYPE_SPECIFIER_INT:        return "int";
        case TYPE_SPECIFIER_ARRAY_INT:  return "[int]";
        case TYPE_SPECIFIER_BOOL:       return "bool";
        default:                        return "invalid type specifier";
    }
}


void expression_free(AST_Expression* expression)
{
    switch (expression->kind)
    {
        case EXPRESSION_LITERAL:
            // NOTE(timo); Lexer will free the literal token 
            // and there is nothing else allocated
            break;
        case EXPRESSION_UNARY:
            expression_free(expression->unary.operand);
            break;
        case EXPRESSION_BINARY:
            expression_free(expression->binary.left);
            expression_free(expression->binary.right);
            break;
        case EXPRESSION_VARIABLE:
            // NOTE(timo); This can be just free'd since the expression
            // only has the identifier, and lexer will free the token
            break;
        case EXPRESSION_ASSIGNMENT:
            expression_free(expression->assignment.variable);
            expression_free(expression->assignment.value);
            break;
        case EXPRESSION_INDEX:
            expression_free(expression->index.variable);
            expression_free(expression->index.value);
            break;
        case EXPRESSION_FUNCTION:
            for (int i = 0; i < expression->function.parameters->length; i++)
            {
                // NOTE(timo): These are the unresolved parameters which 
                // have no types etc. so this free is enough
                free(expression->function.parameters->items[i]);
                expression->function.parameters->items[i] = NULL;
            }
            array_free(expression->function.parameters);
            statement_free(expression->function.body);
            break; 
        case EXPRESSION_CALL:
            expression_free(expression->call.variable);
            for (int i = 0; i < expression->call.arguments->length; i++)
            {
                AST_Expression* argument = expression->call.arguments->items[i];
                expression_free(argument);
                // NOTE(timo): The array item gets freed and assigned to NULL 
                // when the expression is freed so no need to do it in here
                expression->call.arguments->items[i] = NULL;
            }
            array_free(expression->call.arguments);
            break;
        case EXPRESSION_NONE:
            break;
    }

    free(expression);
    expression = NULL;
}


void statement_free(AST_Statement* statement)
{
    switch (statement->kind)
    {
        case STATEMENT_EXPRESSION:
            expression_free(statement->expression);
            break;
        case STATEMENT_BLOCK:
            for (int i = 0; i < statement->block.statements->length; i++)
            {
                statement_free((AST_Statement*)statement->block.statements->items[i]);
                statement->block.statements->items[i] = NULL;
            }

            array_free(statement->block.statements);
            break;
        case STATEMENT_IF:
            expression_free(statement->_if.condition);
            statement_free(statement->_if.then);
            if (statement->_if._else != NULL)
                statement_free(statement->_if._else);
            break;
        case STATEMENT_WHILE:
            expression_free(statement->_while.condition);
            statement_free(statement->_while.body);
            break;
        case STATEMENT_BREAK:
            break;
        case STATEMENT_RETURN:
            expression_free(statement->_return.value);
            break;
        case STATEMENT_DECLARATION:
            declaration_free(statement->declaration);
            break;
    }

    free(statement);
    statement = NULL;
}


void declaration_free(AST_Declaration* declaration)
{
    expression_free(declaration->initializer);
    free(declaration);
    declaration = NULL;
}
