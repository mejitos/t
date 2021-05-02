

main: int = (argc: int, argv: [int]) => {
    result: int = 0;
    i: int = 0;
    
    while i < 100 do {
       i := i + 1;
       if i / 10 == 0 then continue;
       result := result + 1;
       break;
    }

    return result + i; # 1 + 10 = 11
};
