# Program which calculates x-coordinate of the intersection of lines if
# the lines intersect. 0 will be returned in case of no intersection.
#
# Author: Timo Mehto
# Date: 2020/05/12


# Checks if two lines has the same slope.
#
# Returns
#       Value true if the lines has same slope, otherwise false.
same_slopes: bool = (line1_coef_a: int, line1_coef_b: int, line2_coef_a: int, line2_coef_b: int) => {
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


# Computes the y-coordinate of the intersection of two lines.
#
# Returns
#       The y-coordinate of the intersection of two lines.
y_intersect: int = (line1_coef_a: int, line1_coef_b: int, line1_coef_c: int, line2_coef_a: int, line2_coef_b: int, line2_coef_c: int) => {
    return (line1_coef_a * line2_coef_c - line2_coef_a * line1_coef_c) / (line1_coef_b * line2_coef_a - line2_coef_b * line1_coef_a);
};


main: int = (argc: int, argv: [int]) => {
    result: int = 0;

    if argc == 8 then {
        # Line 1 (argv[0], argv[1]), (argv[2], argv[3])
        line1_coef_a: int = argv[1] - argv[3];
        line1_coef_b: int = argv[2] - argv[0];
        line1_coef_c: int = argv[0] * argv[3] - argv[2] * argv[1];
        # Line 2 (argv[4], argv[5]), (argv[6], argv[7])
        line2_coef_a: int = argv[5] - argv[7];
        line2_coef_b: int = argv[6] - argv[4];
        line2_coef_c: int = argv[4] * argv[7] - argv[6] * argv[5];
        
        if same_slopes(line1_coef_a, line1_coef_b, line2_coef_a, line2_coef_b) and 
           same_axis_intersections(line1_coef_a, line1_coef_b, line1_coef_c, line2_coef_a, line2_coef_b, line2_coef_c) then 
                result := 0;
        else if same_slopes(line1_coef_a, line1_coef_b, line2_coef_a, line2_coef_b) then
            result := 0;
        else
            result := y_intersect(line1_coef_a, line1_coef_b, line1_coef_c, line2_coef_a, line2_coef_b, line2_coef_c);
    }

    return result;
};
