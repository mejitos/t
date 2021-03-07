#!/bin/bash

SRC_DIR=./src
BUILD_DIR=./build
EXECUTABLE=t.exe
TEST_BUILD_DIR=./tests/build
TEST_EXECUTABLE=t_test.exe
COMMAND=$1
USAGE="
Usage: t.sh [COMMAND]

Commands:
    help:       Show the help/usage message
    build:      Builds the compiler in the build directory
    run:        Builds the compiler and runs the created executable
    test:       Builds the compiler in the tests directory and runs the tests
    vgrun:      Builds the compiler and runs the program with Valgrind analysis
    vgtest:     Builds the compiler in the testst directory and runs the tests with Valgrind analysis
"

if [ $# -ne 1 ]; then
    echo -e "Invalid number of arguments\n$USAGE"
    exit 1
fi


# Argument 1: Build directory
# Argument 2: Entry point of the build (main.c)
# Argument 3: Executable path
# Argument 4: Source files
build () {
    # Check if the build directory exists and if it doesn't, create one
    [ -d "$1" ] || mkdir "$1"

    # Compile the source code to executable if there is entry point created
    [ -f "$2" ] && gcc -O3 -g -o $3 $4
}


# Run different commands based on passed argument
# TODO(timo): Add command for valgrind analysis
case $COMMAND in
    help )
        echo -e "$USAGE"
        exit 0;;
    test )
        build "$TEST_BUILD_DIR" "tests/main.c" "$TEST_BUILD_DIR/$TEST_EXECUTABLE" "tests/*.c src/lexer.c src/scope.c src/array.c src/parser.c src/resolver.c src/interpreter.c src/instruction.c src/ir_generator.c src/ir_runner.c src/stringbuilder.c src/ast.c src/symbol.c src/type.c src/token.c src/memory.c src/diagnostics.c"
        $TEST_BUILD_DIR/$TEST_EXECUTABLE
        exit 0;;
    run )
        build "$BUILD_DIR" "$SRC_DIR/main.c" "$BUILD_DIR/$EXECUTABLE" "$SRC_DIR/*.c"
        $BUILD_DIR/$EXECUTABLE
        exit 0;;
    build )
        build "$BUILD_DIR" "$SRC_DIR/main.c" "$BUILD_DIR/$EXECUTABLE" "$SRC_DIR/*.c"
        exit 0;;
    vgrun )
        build "$BUILD_DIR" "$SRC_DIR/main.c" "$BUILD_DIR/$EXECUTABLE" "$SRC_DIR/*.c"
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose $BUILD_DIR/$EXECUTABLE
        exit 0;;
    vgtest )
        build "$TEST_BUILD_DIR" "tests/main.c" "$TEST_BUILD_DIR/$TEST_EXECUTABLE" "tests/*.c src/lexer.c src/scope.c src/array.c src/parser.c src/resolver.c src/interpreter.c src/instruction.c src/ir_generator.c src/ir_runner.c src/stringbuilder.c src/ast.c src/symbol.c src/type.c src/token.c src/memory.c src/diagnostics.c"
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="vglog" --verbose $TEST_BUILD_DIR/$TEST_EXECUTABLE
        exit 0;;
esac

echo -e "Invalid command\n$USAGE"
