#!/bin/bash

SRC_DIR=./src
BUILD_DIR=./build
EXECUTABLE=t.exe
COMMAND=$1

# Check if the build directory exists and if it doesn't, create one
[ -d "$BUILD_DIR" ] || mkdir "$BUILD_DIR"

# Compile the source code to executable if there is entry point created
[ -f "$SRC_DIR/main.c" ] && gcc -g -o $BUILD_DIR/$EXECUTABLE $SRC_DIR/*.c

# Run different commands based on passed argument
case $COMMAND in
    "test" )
        echo "I would run tests, but they are not implemented yet";;
    "run" )
        $BUILD_DIR/$EXECUTABLE;;
esac
