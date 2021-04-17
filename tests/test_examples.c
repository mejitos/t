#include "tests.h"
#include "../src/t.h"
#include <unistd.h>
#include <fcntl.h>

#define NUMBER_OF_TESTS 35

const char* tests[NUMBER_OF_TESTS][3] =
{
    { "./examples/first.t",                 "first",                "Program exited with the value 0\n" },      // first program (just return 0) 
    { "./examples/trivial_add.t",           "trivial_add",          "Program exited with the value 2\n" },      // trivial addition
    { "./examples/trivial_subtract.t",      "trivial_subtract",     "Program exited with the value 0\n" },      // trivial subtract
    { "./examples/trivial_multiply.t",      "trivial_multiply",     "Program exited with the value 6\n" },      // trivial multiplication
    { "./examples/trivial_divide.t",        "trivial_divide",       "Program exited with the value 5\n" },      // trivial division
    { "./examples/trivial_arithmetics.t",   "trivial_arithmetics",  "Program exited with the value 7\n" },      // trivial arithmetics
    { "./examples/local_variables_1.t",     "local_variables_1",    "Program exited with the value 42\n" },     // local variables 1 (return simple value)
    { "./examples/local_variables_2.t",     "local_variables_2",    "Program exited with the value 49\n" },     // local variables 2 (addition)
    { "./examples/local_variables_3.t",     "local_variables_3",    "Program exited with the value 35\n" },     // local variables 3 (subtraction)
    { "./examples/local_variables_4.t",     "local_variables_4",    "Program exited with the value 294\n" },    // local variables 4 (multiplication)
    { "./examples/local_variables_5.t",     "local_variables_5",    "Program exited with the value 6\n" },      // local variables 5 (division)
    { "./examples/local_variables_6.t",     "local_variables_6",    "Program exited with the value 70\n" },     // local variables 6 (arithmetics)
    { "./examples/local_variables_7.t",     "local_variables_7",    "Program exited with the value 42\n" },     // local variables 7 (reassign variable value)
    { "./examples/local_variables_8.t",     "local_variables_7",    "Program exited with the value 168\n" },    // local variables 8 (reassign variable value)
    { "./examples/if_1.t",                  "if_1",                 "Program exited with the value 0\n" },      // if 1 (equals)
    { "./examples/if_2.t",                  "if_2",                 "Program exited with the value 42\n" },     // if 2 (not equal)
    { "./examples/if_3.t",                  "if_3",                 "Program exited with the value 42\n" },     // if 3 (less than)
    { "./examples/if_4.t",                  "if_4",                 "Program exited with the value 0\n" },      // if 4 (less than equal)
    { "./examples/if_5.t",                  "if_5",                 "Program exited with the value 42\n" },     // if 5 (greater than)
    { "./examples/if_6.t",                  "if_6",                 "Program exited with the value 0\n" },      // if 6 (greater than equal)
    { "./examples/if_7.t",                  "if_7",                 "Program exited with the value 1\n" },      // if 7 (if-else not equal)
    { "./examples/if_8.t",                  "if_8",                 "Program exited with the value 1\n" },      // if 8 (if-else less than)
    { "./examples/if_9.t",                  "if_9",                 "Program exited with the value 1\n" },      // if 9 (if-else greater than)
    { "./examples/while_loop_1.t",          "while_loop_1",         "Program exited with the value 4950\n" },   // while 1
    { "./examples/while_loop_2.t",          "while_loop_2",         "Program exited with the value 105\n" },    // while 2
    { "./examples/while_loop_3.t",          "while_loop_3",         "Program exited with the value 28\n" },     // while 3 (break statement)
    { "./examples/function_1.t",            "function_1",           "Program exited with the value 24352\n" },  // function 1
    { "./examples/function_2.t",            "function_2",           "Program exited with the value 777\n" },    // function 2
    { "./examples/function_3.t",            "function_3",           "Program exited with the value 42\n" },     // function 3
    { "./examples/function_4.t",            "function_4",           "Program exited with the value 42\n" },     // function 4
    { "./examples/function_5.t",            "function_5",           "Program exited with the value 42\n" },     // function 5
    { "./examples/function_6.t",            "function_6",           "Program exited with the value 21\n" },     // function 6
    { "./examples/function_7.t",            "function_7",           "Program exited with the value 100\n" },    // function 7

    { "./examples/fibonacci.t",             "fibonacci",            "Program exited with the value 512\n" },    // fibonacci
    { "./examples/factorial.t",             "factorial",            "Program exited with the value 3628800\n" },// factorial
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
    // setvbuf(stdout, (char*)NULL, _IOLBF, 0);

    printf("Running example file tests...\n");

    for (int i = 0; i < sizeof(tests) / sizeof(*tests); i++)
    {
        Options options = { .flag_verbose = false,
                            .program = tests[i][1] };

        printf("\t%s ... ", tests[i][0]);
        // printf("        Compiling...");

        // TODO(timo): Return some kind of value from the compilation?
        compile_from_file(tests[i][0], options);
        
        // TODO(timo): assert that the compilation was successful

        // printf("OK\n");
        // printf("        Executing...");

        // TODO(timo): file will be executed only if the compilation was successful

        /*
        int pipes[2];
        pipe(pipes);

        if (fork() == 0)
        {
            dup2(pipes[1], 1);
            close(pipes[1]);

            int length = strlen(tests[i][1]) + 2 + 1;
            char program[length];
            snprintf(program, length, "./%s", tests[i][1]);

            execlp(program, "");
        }
        else
        {
            int buffer_length = strlen(tests[i][2]);
            char buffer[buffer_length + 1];
            buffer[buffer_length] = 0;
            read(pipes[0], buffer, buffer_length);
        }
        */
        
        // Execute the program
        // fflush(stdout);
        int length = strlen(tests[i][1]) + 2 + 1;
        char program[length];
        snprintf(program, length, "./%s", tests[i][1]);
        // FILE* program_stdout = popen("./main", "r");
        FILE* program_stdout = popen(program, "r");
        // FILE* program_stdout = popen("./main", "re");
        // execute_command(program);
        // fcntl(fileno(program_stdout), F_SETFL, O_NONBLOCK);
        
        if (program_stdout != NULL)
        {
            // Capture the output from program
            int buffer_length = strlen(tests[i][2]);
            char buffer[buffer_length + 1];
            buffer[buffer_length] = 0;
            
            fread(buffer, sizeof(char), buffer_length, program_stdout);
            // read(fileno(program_stdout), buffer, buffer_length);
            
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
        
            if (strcmp(tests[i][2], buffer) == 0)
                printf("OK\n");
            else
                printf("FAILED\n");
        }
        else 
        {
            printf("FAILED\n");
            printf("    Failed to open main\n");
        }

        // sleep(1);
        fflush(program_stdout);
        
        if (pclose(program_stdout) != 0)
            printf("Invalid return value\n");

        char rm_program[4 + strlen(tests[i][1])];
        snprintf(rm_program, 4 + strlen(tests[i][1]), "rm %s", tests[i][1]);
        execute_command(rm_program);
    }
}
