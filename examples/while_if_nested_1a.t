

main: int = () => {
    result: int = 0;
    i: int = 0;

    while true do {
        if result >= 42 
            then break;
        else
            result := result + i;

        i := i + 1;
    }
    
    # TODO(timo): Missing return statement gave just a segfault
    # from resolver
    return result;
};
