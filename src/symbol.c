#include "t.h"


Symbol* symbol_variable(AST_Declaration* declaration)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->state = STATE_UNRESOLVED;
    symbol->kind = SYMBOL_VARIABLE;
    symbol->identifier = declaration->identifier->lexeme;
    symbol->type = resolve_type_specifier(declaration->specifier);
        
    return symbol;
}


Symbol* symbol_function(AST_Declaration* declaration)
{
    Symbol* symbol = xcalloc(1, sizeof (Symbol));
    symbol->state = STATE_UNRESOLVED;
    symbol->kind = SYMBOL_FUNCTION;
    symbol->identifier = declaration->identifier->lexeme;
    symbol->type = resolve_type_specifier(declaration->specifier);

    return symbol;
}
