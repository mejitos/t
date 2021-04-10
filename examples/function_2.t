#
# Author: Timo Mehto
# Date: 2020/03/16

my_func: int = () => {
    return 777;
};

main: int = () => {
    foo: int = 0;
    foo := my_func();
    return foo;
};
