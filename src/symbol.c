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
    // TODO(timo): Should the identifier be freed?
    // TODO(timo): Freeing the type?

    free(symbol);
    symbol = NULL;
}
