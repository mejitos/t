// Definitions for datatypes and interfaces used with the test runner.
//
// Test runner will collect and run tests while collecting infromation of them.
//
// Author: Timo Mehto
// Date: 2021/05/12

#ifndef t_tests_h
#define t_tests_h

#include "../src/array.h"
#include <stdbool.h>


// Type definitions
typedef struct Test_Runner Test_Runner;
typedef struct Test_Set Test_Set;
typedef struct Test_Case Test_Case;
typedef struct Test_Error Test_Error;


// Options for test runner. They handle the info shown to the user and what
// tests are being run.
//
// Members
//     show_cases: If descriptions of individual test cases are shown or not.
//     show_summary: If test summary is shown at the end or not.
//     show_errors: If collected errors are shown at the end or not.
//
//     run_all: If all tests are run or not.
//     lexer_tests: If lexer tests are run or not.
//     parser_tests: If parser tests are run or not.
//     resolver_tests: If resolver tests are run or not.
//     ir_generator_tests: If IR generator tests are run or not.
//     compiler_tests: If compiler tests are run or not.
struct Test_Options
{
    bool show_cases;
    bool show_summary;
    bool show_errors;

    bool run_all;
    bool lexer_tests;
    bool parser_tests;
    bool resolver_tests;
    bool interpreter_tests;
    bool ir_generator_tests;
    bool compiler_tests;
};


// Collected errors from tests.
//
// Members
//      test_set: Test set which the error was from.
//      test_case: Test case which the error was from.
//      message: Description of the spotted error.
struct Test_Error
{
    const char* test_set;
    const char* test_case;
    const char* message;
};


// Test case in a test set.
//
// Members
//      description: Description of the test case.
//      test: Function pointer of a test function.
struct Test_Case
{
    const char* description;
    void (*test)(Test_Runner* runner);
};


// Test set containing the test cases.
//
// Members
//      name: Name of the test set. NOT USED AT THE MOMENT.
//      description: Description of the test set.
//      length: How many test cases there is in the the set.
//      tests: Array of test cases.
struct Test_Set
{
    const char* name;
    const char* description;
    int length;
    array* tests;
};


// Test runner to collect and run tests and to show info collected from 
// the tests.
//
// Members
//      number_of_sets: Number of test sets to be run.
//      number_of_tests: Total number of test cases to be run.
//      passed: How many tests passed.
//      failed: How many tests failed.
//      error: If there was error in the tests. This will be set by the 
//             custom made assert functions if the assertion fails.
//      current_set: Currently running test set.
//      current_case: Currently running test case.
//      test_sets: Array of test sets to be run.
//      errors: Array of collected errors.
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


// Factory function for initializing a new test runner.
//
// File(s): test_runner.c
//
// Arguments
//      runner: Address of the test runner to be initialized.
void test_runner_init(Test_Runner* runner);


// Appends new test set to the test runner.
//
// File(s): test_runner.c
//
// Arguments
//      runner: Initialized test runner.
//      set: Test set to be appended into the runner.
void test_runner_append_test_set(Test_Runner* runner, Test_Set* set);


// Main function 
//
// Arguments
//      runner: Initialized test runner.
//      options: Options to determine what is run and shown.
void test_runner_run(Test_Runner* runner, struct Test_Options options);


// Frees the memory allocated for test runner.
//
// File(s): test_runner.c
//
// Arguments
//      runner: Test runner to be freed.
void test_runner_free(Test_Runner* runner);


// Factory function for creating a new test case.
//
// File(s): test_runner.c
//
// Arguments
//      description: Description of the test case.
//      test: Pointer to function that handles the test case.
// Returns
//      Pointer to the newly created test case.
Test_Case* test_case(const char* description, const void* test);


// Factory function for creating a new test set.
//
// File(s): test_runner.c
//
// Arguments
//      description: Description of the test set.
// Returns
//      Pointer to the newly created test set.
Test_Set* test_set(const char* description);


// Factory function for creating a new test errors.
//
// File(s): test_runner.c
//
// Arguments
//      test_set: Description of the test set.
//      test_case: Description of the test case.
//      message: Description of the spotted error.
// Returns
//      Pointer to the newly created test error.
Test_Error* test_error(const char* test_set, const char* test_case, const char* message);


// Prints the summary of the tests run.
//
// File(s): test_runner.c
//
// Arguments
//      runner: Initialized test runner.
void test_runner_print_summary(Test_Runner* runner);


// Prints all the errors collected when running tests.
//
// File(s): test_runner.c
//
// Arguments
//      runner: Initialized test runner.
void test_runner_print_errors(Test_Runner* runner);


// Factory functions for creating test sets for each main module of the 
// compiler.
//
// File(s): test_lexer.c
//          test_parser.c
//          test_resolver.c
//          test_interpreter.c
//          test_ir_generator.c
//          test_examples.c
//
// Returns
//      Pointer to the newly created test set with test cases of the module.
Test_Set* lexer_test_set();
Test_Set* parser_test_set();
Test_Set* resolver_test_set();
Test_Set* interpreter_test_set();
Test_Set* ir_generator_test_set();
Test_Set* compiler_test_set();


#endif
