#define T_HASHTABLE_IMPLEMENTATION
#include "t.h"


int main(int argc, char** argv)
{
    Options options = { 0 };

    if (argc < 2) 
    {
        printf("Usage:\n");
        printf("    t.sh source_file [-v]");
        printf("Options:\n");
        printf("    -v: Verbose -> show instruction dumps and compilation summary\n");
        exit(1);
    }

    // TODO(timo): Parsing and Passing user passed arguments for the program
    // Parsing user passed arguments
    
    // argv[0] = program itself
    // argv[1] = source file being compiled
    // argv[2..*] = user passed arguments
    const char* file_path = argv[1];

    if (argc >= 3 && strcmp(argv[2], "-v") == 0)
        options.flag_verbose = true;
    
    // TODO(timo): This probably should return somekind of code indicating the success
    // or non-success of the compiling process
    compile_from_file(file_path, options);

    return 0;
}
