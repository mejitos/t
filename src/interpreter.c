#include "t.h"


typedef enum Value_Type
{
    VALUE_NONE,
    VALUE_INTEGER,
    VALUE_BOOLEAN,
} Value_Type;


typedef struct Value
{
    Value_Type type;
    union {
        int integer_value;
        bool boolean_value;
    };
} Value;




Value evaluate_expression(AST_Expression* expression)
{
    // 
}


void evaluate_statement(AST_Statement* statement)
{
    //
}


void evaluate_declaration(AST_Declaration* declaration)
{
    //
}


void interpret(array* declarations)
{
    for (int i = 0; i < declarations->length; i++)
        evaluate_declaration(declarations->items[i]);
}
