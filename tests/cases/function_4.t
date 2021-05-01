#
# Author: Timo Mehto
# Date: 2020/04/10

max: int = (x: int, y: int) => {
    result: int = -1;

    if x > y then {
        result := x;
    } else {
        result := y;
    }

    return result;
};

main: int = () => {
    foo: int = 42;
    bar: int = 7;

    return max(foo, bar);
};
