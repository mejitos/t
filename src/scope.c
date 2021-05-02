//
// TODO(timo): Filedocstring
//

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

        if (symbol != NULL) 
            symbol_free(symbol);
    }

    hashtable_free(scope->symbols);

    free(scope);
    scope = NULL;
}


Symbol* scope_get(const Scope* scope, const char* identifier)
{
    return hashtable_get(scope->symbols, identifier);
}


static void scope_put(Scope* scope, Symbol* symbol)
{
    hashtable_put(scope->symbols, symbol->identifier, symbol);
}


Symbol* scope_lookup(const Scope* scope, const char* identifier)
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

    // TODO(timo): I'm not sure though this is job of the scope to set these 
    // offsets. Maybe this should be done with totally different module or
    // at least with different function? Create somekind of Sizer or something.

    // Setting the memory offsets / alignment
    if (symbol->kind == SYMBOL_PARAMETER)
    {
        if (strcmp(scope->name, "main") == 0)
            symbol->offset = scope->offset_parameter - 8;
        else
            symbol->offset = scope->offset_parameter;
        
        scope->offset_parameter += symbol->type->size;

        // TODO(timo): We should probably compute the correct alignment
        // for the parameters too
    }
    else
    {
        // NOTE(timo): This has to be offset + 8 because of how the stack 
        // works. We cannot have position stack_top - 0, since then the 
        // value would go outside of the stack.
        symbol->offset = scope->offset + 8;

        // Compute the new offset for the scope
        scope->offset += symbol->type->size;
        // scope->offset += symbol->type->alignment;

        // Lets just use the alignment of 8 bytes for everything for now to 
        // make life easier
        while (scope->offset % symbol->type->alignment != 0)
            scope->offset += 1;
    }

    scope_put(scope, symbol);
}


const bool scope_contains(const Scope* scope, const char* identifier)
{
    Symbol* symbol = scope_get(scope, identifier);

    return symbol != NULL;
}


const array* scope_symbols(const Scope* scope)
{
    array* symbols = array_init(sizeof (Symbol*));

    for (int i = 0; i < scope->symbols->capacity; i++)
    {
        Symbol* symbol = scope->symbols->entries[i].value;

        if (symbol) array_push(symbols, symbol);
    }

    return symbols;
}


void dump_scope(const Scope* scope, int indentation)
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
