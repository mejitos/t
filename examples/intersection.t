
same_slopes: bool = (line1_coef_a: int, line1_coef_b: int, line2_coef_a: int, line2_coef_b: int) => {
    # NOTE(timo): There when I used 'line_coef_b' the error was "variable result was expecting bool" or whatever
    # result: bool = line1_coef_a * line2_coef_b == line2_coef_a * line1_coef_b;
    # return result;
    return line1_coef_a * line2_coef_b == line2_coef_a * line1_coef_b;
};

same_axis_intersections: bool = (line1_coef_a: int, line1_coef_b: int, line1_coef_c: int, line2_coef_a: int, line2_coef_b: int, line2_coef_c: int) => {
    result: bool = false;

    if line1_coef_b == 0 and line2_coef_b == 0 then { # lines go with y-axis
        if line1_coef_a * line2_coef_c == line2_coef_a * line1_coef_c then result := true;
        else result := false;
    }
    else if line1_coef_b * line2_coef_c == line2_coef_b * line1_coef_c then result := true;
    else result := false;

    return result;
};

_y_intersect: int = (line1_coef_a: int, line1_coef_b: int, line1_coef_c: int, line2_coef_a: int, line2_coef_b: int, line2_coef_c: int) => {
    # TODO(timo): For some reason I can't just return this monstrosity
    result: int = (line1_coef_a * line2_coef_c - line2_coef_a * line1_coef_c) / (line1_coef_b * line2_coef_2 - line2_coef_b * line1_coef_a);
    return result;
};

_x_intersect: int = (line1_coef_a: int, line1_coef_b: int, line1_coef_c: int, line2_coef_a: int, line2_coef_b: int, line2_coef_c: int) => {
    x_intersect: int = 0;
    y_intersect: int = _y_intersect(line1_coef_a, line1_coef_b, line1_coef_c, line2_coef_a, line2_coef_b, line2_coef_c);

    if line1_coef_a == 0 then
        x_intersect := (-line2_coef_b * y_intersect - line2_coef_c) / line2_coef_a;
    else
        x_intersect := (-line1_coef_b * y_intersect - line1_coef_c) / line1_coef_a;

    return x_intersect;
};

main: bool = (argc: int, argv: [int]) => {
    result: bool = false;

    if argc == 8 then {
        # Line 1 (argv[0], argv[1]), (argv[2], argv[3])
        line1_coef_a: int = argv[1] - argv[3];
        line1_coef_b: int = argv[2] - argv[0];
        line1_coef_c: int = argv[0] * argv[3] - argv[2] * argv[1];
        # Line 2 (argv[4], argv[5]), (argv[6], argv[7])
        line2_coef_a: int = argv[5] - argv[7];
        line2_coef_b: int = argv[6] - argv[4];
        line2_coef_c: int = argv[4] * argv[7] - argv[6] * argv[5];
        
        # wrong function name (same_axis_intersections) just gave straight up segfault
        # resolving the call expression returned none type and the error (else block) couldnt handle it
        # (resolver_call_expression) maybe the expression doesn't have position or symbol does not have identifier or something
        if same_slopes(line1_coef_a, line1_coef_b, line2_coef_a, line2_coef_b) and 
           same_axis_intersections(line1_coef_a, line1_coef_b, line1_coef_c, line2_coef_a, line2_coef_b, line2_coef_c) then 
                result := false;
        else if same_slopes(line1_coef_a, line1_coef_b, line2_coef_a, line2_coef_b) then
            result := false;
        else
            result := true;
    }

    return result;
};
