

main: int = () => {
    result: int = 0;
    i: int = 0;

    while true do {
        if result < 100 then {
            2 + 2;

            if result >= 42 then 
                break;
            else
                result := result + i;
            
            # i := i + (i / 2); # gives error from parser, expecting type specifier after i inside parenthesis
            # i + 1; # gives error from parser, expecting a type specifier
            # the problem is when we try to separate the declarations from statements, we dont
            # take into account the arithmetic operators, only assignments/declarations
            1 + 1;
        }

        i := i + 1;
    }

    return result;
};
