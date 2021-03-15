#include "t.h"


Scope* scope_init(Scope* enclosing, const char* name)
{
    Scope* scope = xmalloc(sizeof (Scope));
    scope->name = name;
    scope->offset = 0;
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

    // if (scope->enclosing) scope_free(scope->enclosing);

    free(scope);
    scope = NULL;
}


static Symbol* scope_get(Scope* scope, const char* identifier)
{
    return hashtable_get(scope->symbols, identifier);
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
        return scope_lookup(scope->enclosing, identifier);

    return NULL;
}


void scope_declare(Scope* scope, Symbol* symbol)
{
    assert(scope != NULL);
    Symbol* temp = scope_get(scope, symbol->identifier);
    
    // TODO(timo): This is not really scopes problem to handle to be honest. Resolver
    // should handle this situation on its own
    if (temp != NULL)
    {
        // TODO(timo): Error
        // TODO(timo): Add the location of the declared variable into the message
        printf("Redeclaration of '%s'\n", symbol->identifier);
        exit(1);
    }

    // Setting the memory offsets / alignment
    symbol->offset = scope->offset;

    // Compute the new offset for the scope
    scope->offset += symbol->type->size;

    // Lets just use the alignment of 8 bytes for everything for now to make life easier
    while (scope->offset % 8 != 0)
        scope->offset += 4;

    scope_put(scope, symbol);
}


void dump_scope(Scope* scope, int indentation)
{
    for (int i = 0; i < indentation; i++)
        printf("\t");

    printf("Scope: %s\t\tOffset: %d\n", scope->name, scope->offset);

    for (int i = 0; i < indentation; i++)
        printf("\t");

    printf("---\n");

    for (int i = 0; i < scope->symbols->capacity; i++)
    {
        hashtable_entry entry = scope->symbols->entries[i];

        if (entry.key)
        {
            Symbol* symbol = entry.value;

            for (int j = 0; j < indentation; j++)
                printf("\t");

            switch (symbol->kind)
            {
                case SYMBOL_FUNCTION:
                    // printf("function\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->type->offset);
                    printf("function\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->offset);
                    dump_scope(symbol->type->function.scope, indentation + 1);
                    break;
                case SYMBOL_VARIABLE:
                    // printf("variable\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->type->offset);
                    printf("variable\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->offset);
                    break;
                case SYMBOL_PARAMETER:
                    // printf("parameter\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->type->offset);
                    printf("parameter\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->offset);
                    break;
                case SYMBOL_TEMP:
                    // printf("temporary\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->type->offset);
                    printf("temporary\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type), symbol->type->size, symbol->offset);
                    break;
                default:
                    break;
            }
        }
    }
}
