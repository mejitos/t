# Program that checks if number is a prime number
#
# Author: Timo Mehto
# Date: 2020/04/10

# Simple modulo function to get the remainder 
# since the language does not have module operator
#
# TODO(timo): When these parameters are not typed => no errors where shown
# Compiler just gave straight up a  segfault
modulo: int = (number: int, divisor: int) => {
    a: int = number / divisor;
    b: int = a * divisor;
    c: int = number - b;

    return c;

    # TODO(timo): For some reason this statement gave error
    # from resolver where it tried to enter new scope?!?
    # return number - divisor * (number / divisor);
};

main: bool = (argc: int, argv: [int]) => {
    result: bool = true;

    if argc == 0 then result := false;
    else {
        number: int = argv[0];
        i: int = 2;

        while i < number do {
            if modulo(number, i) == 0 then {
                result := false;
                break;
            }

            i := i + 1;
        }
    }

    return result;
};
