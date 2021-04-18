#include <time.h>
#include <stdio.h>
#include "tests.h"
#define T_HASHTABLE_IMPLEMENTATION
#include "../src/hashtable.h"
#include "../src/common.h"


// TODO(timo): Some kind of error handling
void parse_test_options(struct Test_Options* options, int* argc, char*** argv)
{
    shift(argc, argv); // skip the program name

    while (*argc)
    {
        if (**argv && (str_equals(**argv, "-h") || str_equals(**argv, "--help")))
        {
            printf("Usage: \n");
            exit(1);
        }
        else if (**argv && str_equals(**argv, "-m"))
        {
            options->run_all = false;
            shift(argc, argv);

            if      (**argv && str_equals(**argv, "lexer"))         options->lexer_tests = true;
            else if (**argv && str_equals(**argv, "parser"))        options->parser_tests = true;
            else if (**argv && str_equals(**argv, "resolver"))      options->resolver_tests = true;
            else if (**argv && str_equals(**argv, "interpreter"))   options->interpreter_tests = true;
            else if (**argv && str_equals(**argv, "irgenerator"))   options->ir_generator_tests = true;
            else if (**argv && str_equals(**argv, "compiler"))      options->compiler_tests = true;
            else 
            {
                printf("No value provided for argument '-m'\n");
                exit(1);
            }
        }
        else if (**argv && str_equals(**argv, "--hide-cases"))
        {
            options->show_cases = false;
            shift(argc, argv);
        }
        else if (**argv && str_equals(**argv, "--show-errors"))
        {
            options->show_errors = true;
            shift(argc, argv);
        }
        else if (**argv && str_equals(**argv, "--show-summary"))
        {
            options->show_summary = true;
            shift(argc, argv);
        }
        else shift(argc, argv);
    }
}


int main(int argc, char** argv)
{
    // TODO(timo): If argc == 1, just run all tests with some default options
    struct Test_Options options = 
    {
        .show_cases = true,
        .show_summary = false,
        .show_errors = false,

        .run_all = true,
        .lexer_tests = false,
        .parser_tests = false,
        .resolver_tests = false,
        .interpreter_tests = false,
        .ir_generator_tests = false,
        .compiler_tests = false,
    };

    parse_test_options(&options, &argc, &argv);

    Test_Runner runner;
    test_runner_init(&runner);

    if (options.lexer_tests || options.run_all)
        test_runner_append_test_set(&runner, lexer_test_set());
    if (options.parser_tests || options.run_all)
        test_runner_append_test_set(&runner, parser_test_set());
    if (options.resolver_tests || options.run_all)
        test_runner_append_test_set(&runner, resolver_test_set());
    if (options.interpreter_tests || options.run_all)
        test_runner_append_test_set(&runner, interpreter_test_set());
    if (options.ir_generator_tests || options.run_all)
        test_runner_append_test_set(&runner, ir_generator_test_set());
    if (options.compiler_tests || options.run_all)
        test_runner_append_test_set(&runner, compiler_test_set());

    if (runner.test_sets->length > 0)
    {
        printf("-----===== RUNNING TESTS =====-----\n");

        clock_t tests_start = clock();
        test_runner_run(&runner, options);
        clock_t tests_end = clock();

        runner.time = (double)(tests_end - tests_start) * 1000 / (double)CLOCKS_PER_SEC;

        if (options.show_errors)
            test_runner_print_errors(&runner);
        if (options.show_summary)
            test_runner_print_summary(&runner);
    }
    else
        printf("No tests selected to run\n");

    test_runner_free(&runner);

    return 0;
}
