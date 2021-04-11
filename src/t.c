#include "t.h"
#include <time.h>


void compile(const char* source, Options options)
{
    // Setup
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator ir_generator;
    Code_Generator code_generator;

    // Lexing
    lexer_init(&lexer, source);
    lex(&lexer);

    // Parsing
    parser_init(&parser, lexer.tokens);
    parse(&parser);

    // Resolving
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    // IR generation
    ir_generator_init(&ir_generator, resolver.global);
    ir_generate(&ir_generator, parser.declarations);

    // Code generation
    code_generator_init(&code_generator, ir_generator.global, ir_generator.instructions);
    code_generate(&code_generator);

    // Assembler
    char* assemble = "nasm -f elf64 -o main.o main.asm";
    int assemble_error;

    if ((assemble_error = system(assemble)) != 0)
    {
        printf("FAILED\n");
        printf("Error code on command '%s': %d\n", assemble, assemble_error);
    }

    // Linker
    char* link = "gcc -no-pie -o main main.o";
    int link_error;

    if ((link_error = system(link)) != 0)
    {
        printf("FAILED\n");
        printf("Error code on command '%s': %d\n", link, link_error);
    }
    
    // Teardown
    ir_generator_free(&ir_generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
    
    // Remove created files
    char* rm_files = "rm main.asm main.o";
    int rm_files_error;
    
    if ((rm_files_error = system(rm_files)) != 0 )
    {
        printf("\n");
        printf("Error code on command '%s': %d\n", rm_files, rm_files_error);
    }
}


// TODO(timo): This is not a smart way of handling different compilation options
// so instead of creating different functions for different options, do something more senseful
void compile_verbose(const char* source, Options options)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator ir_generator;
    Code_Generator code_generator;

    // Lexing part
    clock_t lexing_start = clock();

    lexer_init(&lexer, source);
    lex(&lexer);

    clock_t lexing_end = clock();
    double lexing_time = (double)(lexing_end - lexing_start) * 1000 / (double)CLOCKS_PER_SEC;

    // Parsing part
    clock_t parsing_start = clock();

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    clock_t parsing_end = clock();
    double parsing_time = (double)(parsing_end - parsing_start) * 1000 / (double)CLOCKS_PER_SEC;
    
    // Resolving part
    clock_t resolving_start = clock();

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);
    
    // Dump symbol tables after resolving
    printf("-----===== SCOPES / SYMBOLS =====-----\n");
    dump_scope(resolver.global, 0);
    printf("-----===== |||||||||| =====-----\n");

    clock_t resolving_end = clock();
    double resolving_time = (double)(resolving_end - resolving_start) * 1000 / (double)CLOCKS_PER_SEC;

    // TODO(timo): AST tree interpreter could be run if there is option set for that

    // IR generation
    clock_t ir_generating_start = clock();

    ir_generator_init(&ir_generator, resolver.global);
    ir_generate(&ir_generator, parser.declarations);

    clock_t ir_generating_end = clock();
    double ir_generating_time = (double)(ir_generating_end - ir_generating_start) * 1000 / (double)CLOCKS_PER_SEC;

    // Dump instructions
    dump_instructions(ir_generator.instructions);

    // Dump symbol tables after generating IR
    printf("-----===== SCOPES / SYMBOLS =====-----\n");
    dump_scope(resolver.global, 0);
    printf("-----===== |||||||||| =====-----\n");

    // TODO(timo): IR runner could run/interpret the created IR code if there is option set for that

    // Code generation
    clock_t code_generating_start = clock();
    
    code_generator_init(&code_generator, ir_generator.global, ir_generator.instructions);
    code_generate(&code_generator);

    clock_t code_generating_end = clock();
    double code_generating_time = (double)(code_generating_end - code_generating_start) * 1000 / (double)CLOCKS_PER_SEC;

    // Cat the created assembly file
    printf("-----===== ASSEMBLY =====-----\n");
    printf("\n");

    char* cat_main = "cat main.asm";
    int cat_error;

    if ((cat_error = system(cat_main)) != 0)
    {
        printf("Error code on command '%s': %d\n", cat_main, cat_error);
    }

    printf("\n");
    printf("-----===== ||||||| =====-----\n");

    // Assembling
    printf("Assembling...");

    clock_t assembly_start = clock();

    char* assemble = "nasm -f elf64 -o main.o main.asm";
    int assemble_error;

    if ((assemble_error = system(assemble)) != 0)
    {
        printf("FAILED\n");
        printf("Error code on command '%s': %d\n", assemble, assemble_error);
    }
    else
        printf("OK\n");

    clock_t assembly_end = clock();
    double assembly_time = (double)(assembly_end - assembly_start) * 1000 / (double)CLOCKS_PER_SEC;

    // Linking
    printf("Linking...");

    clock_t linker_start = clock();

    char* link = "gcc -no-pie -o main main.o";
    int link_error;

    if ((link_error = system(link)) != 0)
    {
        printf("FAILED\n");
        printf("Error code on command '%s': %d\n", link, link_error);
    }
    else
        printf("OK\n");

    clock_t linker_end = clock();
    double linker_time = (double)(linker_end - linker_start) * 1000 / (double)CLOCKS_PER_SEC;

    // Teardown
    printf("Teardown...");
    
    ir_generator_free(&ir_generator);
    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);
    
    char* rm_main_asm = "rm main.asm";
    int rm_main_asm_error;
    
    if ((rm_main_asm_error = system(rm_main_asm)) != 0 )
    {
        printf("\n");
        printf("Error code on command '%s': %d\n", rm_main_asm, rm_main_asm_error);
    }

    char* rm_main_o = "rm main.o";
    int rm_main_o_error;

    if ((rm_main_o_error = system(rm_main_o)) != 0 )
    {
        printf("\n");
        printf("Error code on command '%s': %d\n", rm_main_o, rm_main_o_error);
    }

    printf("DONE\n");

    // Compilation summary
    double compilation_time = (lexing_time + parsing_time + resolving_time + ir_generating_time +
                               code_generating_time + assembly_time + linker_time);

    printf("-----===== COMPILATION SUMMARY =====-----\n");
    printf("Total compilation time: %f ms\n", compilation_time);
    printf("    Lexing time:             %f ms\n", lexing_time);
    printf("    Parsing time:            %f ms\n", parsing_time);
    printf("    Resolving time:          %f ms\n", resolving_time);
    printf("    IR generation time:      %f ms\n", ir_generating_time);
    printf("    Code generation time:    %f ms\n", code_generating_time);
    printf("    Assembly time:           %f ms\n", assembly_time);
    printf("    Linking time:            %f ms\n", linker_time);
}


void compile_from_file(const char* path, Options options)
{
    FILE* file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Could not open file '%s'\n", path);
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char* buffer = malloc(file_size * sizeof (char) + 1);
    
    if (buffer == NULL)
    {
        printf("Malloc failed. Not enough memory to read file '%s'\n", path);
        exit(1);
    }

    size_t bytes_read = fread(buffer, sizeof (char), file_size, file);

    if (bytes_read < file_size)
    {
        printf("Could not read file '%s'\n", path);
        exit(1);
    }

    buffer[bytes_read] = 0;
    fclose(file);
    
    if (options.flag_verbose)
        compile_verbose(buffer, options);
    else
        compile(buffer, options);

    free(buffer);
}
