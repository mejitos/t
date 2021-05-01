# Simple example / test for break statement
# 
# Author: Timo Mehto
# Date: 2020/04/10

main: int = () => {
    result: int = 0;
    i: int = 0;
    
    while result < 100 do {
        result := result + i;

        if i == 7 then {
            break;
        } else {
            i := i + 1;
        }
    }

    return result; # 28
};
