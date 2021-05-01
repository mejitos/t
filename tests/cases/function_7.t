#
# Author: Timo Mehto
# Date: 2020/04/10

g: int = (n: int) => {
    return n + n;
};

f: int = (n: int) => {
    return n * n;
};

main: int = () => {
    return f(g(5));
};
