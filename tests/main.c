//
// TODO(timo): Filedocstring
//

#include <time.h>
#include <stdio.h>
#include "tests.h"
#define T_HASHTABLE_IMPLEMENTATION
#include "../src/hashtable.h"
#include "../src/common.h"


static const char* usage =
    "Usage:\n"
    "    t.sh test [options] [flags]\n"
    "Options:\n"
    "    -h, --help: Prints help/usage screen\n"
    "    -m, --module: Explicitly selected test module to be run. Multiple modules\n"
    "                  can be selected by passing the option multiple times.\n"
    "\n"
    "                  Available test modules and valid arguments for --module:\n"
    "                       lexer: Run tests for lexer\n"
    "                       parser: Run tests for parser\n"
    "                       resolver: Run tests for resolver\n"
    "                       interpreter: Run tests for interpreter\n"
    "                       irgenerator: Run tests for IR generator\n"
    "                       compiler: Run tests for compiler/code generator\n"
    "Flags:\n"
    "    --hide-cases: Hides the descriptions of individual test cases\n"
    "    --hide-errors: Hides the collected errors\n"
    "    --hide-summary: Hides the compilation summary\n";


static void parse_test_options(struct Test_Options* options, int* argc, char*** argv)
{
    shift(argc, argv); // skip the program name

    while (*argc > 0)
    {
        char* arg = (char*)**argv;

        if (str_equals(arg, "-h") || str_equals(arg, "--help"))
        {
            printf(usage);
            exit(1);
        }
        else if (str_equals(arg, "-m") || str_equals(arg, "--module"))
        {
            options->run_all = false;
            shift(argc, argv);
            arg = (char*)**argv;

            if (arg && str_equals(arg, "lexer"))         
                options->lexer_tests = true;
            else if (arg && str_equals(arg, "parser"))        
                options->parser_tests = true;
            else if (arg && str_equals(arg, "resolver"))      
                options->resolver_tests = true;
            else if (arg && str_equals(arg, "interpreter"))   
                options->interpreter_tests = true;
            else if (arg && str_equals(arg, "irgenerator"))   
                options->ir_generator_tests = true;
            else if (arg && str_equals(arg, "compiler"))      
                options->compiler_tests = true;
            else 
            {
                printf("Error: Invalid or missing argument for option '-m, --module'\n\n");
                printf(usage);
                exit(1);
            }
        }
        else if (str_equals(arg, "--hide-cases"))
            options->show_cases = false;
        else if (str_equals(arg, "--hide-errors"))
            options->show_errors = false;
        else if (str_equals(arg, "--hide-summary"))
            options->show_summary = false;

        shift(argc, argv);
    }
}


int main(int argc, char** argv)
{
    // TODO(timo): If argc == 1, just run all tests with some default options
    struct Test_Options options = 
    {
        .show_cases = true,
        .show_summary = true,
        .show_errors = true,

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
