# The T programming language

This is a course project for the course TIES448 in the university of Jyväskylä.
Goal of the course was learn about compiler techniques and principles by creating
a compiler for own source language.


- [Requirements](#requirements)
- [Getting the program](#getting-the-program)
- [Usage](#usage)
- [Compiler options and flags](#compiler-options-and-flags)
- [Test options and flags](#test-options-and-flags)


## Requirements

### GCC 10.2.0 

Probably any version from 2000's works though.


### Linux operating system

Program is not tested on Windows (it's developed on Linux) and I'm pretty sure 
it won't work on Windows at all. 


### Valgrind 3.x.x

Valgrind is not necessary for the program per se, but it can be used to analyze
the program for errors and memory leaks.


## Getting the program

```
mkdir t && cd t
git clone https://github.com/mejitos/t.git .
```


## Usage

There is convenience shellscript `t.sh` for building and testing the compiler.
All of the commands assumes that the current working directory is the root
directory (=same directory as the script).

```
t.sh <command> [compiler_options] [compiler_flags]
```

### [command] `help`

Prints the help/usage screen.

### [command] `build`

Builds the compiler to the `build` directory by the name `t.exe`. Compiler will
be built with command

```
gcc -O3 -o ./build/t src/*.c
```

The `build` directory will be created by the script if it does not exist.

### [command] `run`

Builds the compiler in the build directory and runs the compiler with passed 
arguments, options and flags.

### [command] `test`

Builds the compiler with the test modules to the `tests/build` directory and 
runs all tests with passed options and flags.

### [command] `vgtest`

Same as command `test` but it will be run through Valgrind. The Valgrind report 
will go to the file `vglog`

### [command] `vgrun`

Same as command `run` but it will be run through Valgrind. The Valgrind report 
will go to the file `vglog`


## Compiler options and flags

Compiler has one mandatory argument: path to the source file being compiled.
Other options and flags are optional. Compiler can be used after building it
by using the `t.sh` script with command

```
./t.sh <run|vgrun> <source_file> [options] [flags]
```

or by using the built compiler directly from the build directory

```
./build/t <source_file> [options] [flags]
```


### [option] `-h, --help`

Prints the help/usage screen and exits the program.

### [option] `-o <arg>, --ouput <arg>`

Name of the output/program.

### [option] `-i <arg>, --interpret <arg>`

Interpret the program with selected interpreter.

- #### [arg] `ast` **NOT_IMPLEMENTED**
    - Interpret with AST walker
- #### [arg] `ir` **NOT_IMPLEMENTED**
    - Interpret with intermediate code interpreter 


### [flag] `--show-summary`

Prints a summary of the compilation at the end.

### [flag] `--show-symbols`

Prints the contents of the symbol table after resolving stage.

### [flag] `--show-ir`

Prints the generated intermediate code.

### [flag] `--show-asm`

Prints the generated assembly file.


## Test options and flags

Tests can be run by using the `t.sh` script

```
./t.sh test [options] [flags]
```

If no options or flags are passed, all tests will be run and all info will
be shown in the screen.

### [option] `-h, --help`

Prints the help/usage screen and exits the program.

### [option] `-m <arg>, --module <arg>`

Explicitly selected test module to be run. Multiple modules can be selected
by passing the option multiple times. Option takes one argument at a time.

Available test modules and valid arguments:

- #### [arg] `lexer`
    - Run tests for lexer.
- #### [arg] `parser`
    - Run tests for parser.
- #### [arg] `resolver`
    - Run tests for resolver.
- #### [arg] `interpreter`
    - Run tests for AST interpreter.
- #### [arg] `irgenerator`
    - Run tests for intermediate code generator.
- #### [arg] `compiler`
    - Run tests for code generator / compiler.

### [flag] `--hide-cases`

Hides the descriptions and results of individual test cases.

### [flag] `--hide-errors`

Hides the collected errors from the output.

### [flag] `--hide-summary`

Hides the test summary from the output.
