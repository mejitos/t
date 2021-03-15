#
# Author: Timo Mehto
# Date: 2020/03/15

main: int = () => {
    result: int = 0;
    i: int = 0;
    
    # For some reason if the condition is result < 100, it goes into infinite loop
    while i < 100 do {
        result := result + i;
        i := i + 1;
    }

    return result; # 4950
};
