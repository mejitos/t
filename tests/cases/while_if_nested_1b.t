

main: int = () => {
    i: int = 0;

    while true do {
        if i >= 42 then break;
        i := i + 1;
    }
    
    # TODO(timo): Missing return statement gave just a segfault
    # from resolver
    return i;
};
