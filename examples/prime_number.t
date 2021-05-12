# Program that checks if number passed as argument is a prime number
#
# Author: Timo Mehto
# Date: 2020/05/02


# Simple modulo function to get the remainder since the language does not
# have a modulo operator
#
# Arguments
#       divident: Number being divided.
#       divisor: What the number is being divided with.
# Returns
#       The remainder of the division between divident and divisor.
modulo: int = (divident: int, divisor: int) => {
    return divident - divisor * (divident / divisor);
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
