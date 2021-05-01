

main: int = () => {
    result: int = 42;

    if result < 100 then {
        if result == 42 then
            result := 1;
        else
            result := 0;
    }

    return result;
};
