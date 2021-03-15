#include "tests.h"
#include "../src/t.h"


// TODO(timo): These are being built to the main directory
// probably should make it so it creates its own package to
// here in tests directory
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
    // "./examples/local_variables_6.t",
    // "./examples/local_variables_7.t",
    // "./examples/local_variables_8.t",
    "./examples/while_loop_1.t",
    // "./examples/args_1.t",
    // "./examples/factorial.t",
    // "./examples/fibonacci.t",
};


void test_example_files()
{
    printf("Running example file tests...");

    for (int i = 0; i < sizeof(test_files) / sizeof(*test_files); i++)
        compile_from_file(test_files[i]);
}
