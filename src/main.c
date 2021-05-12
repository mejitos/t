// Entry point for the compiler where the passed compiler options will be
// parsed. The options will be passed to compiler for compilation of the
// program or to interpreter for interpretation of the program.
//
// The hashtable implementation has to be defined here so it can be used
// throughout the program.
// 
// Author: Timo Mehto
// Date: 2021/05/12

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
