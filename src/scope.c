#include "t.h"


Scope* scope_init(Scope* enclosing, const char* name)
{
    Scope* scope = xmalloc(sizeof (Scope));
    scope->name = name;
    scope->offset = 0;
    scope->offset_parameter = 16;
    scope->enclosing = enclosing;
    scope->symbols = hashtable_init(10);

    return scope;
}


void scope_free(Scope* scope)
{
    for (int i = 0; i < scope->symbols->capacity; i++)
    {
        Symbol* symbol = scope->symbols->entries[i].value;
        if (symbol != NULL) symbol_free(symbol);
    }

    hashtable_free(scope->symbols);

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
    // TODO(timo): I'm not sure though this is job of the scope to set these offsets
    // NOTE(timo): This has to be offset + 8 because of how the stack works
    // We cannot have position stack_top - 0, since then the value would go
    // outside of the stack.
    // TODO(timo): Get rid of this and do something more useful
    // At least separate the parameters and local variables from each other
    // TODO(timo): We would need a separate counter for parameter offsets
    if (symbol->kind == SYMBOL_PARAMETER)
    {
        if (strcmp(scope->name, "main") == 0)
            symbol->offset = scope->offset_parameter - 8;
        else
            symbol->offset = scope->offset_parameter;
        
        scope->offset_parameter += symbol->type->size;

        // TODO(timo): We should probably compute the correct alignment
        // for these parameters too
    }
    else
    {
        symbol->offset = scope->offset + 8;

        // Compute the new offset for the scope
        scope->offset += symbol->type->size;
        // scope->offset += symbol->type->alignment;

        // Lets just use the alignment of 8 bytes for everything for now to make life easier
        while (scope->offset % symbol->type->alignment != 0)
            scope->offset += 1;
    }

    scope_put(scope, symbol);
}


bool scope_contains(Scope* scope, const char* identifier)
{
    Symbol* symbol = scope_get(scope, identifier);

    return symbol != NULL;
}


array* scope_symbols(Scope* scope)
{
    array* symbols = array_init(sizeof (Symbol*));

    for (int i = 0; i < scope->symbols->capacity; i++)
    {
        Symbol* symbol = scope->symbols->entries[i].value;

        if (symbol) array_push(symbols, symbol);
    }

    return symbols;
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
                    printf("function\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type->kind), symbol->type->size, symbol->offset);
                    dump_scope(symbol->type->function.scope, indentation + 1);
                    break;
                case SYMBOL_VARIABLE:
                    printf("variable\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type->kind), symbol->type->size, symbol->offset);
                    break;
                case SYMBOL_PARAMETER:
                    printf("parameter\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type->kind), symbol->type->size, symbol->offset);
                    break;
                case SYMBOL_TEMP:
                    printf("temporary\t%s\t\t%s\t%d\t%d\n", symbol->identifier, type_as_string(symbol->type->kind), symbol->type->size, symbol->offset);
                    break;
                default:
                    break;
            }
        }
    }
}
