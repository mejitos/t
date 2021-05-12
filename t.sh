#!/bin/bash

SRC_DIR=./src
BUILD_DIR=./build
EXECUTABLE=t
TEST_BUILD_DIR=./tests/build
TEST_EXECUTABLE=t_test
COMMAND=$1
USAGE="
Usage: t.sh <command> [options] [flags]

Commands:
    help:       Show the help/usage message
    build:      Builds the compiler in the build directory
    buildtest:  Builds the compiler with the test modules in the test build directory
    run:        Builds the compiler and runs the compiler with passed options and flags
    test:       Builds the compiler in the tests directory and runs the tests
    vgrun:      Command run but with Valgrind analysis. Valgrind report goes to file vglog.
    vgtest:     Command test but with Valgrind analysis. Valgrind report goes to file vglog.
"

if [ $# -le 0 ]; then
    echo -e "Error: Missing or invalid command\n$USAGE"
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
    [ -f "$2" ] && gcc -O3 -o $3 $4
}


# Run different commands based on passed argument
case $COMMAND in
    help )
        echo -e "$USAGE"
        exit 0;;
    build )
        build "$BUILD_DIR" "$SRC_DIR/main.c" "$BUILD_DIR/$EXECUTABLE" "$SRC_DIR/*.c"
        exit 0;;
    buildtest )
        build "$TEST_BUILD_DIR" "tests/main.c" "$TEST_BUILD_DIR/$TEST_EXECUTABLE" "tests/*.c 
                                                                                   src/t.c 
                                                                                   src/lexer.c 
                                                                                   src/scope.c 
                                                                                   src/array.c 
                                                                                   src/parser.c 
                                                                                   src/resolver.c 
                                                                                   src/interpreter.c 
                                                                                   src/instruction.c 
                                                                                   src/ir_generator.c 
                                                                                   src/ir_runner.c 
                                                                                   src/code_generator.c 
                                                                                   src/stringbuilder.c 
                                                                                   src/ast.c 
                                                                                   src/symbol.c 
                                                                                   src/type.c 
                                                                                   src/value.c 
                                                                                   src/token.c 
                                                                                   src/memory.c 
                                                                                   src/common.c 
                                                                                   src/diagnostics.c"
        exit 0;;
    run )
        build "$BUILD_DIR" "$SRC_DIR/main.c" "$BUILD_DIR/$EXECUTABLE" "$SRC_DIR/*.c"
        $BUILD_DIR/$EXECUTABLE "${@:2}"
        exit 0;;
    test )
        build "$TEST_BUILD_DIR" "tests/main.c" "$TEST_BUILD_DIR/$TEST_EXECUTABLE" "tests/*.c 
                                                                                   src/t.c 
                                                                                   src/lexer.c 
                                                                                   src/scope.c 
                                                                                   src/array.c 
                                                                                   src/parser.c 
                                                                                   src/resolver.c 
                                                                                   src/interpreter.c 
                                                                                   src/instruction.c 
                                                                                   src/ir_generator.c 
                                                                                   src/ir_runner.c 
                                                                                   src/code_generator.c 
                                                                                   src/stringbuilder.c 
                                                                                   src/ast.c 
                                                                                   src/symbol.c 
                                                                                   src/type.c 
                                                                                   src/value.c 
                                                                                   src/token.c 
                                                                                   src/memory.c 
                                                                                   src/common.c 
                                                                                   src/diagnostics.c"
        $TEST_BUILD_DIR/$TEST_EXECUTABLE "${@:2}"
        exit 0;;
    vgrun )
        build "$BUILD_DIR" "$SRC_DIR/main.c" "$BUILD_DIR/$EXECUTABLE" "$SRC_DIR/*.c"
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="vglog" --verbose $BUILD_DIR/$EXECUTABLE "${@:2}"
        exit 0;;
    vgtest )
        build "$TEST_BUILD_DIR" "tests/main.c" "$TEST_BUILD_DIR/$TEST_EXECUTABLE" "tests/*.c 
                                                                                   src/t.c 
                                                                                   src/lexer.c 
                                                                                   src/scope.c 
                                                                                   src/array.c 
                                                                                   src/parser.c 
                                                                                   src/resolver.c 
                                                                                   src/interpreter.c 
                                                                                   src/instruction.c 
                                                                                   src/ir_generator.c 
                                                                                   src/ir_runner.c 
                                                                                   src/code_generator.c 
                                                                                   src/stringbuilder.c 
                                                                                   src/ast.c 
                                                                                   src/symbol.c 
                                                                                   src/type.c 
                                                                                   src/value.c 
                                                                                   src/token.c 
                                                                                   src/memory.c 
                                                                                   src/common.c
                                                                                   src/diagnostics.c"
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="vglog" --verbose $TEST_BUILD_DIR/$TEST_EXECUTABLE "${@:2}"
        exit 0;;
esac

echo -e "Error: Invalid command\n$USAGE"
