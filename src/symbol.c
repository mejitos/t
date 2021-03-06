// Implementations for the factory functiosn for creating symbols and freeing
// the memory allocated for them.
//
// Author: Timo Mehto
// Date: 2021/05/12

#include "t.h"


Symbol* symbol_variable(Scope* scope, const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->kind = SYMBOL_VARIABLE;
    symbol->scope = scope;
    symbol->identifier = str_copy(identifier);
    symbol->type = type;
    symbol->_register = -1;
        
    return symbol;
}


Symbol* symbol_function(Scope* scope, const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->kind = SYMBOL_FUNCTION;
    symbol->scope = scope;
    symbol->identifier = str_copy(identifier);
    symbol->type = type;
    symbol->_register = -1;

    return symbol;
}


Symbol* symbol_parameter(Scope* scope, const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->kind = SYMBOL_PARAMETER;
    symbol->scope = scope;
    symbol->identifier = str_copy(identifier);
    symbol->type = type;
    symbol->_register = -1;

    return symbol;
}


Symbol* symbol_temp(Scope* scope, const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->kind = SYMBOL_TEMP;
    symbol->scope = scope;
    symbol->identifier = str_copy(identifier);
    symbol->type = type;
    symbol->_register = -1;
        
    return symbol;
}


void symbol_free(Symbol* symbol)
{
    // NOTE(timo): There might not be type if program stops before types are added
    if (symbol->type != NULL && symbol->kind != SYMBOL_TEMP)
    {
        switch (symbol->type->kind)
        {
            case TYPE_NONE:
            case TYPE_INTEGER:
            case TYPE_BOOLEAN:
                break;
            case TYPE_ARRAY:
            case TYPE_FUNCTION:
                type_free(symbol->type);
                symbol->type = NULL;
                break;
            default:
                // TODO(timo): Error
                break;
        }
    }

    // NOTE(timo): The scope is being freed by the resolver
    free((char*)symbol->identifier);
    symbol->identifier = NULL;

    free(symbol);
    symbol = NULL;
}
