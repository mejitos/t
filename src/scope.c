#include "t.h"


Scope* scope_init(Scope* enclosing)
{
    Scope* scope = xcalloc(1, sizeof (Scope));
    scope->enclosing = enclosing;
    scope->symbols = hashtable_init(10);

    return scope;
}


void scope_free(Scope* scope)
{
    for (int i = 0; i < scope->symbols->capacity; i++)
    {
        Symbol* symbol = scope->symbols->entries[i].value;
        if (symbol) symbol_free(symbol);
    }

    hashtable_free(scope->symbols);

    if (scope->enclosing) scope_free(scope->enclosing);

    free(scope);
    scope = NULL;
}


/*
void scope_enter(Scope* enclosing)
{
    //
}


void scope_leave()
{
    //
}
*/


static Symbol* scope_get(Scope* scope, const char* identifier)
{
    return hashtable_get(scope->symbols, identifier);

    /*
    for (int i = 0; i < scope->symbols->length; i++)
    {
        Symbol* it = scope->symbols->items[i];
        if (strcmp(it->identifier, identifier) == 0)
            return it;
    }
    
    return NULL;
    */
}


static void scope_put(Scope* scope, Symbol* symbol)
{
    // TODO(timo): Should I check here if the symbol is already declared?
    // Or do I do it somewhere else before putting the symbol in the table?
    // array_push(scope->symbols, symbol);
    hashtable_put(scope->symbols, symbol->identifier, symbol);
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
