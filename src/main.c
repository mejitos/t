#define T_HASHTABLE_IMPLEMENTATION
#include "t.h"


const char* test_files[] =
{
    // "./examples/trivial_add.t",
    // "./examples/trivial_subtract.t",
    // "./examples/trivial_multiply.t",
    // "./examples/trivial_divide.t",
    // "./examples/trivial_arithmetics.t",
    // "./examples/local_variables_1.t",
    // "./examples/local_variables_2.t",
    // "./examples/local_variables_3.t",
    // "./examples/local_variables_4.t",
    // "./examples/local_variables_5.t",
    "./examples/local_variables_6.t",
    // "./examples/args_1.t",
    // "./examples/factorial.t",
    // "./examples/fibonacci.t",
};


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
    // Parsing user passed arguments
    //
    // argv[0] = program itself
    // argv[1] = source file being compiled
    // argv[2..*] = user passed arguments
    
    const char* file_path = argv[1];
    */


    // TODO(timo): This probably should return somekind of code indicating the success
    // or non-success of the compiling process
    // NOTE(timo): For now we just use hardcoded path
    for (int i = 0; i < sizeof(test_files) / sizeof(*test_files); i++)
        compile_from_file(test_files[i]);

    // compile_from_file(file_path);

    return 0;
}
