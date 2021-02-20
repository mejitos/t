#include "t.h"


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


bool type_is_boolean(Type* type)
{
    return type->kind == TYPE_BOOLEAN;
}


bool type_is_integer(Type* type)
{
    return type->kind == TYPE_INTEGER;
}
