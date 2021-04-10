#
# Author: Timo Mehto
# Date: 2020/03/16

my_func: int = () => {
    return 42;
};

main: int = () => {
    foo: int = my_func();
    return foo;
};
