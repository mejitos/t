#include "t.h"


Type* type_none()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_NONE;
    // TODO(timo): This should be something sensible
    type->size = 0;
    type->alignment = 8;

    return type;
}


Type* type_integer()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_INTEGER;
    // TODO(timo): This should be 4 bytes, but for simplicity everything is 8 bytes
    type->size = 8;
    type->alignment = 8;

    return type;
}


Type* type_boolean()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_BOOLEAN;
    // TODO(timo): This should be like 1 byte, but for simplicity everything is 8 bytes
    type->size = 8;
    type->alignment = 8;

    return type;
}


Type* type_function()
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_FUNCTION;
    type->function.return_type = NULL;
    type->function.parameters = array_init(sizeof (Symbol*));
    type->function.arity = 0;
    type->size = 8; // The size if basically the size of the pointer to the function
    type->alignment = 8;

    return type;
}


Type* type_array(Type* element_type)
{
    Type* type = xmalloc(sizeof (Type));
    type->kind = TYPE_ARRAY;
    type->array.element_type = element_type;
    type->array.length = 0;
    type->size = 8; // The size is the size of the base pointer
    type->alignment = 8;

    return type;
}


const bool type_is_none(const Type* type)
{
    return type->kind == TYPE_NONE;
}


const bool type_is_not_none(const Type* type)
{
    return type->kind != TYPE_NONE;
}


const bool type_is_boolean(const Type* type)
{
    return type->kind == TYPE_BOOLEAN;
}


const bool type_is_not_boolean(const Type* type)
{
    return type->kind != TYPE_BOOLEAN;
}


const bool type_is_integer(const Type* type)
{
    return type->kind == TYPE_INTEGER;
}


const bool type_is_not_integer(const Type* type)
{
    return type->kind != TYPE_INTEGER;
}


const bool type_is_array(const Type* type)
{
    return type->kind == TYPE_ARRAY;
}


const bool type_is_not_array(const Type* type)
{
    return type->kind != TYPE_ARRAY;
}


const bool type_is_function(const Type* type)
{
    return type->kind == TYPE_FUNCTION;
}


const bool type_is_not_function(const Type* type)
{
    return type->kind != TYPE_FUNCTION;
}


const bool types_equal(const Type* type1, const Type* type2)
{
    return type1->kind == type2->kind;
}


const bool types_not_equal(const Type* type1, const Type* type2)
{
    return type1->kind != type2->kind;
}


void type_free(Type* type)
{
    // NOTE(timo): There is a lot of situations where the same types can be found from
    // expressions and from symbols. Therefore we need to make a NULL check here.
    if (type == NULL) return;

    switch (type->kind)
    {
        case TYPE_NONE:
        case TYPE_INTEGER:
        case TYPE_BOOLEAN:
            break;
        case TYPE_FUNCTION:
            if (type->function.scope) scope_free(type->function.scope);
            // NOTE(timo): Return types are primitive types at this point so there 
            // is no need to explicitly remove them as they point to type table
            // NOTE(timo): The parameter list is a list of pointers to symbols
            // which are being freed with the scope.
            array_free(type->function.parameters);
            break;
        case TYPE_ARRAY:
            // NOTE(timo): Element types are primitive types at this point so there 
            // is no need to explicitly remove them as they point to type table
            break;
        default:
            // TODO(timo): Error
            break;
    }
    
    free(type);
    type = NULL;
}


const char* type_as_string(const Type_Kind kind)
{
    switch (kind)
    {
        case TYPE_NONE:
            return "none";
        case TYPE_INTEGER:
            return "int";
        case TYPE_BOOLEAN:
            return "bool";
        case TYPE_FUNCTION:
            return "function";
        case TYPE_ARRAY:
            return "array";
        default:
            return "unknown type";
    }
}


hashtable* type_table_init()
{
    hashtable* type_table = hashtable_init(3);
    hashtable_put(type_table, "none", type_none());
    hashtable_put(type_table, "int", type_integer());
    hashtable_put(type_table, "bool", type_boolean());

    return type_table;
}


void type_table_free(hashtable* table)
{
    for (int i = 0; i < table->capacity; i++)
    {
        Type* type = table->entries[i].value;

        if (type)
        {
            type_free(type);
            type = NULL;
        }
    }

    hashtable_free(table);
    table = NULL;
}
