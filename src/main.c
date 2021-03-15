#define T_HASHTABLE_IMPLEMENTATION
#include "t.h"


int main(int argc, char** argv)
{
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
    // Parsing user passed arguments
    //
    // argv[0] = program itself
    // argv[1] = source file being compiled
    // argv[2..*] = user passed arguments
    
    const char* file_path = argv[1];
    // TODO(timo): This probably should return somekind of code indicating the success
    // or non-success of the compiling process
    compile_from_file(file_path);

    return 0;
}
