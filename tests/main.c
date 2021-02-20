#include "tests.h"


int main(int argc, char** argv)
{
    test_lexer();
    test_parser();
    test_resolver();
    test_interpreter();

    return 0;
}
