
main: int = (argc: int, argv: [int]) => {
    result: int = -1;
    i: int = 0;
    
    if argc == 1 then result := argv[0];

    while i < argc do {
        if argv[i] > result then result := argv[i];
        i := i + 1;
    }
    
    return result;
};
