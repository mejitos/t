# Finds the largest number from the passed arguments
#
# Author: Timo Mehto
# Date: 2020/05/02


main: int = (argc: int, argv: [int]) => {
    result: int = -1;
    i: int = 0;

    while i < argc do {
        if argv[i] > result then result := argv[i];
        i := i + 1;
    }
    
    return result;
};
