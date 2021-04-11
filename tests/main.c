#include <time.h>
#include <stdio.h>
#include "tests.h"
#define T_HASHTABLE_IMPLEMENTATION
#include "../src/hashtable.h"


int main(int argc, char** argv)
{
    printf("-----===== RUNNING TESTS =====-----\n");
    
    clock_t tests_start = clock();

    test_lexer();
    // test_parser();
    // test_resolver();
    // test_interpreter();
    // test_ir_generator();
    // test_example_files();

    clock_t tests_end = clock();
    double tests_time = (double)(tests_end - tests_start) * 1000 / (double)CLOCKS_PER_SEC;

    printf("-----===== %fms =====-----\n", tests_time);

    return 0;
}
