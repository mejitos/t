//
// TODO(timo): Filedocstring
//

#define T_HASHTABLE_IMPLEMENTATION
#include "t.h"


int main(int argc, char** argv)
{
    struct Options options = 
    { 
        .program = "main",
        .source_file = NULL,
        .interpret = false,
        .interpret_ast = false,
        .interpret_ir = false,
        .show_summary = false,
        .show_symbols = false,
        .show_ir = false,
        .show_asm = false,
    };

    parse_options(&options, &argc, &argv);
    
    if (options.interpret && options.interpret_ast)
        interpret_from_file(options.source_file);
    else
        // TODO(timo): This probably should return somekind of code indicating 
        // the success or the non-success of the compiling process
        compile_from_file(options.source_file, options);

    return 0;
}
