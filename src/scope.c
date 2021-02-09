#include "t.h"


Scope* scope_init(Scope* enclosing)
{
    Scope* scope = xcalloc(1, sizeof (Scope));
    scope->enclosing = enclosing;
    scope->symbols = array_init(sizeof(Symbol*));

    return scope;
}


Symbol* scope_get(Scope* scope, const char* identifier)
{
    for (int i = 0; i < scope->symbols->length; i++)
    {
        Symbol* it = scope->symbols->items[i];
        if (strcmp(it->identifier, identifier) == 0)
            return it;
    }
    
    return NULL;
}


void scope_put(Scope* scope, Symbol* symbol)
{
    // TODO(timo): Should I check here if the symbol is already declared?
    // Or do I do it somewhere else before putting the symbol in the table?
    array_push(scope->symbols, symbol);
}


Symbol* scope_lookup(Scope* scope, const char* identifier)
{
    Symbol* symbol = scope_get(scope, identifier);

    if (symbol != NULL) return symbol;

    if (scope->enclosing != NULL)
        return scope_lookup(scope, identifier);

    return NULL;
}


void scope_declare(Scope* scope, Symbol* symbol)
{
    assert(scope != NULL);
    Symbol* temp = scope_get(scope, symbol->identifier);
    
    if (temp != NULL)
    {
        // TODO(timo): Error
        // TODO(timo): Add the location of the declared variable into the message
        printf("Redeclaration of '%s'\n", symbol->identifier);
        exit(1);
    }

    scope_put(scope, symbol);
}