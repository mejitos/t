#include "t.h"
#define T_HASHTABLE_IMPLEMENTATION


int main(int argc, char** argv)
{
    const char* source = "main: int = (argc: int, argv: int) => {"
                         "    return 0;"
                         "};";

    compile(source);

    return 0;
}
