#include "t.h"


/*
bool is_type(Value value, Value_Type type)
{
    return value.type == type;
}


bool is_bool(Value value)
{
    return value.type == VALUE_BOOLEAN;
}


bool is_int(Value value)
{
    return value.type == VALUE_INTEGER;
}
*/


bool value_as_boolean(Value value)
{
    return value.boolean;
}


bool value_as_integer(Value value)
{
    return value.integer;
}
