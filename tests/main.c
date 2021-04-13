#include <time.h>
#include <stdio.h>
#include "tests.h"
#define T_HASHTABLE_IMPLEMENTATION
#include "../src/hashtable.h"


int main(int argc, char** argv)
{
    // TODO(timo): Parse command line arguments

    printf("-----===== RUNNING TESTS =====-----\n");
    
    Test_Runner runner;
    test_runner_init(&runner);

    // TODO(timo): Collect tests based on user arguments
    test_runner_append_test_set(&runner, lexer_test_set());

    clock_t tests_start = clock();
    test_runner_run(&runner);
    clock_t tests_end = clock();

    runner.time = (double)(tests_end - tests_start) * 1000 / (double)CLOCKS_PER_SEC;
    
    // TODO(timo): If option is set
    test_runner_print_errors(&runner);
    // TODO(timo): If option is set
    test_runner_print_summary(&runner);

    test_runner_free(&runner);

    return 0;
}
