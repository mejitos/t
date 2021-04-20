# Program that iteratively computes factorial based on user passed argument
#
# Author: Timo Mehto
# Date: 2020/03/07

main: int = (argc: int, argv: [int]) => {
    result: int = 0;
    
    if argc <= 0 then {
        result := 0;
    } else {
        n: int = argv[0];
        factorial: int = 1;
        i: int = 1;

        while i <= n do {
            factorial := factorial * i;
            i := i + 1;
        }

        result := factorial;
    }

    return result;
};
