#include "asserts.h"
#include "tests.h"
#include "../src/t.h"
#include <unistd.h>


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


static char* run_example(Test_Runner* runner, const char* program_name, const char* file_path, const char* result, const char* argv)
{
    // Buffer for capturing the output
    int buffer_length = strlen(result);
    char* buffer = xmalloc(sizeof (char) * (buffer_length + 1));
    buffer[buffer_length] = 0;

    // Compile the program
    Options options = { .flag_verbose = false,
                        .program = program_name };

    compile_from_file(file_path, options);

    // TODO(timo): Make this work without this branch
    if (argv == NULL)
    {
        // Create the command to run the program
        int length = strlen(program_name) + 3; // +2 for ./ and +1 for \0
        char program[length];
        snprintf(program, length, "./%s", program_name);

        // Run the program and read the output from the program stdout
        FILE* program_stdout = popen(program, "r");

        if (program_stdout != NULL)
            fread(buffer, sizeof(char), buffer_length, program_stdout);
        else 
            assert_base(runner, false,
                "Failed to open the program '%s'", program);

        // Close the handle to the program
        // fflush(program_stdout);
        pclose(program_stdout);
        
        // Remove the created program
        char rm_program[3 + length];
        snprintf(rm_program, 3 + length, "rm %s", program);
        execute_command(rm_program);
    }
    else
    {
        // Create the command to run the program
        int length = strlen(program_name) + strlen(argv) + 4; // +2 for ./ and +1 for space and + 1 for \0
        char program[length];
        snprintf(program, length, "./%s %s", program_name, argv);

        // Run the program and read the output from the program stdout
        FILE* program_stdout = popen(program, "r");

        if (program_stdout != NULL)
            fread(buffer, sizeof(char), buffer_length, program_stdout);
        else 
            assert_base(runner, false,
                "Failed to open the program '%s'", program);

        // Close the handle to the program
        // fflush(program_stdout);
        pclose(program_stdout);
        
        // Remove the created program
        char rm_program[3 + strlen(program_name)];
        snprintf(rm_program, 3 + length, "rm %s", program_name);
        execute_command(rm_program);
    }

    return buffer;
}


static void test_example_first(Test_Runner* runner)
{
    const char* program_name = "first";
    const char* file_path = "./examples/first.t";
    const char* result = "Program exited with the value 0\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_first_negative(Test_Runner* runner)
{
    const char* program_name = "first_negative";
    const char* file_path = "./examples/first_negative.t";
    const char* result = "Program exited with the value -1\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_first_bool_true(Test_Runner* runner)
{
    const char* program_name = "first_bool_true";
    const char* file_path = "./examples/first_bool_true.t";
    const char* result = "Program exited with the value true\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_first_bool_false(Test_Runner* runner)
{
    const char* program_name = "first_bool_false";
    const char* file_path = "./examples/first_bool_false.t";
    const char* result = "Program exited with the value false\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_trivial_add(Test_Runner* runner)
{
    const char* program_name = "trivial_add";
    const char* file_path = "./examples/trivial_add.t";
    const char* result = "Program exited with the value 2\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_trivial_subtract(Test_Runner* runner)
{
    const char* program_name = "trivial_subtract";
    const char* file_path = "./examples/trivial_subtract.t";
    const char* result = "Program exited with the value 0\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_trivial_multiply(Test_Runner* runner)
{
    const char* program_name = "trivial_multiply";
    const char* file_path = "./examples/trivial_multiply.t";
    const char* result = "Program exited with the value 6\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_trivial_divide(Test_Runner* runner)
{
    const char* program_name = "trivial_divide";
    const char* file_path = "./examples/trivial_divide.t";
    const char* result = "Program exited with the value 5\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_trivial_arithmetics(Test_Runner* runner)
{
    const char* program_name = "trivial_arithmetics";
    const char* file_path = "./examples/trivial_arithmetics.t";
    const char* result = "Program exited with the value 7\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_not_1(Test_Runner* runner)
{
    const char* program_name = "logical_not_1";
    const char* file_path = "./examples/logical_not_1.t";
    const char* result = "Program exited with the value false\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_not_2(Test_Runner* runner)
{
    const char* program_name = "logical_not_2";
    const char* file_path = "./examples/logical_not_2.t";
    const char* result = "Program exited with the value true\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_and_1(Test_Runner* runner)
{
    const char* program_name = "logical_and_1";
    const char* file_path = "./examples/logical_and_1.t";
    const char* result = "Program exited with the value true\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_and_2(Test_Runner* runner)
{
    const char* program_name = "logical_and_2";
    const char* file_path = "./examples/logical_and_2.t";
    const char* result = "Program exited with the value false\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_and_3(Test_Runner* runner)
{
    const char* program_name = "logical_and_3";
    const char* file_path = "./examples/logical_and_3.t";
    const char* result = "Program exited with the value false\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_and_4(Test_Runner* runner)
{
    const char* program_name = "logical_and_4";
    const char* file_path = "./examples/logical_and_4.t";
    const char* result = "Program exited with the value false\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_or_1(Test_Runner* runner)
{
    const char* program_name = "logical_or_1";
    const char* file_path = "./examples/logical_or_1.t";
    const char* result = "Program exited with the value true\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_or_2(Test_Runner* runner)
{
    const char* program_name = "logical_or_2";
    const char* file_path = "./examples/logical_or_2.t";
    const char* result = "Program exited with the value true\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_or_3(Test_Runner* runner)
{
    const char* program_name = "logical_or_3";
    const char* file_path = "./examples/logical_or_3.t";
    const char* result = "Program exited with the value true\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_logical_or_4(Test_Runner* runner)
{
    const char* program_name = "logical_or_4";
    const char* file_path = "./examples/logical_or_4.t";
    const char* result = "Program exited with the value false\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_1(Test_Runner* runner)
{
    const char* program_name = "local_variables_1";
    const char* file_path = "./examples/local_variables_1.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_2(Test_Runner* runner)
{
    const char* program_name = "local_variables_2";
    const char* file_path = "./examples/local_variables_2.t";
    const char* result = "Program exited with the value 49\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_3(Test_Runner* runner)
{
    const char* program_name = "local_variables_3";
    const char* file_path = "./examples/local_variables_3.t";
    const char* result = "Program exited with the value 35\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_4(Test_Runner* runner)
{
    const char* program_name = "local_variables_4";
    const char* file_path = "./examples/local_variables_4.t";
    const char* result = "Program exited with the value 294\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_5(Test_Runner* runner)
{
    const char* program_name = "local_variables_5";
    const char* file_path = "./examples/local_variables_5.t";
    const char* result = "Program exited with the value 6\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_6(Test_Runner* runner)
{
    const char* program_name = "local_variables_6";
    const char* file_path = "./examples/local_variables_6.t";
    const char* result = "Program exited with the value 70\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_7(Test_Runner* runner)
{
    const char* program_name = "local_variables_7";
    const char* file_path = "./examples/local_variables_7.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_local_variables_8(Test_Runner* runner)
{
    const char* program_name = "local_variables_8";
    const char* file_path = "./examples/local_variables_8.t";
    const char* result = "Program exited with the value 168\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_global_variables_1(Test_Runner* runner)
{
    const char* program_name = "global_variables_1";
    const char* file_path = "./examples/global_variables_1.t";
    const char* result = "Program exited with the value 0\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_global_variables_2(Test_Runner* runner)
{
    const char* program_name = "global_variables_2";
    const char* file_path = "./examples/global_variables_2.t";
    const char* result = "Program exited with the value 49\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_1(Test_Runner* runner)
{
    const char* program_name = "if_1";
    const char* file_path = "./examples/if_1.t";
    const char* result = "Program exited with the value 0\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_2(Test_Runner* runner)
{
    const char* program_name = "if_2";
    const char* file_path = "./examples/if_2.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_3(Test_Runner* runner)
{
    const char* program_name = "if_3";
    const char* file_path = "./examples/if_3.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_4(Test_Runner* runner)
{
    const char* program_name = "if_4";
    const char* file_path = "./examples/if_4.t";
    const char* result = "Program exited with the value 0\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_5(Test_Runner* runner)
{
    const char* program_name = "if_5";
    const char* file_path = "./examples/if_5.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_6(Test_Runner* runner)
{
    const char* program_name = "if_6";
    const char* file_path = "./examples/if_6.t";
    const char* result = "Program exited with the value 0\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_7(Test_Runner* runner)
{
    const char* program_name = "if_7";
    const char* file_path = "./examples/if_7.t";
    const char* result = "Program exited with the value 1\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_8(Test_Runner* runner)
{
    const char* program_name = "if_8";
    const char* file_path = "./examples/if_8.t";
    const char* result = "Program exited with the value 1\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_9(Test_Runner* runner)
{
    const char* program_name = "if_9";
    const char* file_path = "./examples/if_9.t";
    const char* result = "Program exited with the value 1\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_10(Test_Runner* runner)
{
    const char* program_name = "if_10";
    const char* file_path = "./examples/if_10.t";
    const char* result = "Program exited with the value 1\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_if_11(Test_Runner* runner)
{
    const char* program_name = "if_11";
    const char* file_path = "./examples/if_11.t";
    const char* result = "Program exited with the value 777\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_while_1(Test_Runner* runner)
{
    const char* program_name = "while_1";
    const char* file_path = "./examples/while_loop_1.t";
    const char* result = "Program exited with the value 4950\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_while_2(Test_Runner* runner)
{
    const char* program_name = "while_2";
    const char* file_path = "./examples/while_loop_2.t";
    const char* result = "Program exited with the value 105\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_while_3(Test_Runner* runner)
{
    const char* program_name = "while_3";
    const char* file_path = "./examples/while_loop_3.t";
    const char* result = "Program exited with the value 28\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_while_4(Test_Runner* runner)
{
    const char* program_name = "while_4";
    const char* file_path = "./examples/while_loop_4.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_1(Test_Runner* runner)
{
    const char* program_name = "function_1";
    const char* file_path = "./examples/function_1.t";
    const char* result = "Program exited with the value 24352\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_2(Test_Runner* runner)
{
    const char* program_name = "function_2";
    const char* file_path = "./examples/function_2.t";
    const char* result = "Program exited with the value 777\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_3(Test_Runner* runner)
{
    const char* program_name = "function_3";
    const char* file_path = "./examples/function_3.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_4(Test_Runner* runner)
{
    const char* program_name = "function_4";
    const char* file_path = "./examples/function_4.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_5(Test_Runner* runner)
{
    const char* program_name = "function_5";
    const char* file_path = "./examples/function_5.t";
    const char* result = "Program exited with the value 42\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_6(Test_Runner* runner)
{
    const char* program_name = "function_6";
    const char* file_path = "./examples/function_6.t";
    const char* result = "Program exited with the value 21\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_function_7(Test_Runner* runner)
{
    const char* program_name = "function_7";
    const char* file_path = "./examples/function_7.t";
    const char* result = "Program exited with the value 100\n";
    const char* args = NULL;

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_args_1(Test_Runner* runner)
{
    const char* program_name = "args_1";
    const char* file_path = "./examples/args_1.t";
    const char* result = "Program exited with the value 3\n";
    const char* args = "1 2 3";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_args_2(Test_Runner* runner)
{
    const char* program_name = "args_2";
    const char* file_path = "./examples/args_2.t";
    const char* result = "Program exited with the value 1\n";
    const char* args = "1 2 3";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_args_3(Test_Runner* runner)
{
    const char* program_name = "args_3";
    const char* file_path = "./examples/args_3.t";
    const char* result = "Program exited with the value 2\n";
    const char* args = "1 2 3 4";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_args_4(Test_Runner* runner)
{
    const char* program_name = "args_4";
    const char* file_path = "./examples/args_4.t";
    const char* result = "Program exited with the value 1\n";
    const char* args = "1 2 3 4";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_factorial(Test_Runner* runner)
{
    const char* program_name = "factorial";
    const char* file_path = "./examples/factorial.t";
    const char* result = "Program exited with the value 3628800\n";
    const char* args = "10";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_fibonacci(Test_Runner* runner)
{
    const char* program_name = "fibonacci";
    const char* file_path = "./examples/fibonacci.t";
    const char* result = "Program exited with the value 512\n";
    const char* args = "10";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


static void test_example_prime_number(Test_Runner* runner)
{
    const char* program_name = "prime_number";
    const char* file_path = "./examples/prime_number.t";
    const char* result = "Program exited with the value true\n";
    const char* args = "29";

    char* buffer = run_example(runner, program_name, file_path, result, args);
    
    assert_base(runner, strcmp(result, buffer) == 0,
        "Invalid exit value '%s', expected '%s'", buffer, result);

    free(buffer);
}


Test_Set* compiler_test_set()
{
    Test_Set* set = test_set("Compiler");

    // First
    array_push(set->tests, test_case("Example file: first.t", test_example_first));
    array_push(set->tests, test_case("Example file: first_negative.t", test_example_first_negative));
    array_push(set->tests, test_case("Example file: first_bool_true.t", test_example_first_bool_true));
    array_push(set->tests, test_case("Example file: first_bool_false.t", test_example_first_bool_false));
    // Trivial basic stuff
    array_push(set->tests, test_case("Example file: trivial_add.t", test_example_trivial_add));
    array_push(set->tests, test_case("Example file: trivial_subtract.t", test_example_trivial_subtract));
    array_push(set->tests, test_case("Example file: trivial_multiply.t", test_example_trivial_multiply));
    array_push(set->tests, test_case("Example file: trivial_divide.t", test_example_trivial_divide));
    array_push(set->tests, test_case("Example file: trivial_arithmetics.t", test_example_trivial_arithmetics));
    array_push(set->tests, test_case("Example file: logical_not_1.t", test_example_logical_not_1));
    array_push(set->tests, test_case("Example file: logical_not_2.t", test_example_logical_not_2));
    array_push(set->tests, test_case("Example file: logical_and_1.t", test_example_logical_and_1));
    array_push(set->tests, test_case("Example file: logical_and_2.t", test_example_logical_and_2));
    array_push(set->tests, test_case("Example file: logical_and_3.t", test_example_logical_and_3));
    array_push(set->tests, test_case("Example file: logical_and_4.t", test_example_logical_and_4));
    array_push(set->tests, test_case("Example file: logical_or_1.t", test_example_logical_or_1));
    array_push(set->tests, test_case("Example file: logical_or_2.t", test_example_logical_or_2));
    array_push(set->tests, test_case("Example file: logical_or_3.t", test_example_logical_or_3));
    array_push(set->tests, test_case("Example file: logical_or_4.t", test_example_logical_or_4));
    
    // Local variables
    // Return variable
    array_push(set->tests, test_case("Example file: local_variables_1.t", test_example_local_variables_1));
    // Arithmetics
    array_push(set->tests, test_case("Example file: local_variables_2.t", test_example_local_variables_2));
    array_push(set->tests, test_case("Example file: local_variables_3.t", test_example_local_variables_3));
    array_push(set->tests, test_case("Example file: local_variables_4.t", test_example_local_variables_4));
    array_push(set->tests, test_case("Example file: local_variables_5.t", test_example_local_variables_5));
    array_push(set->tests, test_case("Example file: local_variables_6.t", test_example_local_variables_6));
    // Reassign variable value
    array_push(set->tests, test_case("Example file: local_variables_7.t", test_example_local_variables_7));
    array_push(set->tests, test_case("Example file: local_variables_8.t", test_example_local_variables_8));

    // Global variables
    array_push(set->tests, test_case("Example file: global_variables_1.t", test_example_global_variables_1));
    array_push(set->tests, test_case("Example file: global_variables_2.t", test_example_global_variables_2));

    // If statements
    array_push(set->tests, test_case("Example file: if_1.t", test_example_if_1));
    array_push(set->tests, test_case("Example file: if_2.t", test_example_if_2));
    array_push(set->tests, test_case("Example file: if_3.t", test_example_if_3));
    array_push(set->tests, test_case("Example file: if_4.t", test_example_if_4));
    array_push(set->tests, test_case("Example file: if_5.t", test_example_if_5));
    array_push(set->tests, test_case("Example file: if_6.t", test_example_if_6));
    // With else block
    array_push(set->tests, test_case("Example file: if_7.t", test_example_if_7));
    array_push(set->tests, test_case("Example file: if_8.t", test_example_if_8));
    array_push(set->tests, test_case("Example file: if_9.t", test_example_if_9));
    // With simple boolean value as condition
    array_push(set->tests, test_case("Example file: if_10.t", test_example_if_10));
    array_push(set->tests, test_case("Example file: if_11.t", test_example_if_11));

    // While statements
    array_push(set->tests, test_case("Example file: while_loop_1.t", test_example_while_1));
    array_push(set->tests, test_case("Example file: while_loop_2.t", test_example_while_2));
    // With break
    array_push(set->tests, test_case("Example file: while_loop_3.t", test_example_while_3));
    // Basic "while true" -loop
    array_push(set->tests, test_case("Example file: while_loop_4.t", test_example_while_3));

    // Functions
    array_push(set->tests, test_case("Example file: function_1.t", test_example_function_1));
    array_push(set->tests, test_case("Example file: function_2.t", test_example_function_2));
    array_push(set->tests, test_case("Example file: function_3.t", test_example_function_3));
    array_push(set->tests, test_case("Example file: function_4.t", test_example_function_4));
    array_push(set->tests, test_case("Example file: function_5.t", test_example_function_5));
    array_push(set->tests, test_case("Example file: function_6.t", test_example_function_6));
    array_push(set->tests, test_case("Example file: function_7.t", test_example_function_7));

    // Command line arguments
    array_push(set->tests, test_case("Example file: args_1.t", test_example_args_1));
    array_push(set->tests, test_case("Example file: args_2.t", test_example_args_2));
    array_push(set->tests, test_case("Example file: args_3.t", test_example_args_3));
    array_push(set->tests, test_case("Example file: args_4.t", test_example_args_4));

    // Little programs
    array_push(set->tests, test_case("Example file: factorial.t", test_example_factorial));
    array_push(set->tests, test_case("Example file: fibonacci.t", test_example_fibonacci));
    array_push(set->tests, test_case("Example file: prime_number.t", test_example_prime_number));

    set->length = set->tests->length;

    return set;
}
