#include "t.h"


Type* type_none()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_NONE;
    type->size = 0;

    return type;
}


Type* type_integer()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_INTEGER;
    type->size = 4;

    return type;
}


Type* type_boolean()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_BOOLEAN;
    type->size = 1;

    return type;
}


Type* type_function()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_FUNCTION;
    // TODO(timo): How to calculate the size?
    type->function.return_type = NULL;
    type->function.parameters = array_init(sizeof (Type*));
    type->function.arity = 0;

    return type;
}


// Type* type_array(Type* element_type, int length)
Type* type_array(Type* element_type)
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_ARRAY;
    // TODO(timo): Size can be calculated with the size of the element type * length
    type->array.element_type = element_type;
    type->array.length = 0;

    return type;
}


bool type_is_boolean(Type* type)
{
    return type->kind == TYPE_BOOLEAN;
}


bool type_is_integer(Type* type)
{
    return type->kind == TYPE_INTEGER;
}
