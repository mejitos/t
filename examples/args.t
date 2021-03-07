# Return first command line argument passed by user
#
# Author: Timo Mehto
# Date: 2020/03/07

main: int = (argc: int, argv: [int]) => {
    result: int = 0;

    if argc > 0 then {
        result := argv[0];
    }

    return result;
};
