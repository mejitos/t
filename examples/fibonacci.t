# Program that iteratively computes fibonacci numbers based on user passed argument
#
# Author: Timo Mehto
# Date: 2020/03/07

# Returns the nth term of the fibonacci sequence
#
# Arguments
#       n: nth term
fibonacci: int = (n: int) => {
    a: int = 0;
    b: int = 0;
    i: int = 0;

    while i < n do {
        a = b;
        b = a + b;
        i := i + 1;
    }

    return a;
};

# Main program
main: int = (argc: int, argv: [int]) => {
    result: int = 0;
    
    if argc <= 0 then {
        result := 0;
    } else {
        # n: int = argv[0];
        n: int = 10;
        result := fibonacci(n);
    }

    return result;
};
