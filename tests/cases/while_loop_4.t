#
# Author: Timo Mehto
# Date: 2020/04/18

main: int = () => {
    foo: bool = true;
    result: int = 0;
    
    while foo do {
        result := result + 1;

        if result == 42 then break;
    }
    
    return result; 
};
