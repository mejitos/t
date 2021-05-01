#
# Author: Timo Mehto
# Date: 2020/03/15

main: int = () => {
    result: int = 0;
    i: int = 0;
    
    while result < 100 do {
        result := result + i;
        i := i + 1;
    }

    return result; # 105
};
