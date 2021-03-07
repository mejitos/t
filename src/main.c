#define T_HASHTABLE_IMPLEMENTATION
#include "t.h"


int main(int argc, char** argv)
{
    /*
    if (argc == 1) 
    {
        // TODO(timo): This should give an error and print the usage/help to the console
        const char* source = "main: int = (argc: int, argv: [int]) => {"
                             "    return 0;"
                             "};";

        compile(source);
        return 0;
    }

    // TODO(timo): Parsing and Passing user passed arguments for the program
    const char* file_path = argv[1];
    */

    // IR instructions
    // add, sub, mul, div etc
    // icmp - integer comparison
    // function local memory
    // alloca - allocates memory from stack which doesn't have to be freed since it is in stack
    // store - save things into memory
    // load - load things from memory
    //
    // br - jumps to next basic block with or without condition
    // ret - return from function
    // call - call function
    

    // TODO(timo): Do we actually do anything with this? I mean these aren't really the
    // arguments that the user can use in their compiled program. That functionality needs
    // to come from the bytecode/assembly stage?
    
    /*
    // Parsing user passed arguments
    //
    // argv[0] = program itself
    // argv[1] = source file being compiled
    // argv[2..*] = user passed arguments
     
    // NOTE(timo): If user has not passed arguments, this array will 
    // be empty and array will not have any values
    int num_args = argc - 2;
    int args[num_args];
    
    for (int i = 2; i < argc; i++)
    {
        bool negative = *argv[i] == '-';
        int arg = atoi(argv[i]);

        if (! negative  && arg < 0) // positive overflow
        {
            printf("Integer overflow in argument %d\n", i);
            exit(1);
        }
        if (negative && arg > 0) // negative overflow
        {
            printf("Integer overflow in argument %d\n", i);
            exit(1);
        }
        
        args[i - 2] = atoi(argv[i]);
    }
    
    if (argc - 2 > 0)
    {
        for (int i = 0; i < argc - 2; i++)
            printf("arg %d: %d\n", i, args[i]);
    }
    */

    // TODO(timo): This probably should return somekind of code indicating the success
    // or non-success of the compiling process
    // NOTE(timo): For now we just use hardcoded path
    compile_from_file("./examples/first.t");
    // compile_from_file("./examples/factorial.t");
    // compile_from_file("./examples/fibonacci.t");
    // compile_from_file(file_path);

    return 0;
}
