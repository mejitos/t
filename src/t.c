//
// TODO(timo): Filedocstring
//

#include "t.h"
#include <time.h>


static const char* usage = 
    "Usage:\n"
    "    t.sh source_file [options] [flags]\n"
    "Options:\n"
    "    -h, --help: Prints a help screen\n"
    "    -o, --output: Name of the output/program without file extensions\n"
    "    -i, --interpret: Interpret the program with ast tree walker or with IR runner\n"
    "Flags:\n"
    "    --show-summary: Prints a summary of the compilation at the end\n"
    "    --show-symbols: Prints the contents of the symbol table after resolving stage\n"
    "    --show-ir: Prints the instructions of the intermediate representation\n"
    "    --show-asm: Prints the assembly file\n";


void parse_options(struct Options* options, int* argc, char*** argv)
{
    shift(argc, argv); // skip the program name

    if (*argc == 0)
    {
        printf("Error: No source file given\n\n");
        printf(usage);
        exit(1);
    }
    
    while (*argc > 0)
    {
        char* arg = (char*)**argv;

        if (str_equals(arg, "-h") || str_equals(arg, "--help"))
        {
            printf(usage);
            exit(1);
        }
        else if (str_equals(arg, "-o") || str_equals(arg, "--output"))
        {
            shift(argc, argv);
            arg = (char*)**argv;

            if (arg)
                options->program = arg;
            else
            {
                printf("Error: Missing argument for '-o' or '--output'\n");
                exit(1);
            }
        }
        else if (str_equals(arg, "-i") || str_equals(arg, "--interpret"))
        {
            options->interpret = true;
            shift(argc, argv);
            arg = (char*)**argv;

            if (arg && str_equals(arg, "ast"))
                options->interpret_ast = true;
            else if (arg && str_equals(arg, "ir"))
            {
                options->interpret_ir = true;
                printf("Error: IR interpretation is not implemented at the moment\n");
                exit(1);
            }
            else
            {
                printf("Error: Invalid argument for '-i' or '--interpret'\n");
                exit(1);
            }
        }
        else if (str_equals(arg, "--show-summary"))
            options->show_summary = true;
        else if (str_equals(arg, "--show-symbols"))
            options->show_symbols = true;
        else if (str_equals(arg, "--show-ir"))
            options->show_ir = true;
        else if (str_equals(arg, "--show-asm"))
            options->show_asm = true;
        // NOTE(timo): This has to be last option so if there are no flags or
        // other arguments, we just assume it is a source file then
        else if (options->source_file == NULL)
            options->source_file = arg;

        shift(argc, argv);
    }
}


void compile(const char* source, struct Options options)
{
    if (options.show_summary)
        printf("-----===== COMPILING =====-----\n");

    // Lexing
    Lexer lexer;
    clock_t lexing_start;
    clock_t lexing_end;
    double lexing_time;

    if (options.show_summary)
    {
        printf("Lexing...");
        lexing_start = clock();
    }

    lexer_init(&lexer, source);
    lex(&lexer);

    if (options.show_summary)
    {
        lexing_end = clock();
        lexing_time = (double)(lexing_end - lexing_start) * 1000 / (double)CLOCKS_PER_SEC;
    }

    if (lexer.diagnostics->length > 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        print_diagnostics(lexer.diagnostics);
        goto teardown_lexer;
    } 

    if (options.show_summary)
        printf("OK\n");


    // Parsing
    Parser parser;
    clock_t parsing_start;
    clock_t parsing_end;
    double parsing_time;

    if (options.show_summary)
    {
        printf("Parsing...");
        parsing_start = clock();
    }

    parser_init(&parser, lexer.tokens);
    parse(&parser);

    if (options.show_summary)
    {
        parsing_end = clock();
        parsing_time = (double)(parsing_end - parsing_start) * 1000 / (double)CLOCKS_PER_SEC;
    }

    if (parser.diagnostics->length > 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        print_diagnostics(parser.diagnostics);
        goto teardown_parser;
    }

    if (options.show_summary)
        printf("OK\n");


    // Resolving
    hashtable* type_table;
    Resolver resolver;
    clock_t resolving_start;
    clock_t resolving_end;
    double resolving_time;

    if (options.show_summary)
    {
        printf("Resolving...");
        resolving_start = clock();
    }

    type_table = type_table_init();
    resolver_init(&resolver, type_table);
    resolve(&resolver, parser.declarations);

    if (options.show_summary)
    {
        resolving_end = clock();
        resolving_time = (double)(resolving_end - resolving_start) * 1000 / (double)CLOCKS_PER_SEC;
    }

    if (resolver.diagnostics->length > 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        print_diagnostics(resolver.diagnostics);
        goto teardown_resolver;
    }

    if (options.show_summary)
        printf("OK\n");

    if (options.show_symbols)
    {
        printf("-----===== SYMBOLS =====-----\n");
        dump_scope(resolver.global, 0);
        printf("-----===== ||||||| =====-----\n");
    }


    // IR generation
    IR_Generator ir_generator;
    clock_t ir_generating_start;
    clock_t ir_generating_end;
    double ir_generating_time;

    if (options.show_summary)
    {
        printf("IR Generating...");
        ir_generating_start = clock();
    }

    ir_generator_init(&ir_generator, resolver.global);
    ir_generate(&ir_generator, parser.declarations);

    if (options.show_summary)
    {
        ir_generating_end = clock();
        ir_generating_time = (double)(ir_generating_end - ir_generating_start) * 1000 / (double)CLOCKS_PER_SEC;
    }

    if (ir_generator.diagnostics->length > 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        print_diagnostics(ir_generator.diagnostics);
        goto teardown_ir_generator;
    }

    if (options.show_summary)
        printf("OK\n");

    if (options.show_ir)
        dump_instructions(ir_generator.instructions);


    // Code generation
    clock_t code_generating_start;
    clock_t code_generating_end;
    double code_generating_time;

    if (options.show_summary)
    {
        printf("Code generating...");
        code_generating_start = clock();
    }

    Code_Generator code_generator;
    code_generator_init(&code_generator, ir_generator.global, ir_generator.instructions);
    // TODO(timo): ...do I really need this
    char asm_file[64];
    snprintf(asm_file, 64, "%s.asm", options.program);
    code_generator.asm_file = asm_file;
    code_generate(&code_generator);

    if (options.show_summary)
    {
        code_generating_end = clock();
        code_generating_time = (double)(code_generating_end - code_generating_start) * 1000 / (double)CLOCKS_PER_SEC;
    }

    if (code_generator.diagnostics->length > 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        print_diagnostics(code_generator.diagnostics);
        goto teardown_code_generator;
    }

    if (options.show_summary)
        printf("OK\n");

    if (options.show_asm)
    {
        // Cat the created assembly file
        printf("-----===== ASSEMBLY =====-----\n");

        char cat[128];
        snprintf(cat, 128, "cat %s.asm", options.program);
        int cat_error;

        if ((cat_error = system(cat)) != 0)
        {
            printf("Error code on command '%s': %d\n", cat, cat_error);
            goto teardown;
        }

        printf("\n-----===== ||||||| =====-----\n");
    }


    // Assembling
    clock_t assembly_start;
    clock_t assembly_end;
    double assembly_time;

    char assemble[128];
    snprintf(assemble, 128, "nasm -f elf64 -o %s.o %s.asm", options.program, options.program);
    int assemble_error;

    if (options.show_summary)
    {
        printf("Assembling...");
        assembly_start = clock();
    }

    if ((assemble_error = system(assemble)) != 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        printf("Error code on command '%s': %d\n", assemble, assemble_error);
        goto teardown;
    }
   
    if (options.show_summary)
    {
        assembly_end = clock();
        assembly_time = (double)(assembly_end - assembly_start) * 1000 / (double)CLOCKS_PER_SEC;
        printf("OK\n");
    }


    // Linking
    clock_t linker_start;
    clock_t linker_end;
    double linker_time;

    char link[128];
    snprintf(link, 128, "gcc -no-pie -o %s %s.o", options.program, options.program);
    int link_error;

    if (options.show_summary)
    {
        printf("Linking...");
        linker_start = clock();
    }

    if ((link_error = system(link)) != 0)
    {
        if (options.show_summary)
            printf("FAILED\n");

        printf("Error code on command '%s': %d\n", link, link_error);
        goto teardown;
    }

    if (options.show_summary)
    {
        linker_end = clock();
        linker_time = (double)(linker_end - linker_start) * 1000 / (double)CLOCKS_PER_SEC;
        printf("OK\n");
    }

   
    // Teardown
teardown:
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
    char rm_files[128];
    snprintf(rm_files, 128, "rm %s.asm %s.o", options.program, options.program);
    int rm_files_error;
    
    if ((rm_files_error = system(rm_files)) != 0 )
        printf("Error code on command '%s': %d\n", rm_files, rm_files_error);

    if (options.show_summary)
    {
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

}


void compile_from_file(const char* path, struct Options options)
{
    const char* source = read_file(path);

    compile(source, options);

    free((char*)source);
}
