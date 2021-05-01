#
# Author: Timo Mehto
# Date: 2020/04/10

my_func: int = () => {
    return 42;
};

main: int = () => {
    foo: int = my_func();
    return foo;
};
