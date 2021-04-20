#include "t.h"
#include <time.h>


void compile(const char* source, Options options)
{
    if (! options.program)
    {
        printf("Need to pass name of the program\n");
        exit(1);
    }

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

    if (lexer.diagnostics->length > 0)
    {
        print_diagnostics(lexer.diagnostics);
        goto teardown_lexer;
    }

    // Parsing
    parser_init(&parser, lexer.tokens);
    parse(&parser);

    if (parser.diagnostics->length > 0)
    {
        print_diagnostics(parser.diagnostics);
        goto teardown_parser;
    }

    // Resolving
    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    if (resolver.diagnostics->length > 0)
    {
        print_diagnostics(resolver.diagnostics);
        goto teardown_resolver;
    }

    // IR generation
    ir_generator_init(&ir_generator, resolver.global);
    ir_generate(&ir_generator, parser.declarations);

    if (ir_generator.diagnostics->length > 0)
    {
        print_diagnostics(ir_generator.diagnostics);
        goto teardown_ir_generator;
    }

    // Code generation
    code_generator_init(&code_generator, ir_generator.global, ir_generator.instructions);
    
    // TODO(timo): ...do I really need this
    char asm_file[64];
    snprintf(asm_file, 64, "%s.asm", options.program);
    code_generator.asm_file = asm_file;
    code_generate(&code_generator);

    if (code_generator.diagnostics->length > 0)
    {
        print_diagnostics(code_generator.diagnostics);
        goto teardown_code_generator;
    }

    // Assembler
    // char* assemble = "nasm -f elf64 -o main.o main.asm";
    char assemble[128];
    snprintf(assemble, 128, "nasm -f elf64 -o %s.o %s.asm", options.program, options.program);
    int assemble_error;

    if ((assemble_error = system(assemble)) != 0)
    {
        printf("Error code on command '%s': %d\n", assemble, assemble_error);
    }

    // Linker
    // char* link = "gcc -no-pie -o main main.o";
    char link[128];
    snprintf(link, 128, "gcc -no-pie -o %s %s.o", options.program, options.program);
    int link_error;

    if ((link_error = system(link)) != 0)
    {
        printf("Error code on command '%s': %d\n", link, link_error);
    }
    
    // Teardown
teardown_code_generator:
    code_generator_free(&code_generator);
teardown_ir_generator:
    ir_generator_free(&ir_generator);
teardown_resolver:
    resolver_free(&resolver);
    type_table_free(type_table);
teardown_parser:
    parser_free(&parser);
teardown_lexer:
    lexer_free(&lexer);
    
    // Remove created files
    // char* rm_files = "rm main.asm main.o";
    char rm_files[128];
    snprintf(rm_files, 128, "rm %s.asm %s.o", options.program, options.program);
    int rm_files_error;
    
    if ((rm_files_error = system(rm_files)) != 0 )
    {
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

    // IR generation
    clock_t ir_generating_start = clock();

    ir_generator_init(&ir_generator, resolver.global);
    ir_generate(&ir_generator, parser.declarations);

    clock_t ir_generating_end = clock();
    double ir_generating_time = (double)(ir_generating_end - ir_generating_start) * 1000 / (double)CLOCKS_PER_SEC;

    if (ir_generator.diagnostics->length > 0)
    {
        print_diagnostics(ir_generator.diagnostics);
        goto teardown_ir_generator;
    }

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

    // TODO(timo): ...do I really need this
    char asm_file[64];
    snprintf(asm_file, 64, "%s.asm", options.program);
    code_generator.asm_file = asm_file;

    code_generate(&code_generator);

    clock_t code_generating_end = clock();
    double code_generating_time = (double)(code_generating_end - code_generating_start) * 1000 / (double)CLOCKS_PER_SEC;

    if (code_generator.diagnostics->length > 0)
    {
        print_diagnostics(code_generator.diagnostics);
        goto teardown_code_generator;
    }

    // Cat the created assembly file
    printf("-----===== ASSEMBLY =====-----\n");
    printf("\n");

    char cat[128];
    snprintf(cat, 128, "cat %s.asm", options.program);
    int cat_error;

    if ((cat_error = system(cat)) != 0)
    {
        printf("Error code on command '%s': %d\n", cat, cat_error);
    }

    printf("\n");
    printf("-----===== ||||||| =====-----\n");

    // Assembling
    printf("Assembling...");

    clock_t assembly_start = clock();

    char assemble[128];
    snprintf(assemble, 128, "nasm -f elf64 -o %s.o %s.asm", options.program, options.program);
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

    char link[128];
    snprintf(link, 128, "gcc -no-pie -o %s %s.o", options.program, options.program);
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
   
teardown_code_generator:
    code_generator_free(&code_generator);
teardown_ir_generator:
    ir_generator_free(&ir_generator);
teardown_resolver:
    resolver_free(&resolver);
    type_table_free(type_table);
teardown_parser:
    parser_free(&parser);
teardown_lexer:
    lexer_free(&lexer);
    
    char rm_files[128];
    // char* rm_main_asm = "rm main.asm";
    snprintf(rm_files, 128, "rm %s.asm %s.o", options.program, options.program);
    int rm_files_error;
    
    if ((rm_files_error = system(rm_files)) != 0 )
    {
        printf("\n");
        printf("Error code on command '%s': %d\n", rm_files, rm_files_error);
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
