#include "t.h"


bool type_is_boolean(Type* type)
{
    return type->kind == TYPE_BOOLEAN;
}


bool type_is_integer(Type* type)
{
    return type->kind == TYPE_INTEGER;
}
