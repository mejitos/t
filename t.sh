#!/bin/bash

SRC_DIR=./src
BUILD_DIR=./build
EXECUTABLE=t.exe
COMMAND=$1
USAGE="
Usage: t.sh [COMMAND]

Commands:
    build: Builds the compiler in the build directory
    run: Builds the compiler and runs the created executable
    test: Builds the compiler in the tests directory and runs the tests
"



if [ $# -ne 1 ]; then
    echo -e "Invalid number of arguments\n$USAGE"
    exit 1
fi

# Check if the build directory exists and if it doesn't, create one
[ -d "$BUILD_DIR" ] || mkdir "$BUILD_DIR"

# Compile the source code to executable if there is entry point created
[ -f "$SRC_DIR/main.c" ] && gcc -g -o $BUILD_DIR/$EXECUTABLE $SRC_DIR/*.c

# Run different commands based on passed argument
case $COMMAND in
    "test" )
        echo "I would run tests, but they are not implemented yet" && exit 0;;
    "run" )
        $BUILD_DIR/$EXECUTABLE && exit 0;;
    "build" )
        exit 0;;
esac

echo -e "Invalid command\n$USAGE"
