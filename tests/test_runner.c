#include "../src/t.h"
#include "tests.h"


void test_runner_init(Test_Runner* runner)
{
    *runner = (Test_Runner) { .number_of_tests = 0,
                              .passed = 0,
                              .failed = 0,
                              .test_sets = array_init(sizeof (Test_Set*)),
                              .errors = array_init(sizeof(Test_Error*)) };
}


void test_runner_free(Test_Runner* runner)
{
    for (int i = 0; i < runner->test_sets->length; i++)
    {
        Test_Set* set = runner->test_sets->items[i];

        for (int j = 0; j < set->tests->length; j++)
        {
            Test_Case* _case = set->tests->items[j];
            free((char*)_case->description);
            _case->description = NULL;

            free(_case);
            _case = NULL;
        }

        array_free(set->tests);
        
        free((char*)set->description);
        set->description = NULL;
        free(set);
        set = NULL;
    }

    array_free(runner->test_sets);
    array_free(runner->errors);
}


Test_Case* test_case(const char* description, const void* test)
{
    int description_length = strlen(description);
    Test_Case* _case = xmalloc(sizeof (Test_Case));
    _case->description = xcalloc(description_length + 1, sizeof (char));
    memcpy((char*)_case->description, description, description_length);
    _case->test = test;

    return _case;
}


Test_Set* test_set(const char* description)
{
    int description_length = strlen(description);
    Test_Set* set = xmalloc(sizeof (Test_Set));
    set->description = xcalloc(description_length + 1, sizeof (char));
    memcpy((char*)set->description, description, description_length);
    set->tests = array_init(sizeof (Test_Case*));
    set->length = 0;

    return set;
}


Test_Error* test_error(const char* test_set, const char* test_case, const char* message)
{
    int test_set_length = strlen(test_set);
    int test_case_length = strlen(test_case);
    int message_length = strlen(message);

    Test_Error* error = xmalloc(sizeof (Test_Error));
    error->test_set = xcalloc(test_set_length + 1, sizeof (char));
    memcpy((char*)error->test_set, test_set, test_set_length);
    error->test_case = xcalloc(test_case_length + 1, sizeof (char));
    memcpy((char*)error->test_case, test_case, test_case_length);
    error->message = xcalloc(message_length + 1, sizeof (char));
    memcpy((char*)error->message, message, message_length);

    return error;
}

void test_runner_append_test_set(Test_Runner* runner, Test_Set* set)
{
    runner->number_of_tests += set->length;
    runner->number_of_sets++;
    array_push(runner->test_sets, set);
}


void test_runner_run(Test_Runner* runner)
{
    // Run tests
    for (int i = 0; i < runner->number_of_sets; i++)
    {
        Test_Set* set = (Test_Set*)runner->test_sets->items[i];
        runner->current_set = set;

        printf("Running %s tests...\n", set->description);

        for (int j = 0; j < set->length; j++)
        {
            Test_Case* _case = (Test_Case*)set->tests->items[j];
            runner->current_case = _case;

            printf("    %s...", _case->description);

            (*_case->test)(runner);

            if (runner->error) printf("FAILED\n");
            else printf("PASSED\n");

            runner->error = false;
        }
    }
}


void test_runner_print_summary(Test_Runner* runner)
{
    printf("-----===== TESTS SUMMARY =====-----\n");
    printf("Time: %fms\n", runner->time);
    printf("Tests run: %d\n", runner->number_of_tests);
    printf("Passed: %d\n", runner->passed);
    printf("Failed: %d\n", runner->failed);
    printf("Success: %0.2f%\n", (float)runner->passed / (float)runner->number_of_tests * 100);
}


void test_runner_print_errors(Test_Runner* runner)
{
    printf("-----===== TEST ERRORS =====-----\n");
    
    if (runner->errors->length > 0)
    {
        for (int i = 0; i < runner->errors->length; i++)
        {
            Test_Error* error = runner->errors->items[i];

            printf("Error #%d\n\n", i + 1);
            printf("Test set: %s\n", error->test_set);
            printf("Test case: %s\n", error->test_case);
            printf("Message: %s\n", error->message);

            if (i < runner->errors->length - 1)
                printf("-----\n");
        }
    }
    else
        printf("No errors\n");
}
