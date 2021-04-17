#ifndef t_tests_h
#define t_tests_h

#include "../src/array.h"
#include <stdbool.h>

typedef struct Test_Runner Test_Runner;


typedef struct Test_Error
{
    const char* test_set;
    const char* test_case;
    const char* message;
} Test_Error;


typedef struct Test_Case
{
    const char* description;
    void (*test)(Test_Runner* runner);
} Test_Case;


typedef struct Test_Set
{
    const char* name;
    const char* description;
    int length;
    // Test_Case tests[];
    array* tests;
} Test_Set;


struct Test_Runner
{
    int number_of_sets;
    int number_of_tests;
    int passed;
    int failed;
    double time;
    bool error;
    Test_Set* current_set;
    Test_Case* current_case;
    array* test_sets;
    array* errors;
};


void test_runner_init(Test_Runner* runner);
void test_runner_append_test_set(Test_Runner* runner, Test_Set* set);
void test_runner_run(Test_Runner* runner);
void test_runner_print_summary(Test_Runner* runner);
void test_runner_print_errors(Test_Runner* runner);
void test_runner_free(Test_Runner* runner);

Test_Case* test_case(const char* description, const void* test);
Test_Set* test_set(const char* description);
Test_Error* test_error(const char* test_set, const char* test_case, const char* message);

Test_Set* lexer_test_set();
Test_Set* parser_test_set();
Test_Set* resolver_test_set();
Test_Set* interpreter_test_set();
Test_Set* ir_generator_test_set();
Test_Set* compiler_test_set();

#endif
