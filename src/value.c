#include "t.h"


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


bool as_bool(Value value)
{
    assert(value.type == VALUE_BOOLEAN);

    return value.boolean_value;
}


bool as_int(Value value)
{
    assert(value.type == VALUE_INTEGER);

    return value.integer_value;
}
