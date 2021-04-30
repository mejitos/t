#include "t.h"


const bool value_is_none(const Value value)
{
    return value.type == VALUE_NONE;
}


const bool value_is_not_none(const Value value)
{
    return value.type != VALUE_NONE;
}


const bool value_is_boolean(const Value value)
{
    return value.type == VALUE_BOOLEAN;
}


const bool value_is_integer(const Value value)
{
    return value.type == VALUE_INTEGER;
}


const bool value_is_not_boolean(const Value value)
{
    return value.type != VALUE_BOOLEAN;
}


const bool value_is_not_integer(const Value value)
{
    return value.type != VALUE_INTEGER;
}


const char* value_str(const Value_Type type)
{
    switch (type)
    {
        case VALUE_NONE:    return "none";
        case VALUE_INTEGER: return "integer";
        case VALUE_BOOLEAN: return "boolean";
        default:            return "invalid value type";
    }
}
