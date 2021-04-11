#include "tests.h"
#include "../src/t.h"
#include <unistd.h>
#include <fcntl.h>


const char* test_files[] =
{
    "./examples/first.t",
    "./examples/trivial_add.t",
    "./examples/trivial_subtract.t",
    "./examples/trivial_multiply.t",
    "./examples/trivial_divide.t",
    "./examples/trivial_arithmetics.t",
    "./examples/local_variables_1.t",
    "./examples/local_variables_2.t",
    "./examples/local_variables_3.t",
    "./examples/local_variables_4.t",
    "./examples/local_variables_5.t",
    "./examples/local_variables_6.t",
    "./examples/local_variables_7.t",
    "./examples/local_variables_8.t",
    "./examples/if_1.t",
    "./examples/if_2.t",
    "./examples/if_3.t",
    "./examples/if_4.t",
    "./examples/if_5.t",
    "./examples/if_6.t",
    "./examples/if_7.t",
    "./examples/if_8.t",
    "./examples/if_9.t",
    "./examples/while_loop_1.t",
    "./examples/while_loop_2.t",
    "./examples/while_loop_3.t",
    "./examples/function_1.t",
    "./examples/function_2.t",
    "./examples/function_3.t",
    "./examples/function_4.t",
    "./examples/function_5.t",
    "./examples/function_6.t",
    "./examples/function_7.t",

    "./examples/fibonacci.t",
    "./examples/factorial.t",
};


const char* results[] =
{
    "Program exited with the value 0\n",        // first program (just return 0) 
    "Program exited with the value 2\n",        // trivial addition
    "Program exited with the value 0\n",        // trivial subtract
    "Program exited with the value 6\n",        // trivial multiplication
    "Program exited with the value 5\n",        // trivial division
    "Program exited with the value 7\n",        // trivial arithmetics
    "Program exited with the value 42\n",       // local variables 1 (return simple value)
    "Program exited with the value 49\n",       // local variables 2 (addition)
    "Program exited with the value 35\n",       // local variables 3 (subtraction)
    "Program exited with the value 294\n",      // local variables 4 (multiplication)
    "Program exited with the value 6\n",        // local variables 5 (division)
    "Program exited with the value 70\n",       // local variables 6 (arithmetics)
    "Program exited with the value 42\n",       // local variables 7 (reassign variable value)
    "Program exited with the value 168\n",      // local variables 8 (reassign variable value)
    "Program exited with the value 0\n",        // if 1 (equals)
    "Program exited with the value 42\n",       // if 2 (not equal)
    "Program exited with the value 42\n",       // if 3 (less than)
    "Program exited with the value 0\n",        // if 4 (less than equal)
    "Program exited with the value 42\n",       // if 5 (greater than)
    "Program exited with the value 0\n",        // if 6 (greater than equal)
    "Program exited with the value 1\n",        // if 7 (if-else not equal)
    "Program exited with the value 1\n",        // if 8 (if-else less than)
    "Program exited with the value 1\n",        // if 9 (if-else greater than)
    "Program exited with the value 4950\n",     // while 1
    "Program exited with the value 105\n",      // while 2
    "Program exited with the value 28\n",       // while 3 (break statement)
    "Program exited with the value 24352\n",    // function 1
    "Program exited with the value 777\n",      // function 2
    "Program exited with the value 42\n",       // function 3
    "Program exited with the value 42\n",       // function 4
    "Program exited with the value 42\n",       // function 5
    "Program exited with the value 21\n",       // function 6
    "Program exited with the value 100\n",      // function 7

    "Program exited with the value 512\n",      // fibonacci
    "Program exited with the value 3628800\n",  // factorial
};


int execute_command(char* command)
{
    int error_code;

    if ((error_code = system(command)) != 0)
    {
        printf("Error code on command '%s': %d\n", command, error_code);
        exit(1);
    }

    return error_code;
}


void test_example_files()
{
    Options options = { 0 };

    // setvbuf(stdout, (char*)NULL, _IOLBF, 0);

    printf("Running example file tests...\n");

    for (int i = 20; i < sizeof(test_files) / sizeof(*test_files); i++)
    {
        printf("\t%s ... ", test_files[i]);
        // printf("        Compiling...");

        // TODO(timo): Return some kind of value from the compilation?
        compile_from_file(test_files[i], options);
        
        // TODO(timo): assert that the compilation was successful

        // printf("OK\n");
        // printf("        Executing...");

        // TODO(timo): file will be executed only if the compilation was successful
        
        // Execute the program
        fflush(stdout);
        FILE* program_stdout = popen("./main", "r");
        // FILE* program_stdout = popen("./main", "re");

        // fcntl(fileno(program_stdout), F_SETFL, O_NONBLOCK);

        if (program_stdout != NULL)
        {
            // Capture the output from program
            int buffer_length = strlen(results[i]);
            char buffer[buffer_length + 1];
            buffer[buffer_length] = 0;
            
            // fread(buffer, sizeof(char), buffer_length, program_stdout);
            read(fileno(program_stdout), buffer, buffer_length);
           
            /*
            while (!feof(program_stdout))
            {
                if (fgets(buffer, buffer_length + 1, program_stdout) != NULL)
                {
                    // printf("\n");
                    // NOTE(timo): For some reason closing the file handle
                    // at this point will make this thing not work after a while
                    // -> the console buffer just freezes up
                    // pclose(program_stdout);
                    // break;

                }
            }
            */

            /*
            while (fgets(buffer, buffer_length + 1, program_stdout) != NULL)
            {
                // printf("\n");
                // NOTE(timo): For some reason closing the file handle
                // at this point will make this thing not work after a while
                // -> the console buffer just freezes up
                // pclose(program_stdout);
                break;

            }
            */
        
            if (strcmp(results[i], buffer) == 0)
            {
                printf("OK\n");
            }
            else
                printf("FAILED\n");
        }
        else 
        {
            printf("FAILED\n");
            printf("    Failed to open main\n");
        }

        // sleep(1);
        // fflush(program_stdout);
        
        if (pclose(program_stdout) != 0)
            printf("Invalid return value\n");
    }
}
