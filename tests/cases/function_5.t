#
# Author: Timo Mehto
# Date: 2020/04/10

g: int = () => {
    return 42;
};

f: int = () => {
    return g();
};

main: int = () => {
    return f();
};
