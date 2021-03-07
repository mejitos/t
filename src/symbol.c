#include "t.h"


Symbol* symbol_variable(const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->state = STATE_UNRESOLVED;
    symbol->kind = SYMBOL_VARIABLE;
    symbol->identifier = identifier;
    symbol->type = type;
        
    return symbol;
}


Symbol* symbol_function(const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->state = STATE_UNRESOLVED;
    symbol->kind = SYMBOL_FUNCTION;
    symbol->identifier = identifier;
    symbol->type = type;

    return symbol;
}


Symbol* symbol_parameter(const char* identifier, Type* type)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->state = STATE_RESOLVED;
    symbol->kind = SYMBOL_PARAMETER;
    symbol->identifier = identifier;
    symbol->type = type;

    return symbol;
}


void symbol_free(Symbol* symbol)
{
    // NOTE(timo): There might not be type if program stops before types are added
    if (symbol->type != NULL)
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
                break;
            default:
                // TODO(timo): Error
                break;
        }
    }

    free(symbol);
    symbol = NULL;
}
