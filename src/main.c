#include "t.h"


int main(int argc, char** argv)
{
    const char* source = "foo\n 42\n true  .  then";

    compile(source);

    return 0;
}
