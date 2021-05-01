

main: int = () => {
    result: int = 42;

    if result < 100 then {
        if result < 42 then
            result := 1;
        # If these curlybraces werent here, this would go as else-if, but with
        # the curlybraces this is else block with new if statement with new context
        else {
            if result >= 0 then result := 777;
            else result := 0;
        }
    }

    return result;
};
