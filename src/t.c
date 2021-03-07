#include "t.h"
#include <time.h>


// TODO(timo): Functionality to take somekind of options as argument
void compile(const char* source)
{
    Lexer lexer;
    Parser parser;
    hashtable* type_table;
    Resolver resolver;
    IR_Generator ir_generator;

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

    // TODO(timo): Dump symbol tables

    clock_t resolving_end = clock();
    double resolving_time = (double)(resolving_end - resolving_start) * 1000 / (double)CLOCKS_PER_SEC;

    // TODO(timo): AST tree interpreter could be run if there is option set for that

    // IR generation
    clock_t ir_generating_start = clock();

    ir_generator_init(&ir_generator);
    ir_generate(&ir_generator, parser.declarations);

    // Dump instructions
    dump_instructions(ir_generator.instructions);

    clock_t ir_generating_end = clock();
    double ir_generating_time = (double)(ir_generating_end - ir_generating_start) * 1000 / (double)CLOCKS_PER_SEC;

    // TODO(timo): IR runner could run/interpret the created IR code if there is option set for that

    // TODO(timo): Code generation

    // TODO(timo): Assembling

    // TODO(timo): Linking

    // Teardown
    printf("Teardown...");

    resolver_free(&resolver);
    type_table_free(type_table);
    parser_free(&parser);
    lexer_free(&lexer);

    printf("DONE\n");

    // Compilation summary
    double compilation_time = lexing_time + parsing_time + resolving_time + ir_generating_time;

    printf("-----===== COMPILATION SUMMARY =====-----\n");
    printf("Total compilation time: %f ms\n", compilation_time);
    printf("    Lexing time:           %f ms\n", lexing_time);
    printf("    Parsing time:          %f ms\n", parsing_time);
    printf("    Resolving time:        %f ms\n", resolving_time);
    printf("    IR generation time:    %f ms\n", ir_generating_time);
}


void compile_from_file(const char* path)
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
    
    compile(buffer);

    free(buffer);
}
