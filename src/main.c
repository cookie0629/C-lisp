#include <stdio.h>
#include <string.h>
#include "runtime/nucleus.h"
#include "runtime/gc.h"
#include "parser/parser.h"
#include "transpiler.h"

void print_usage() {
    printf("Usage: c-lisp [command] [args...]\n");
    printf("Commands:\n");
    printf("  test                           Run internal tests\n");
    printf("  file <input.lisp> <output.c>   Transpile Lisp file\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    size_t object_count = (1024 * 1024) / sizeof(LispVal);
    lisp_runtime_init(object_count);

    const char* command = argv[1];

    if (strcmp(command, "test") == 0) {

        printf("--- Running Internal Test ---\n");

        const char* code =
            "(defun add (a b) (+ a b)) "
            "(print \"Hello World\") "
            "(setq x 42) "
            "'(1 2 3)";

        printf("Input Code:\n%s\n\n", code);

        LispVal* ast = lisp_parse(code);
        gc_push(&ast);

        printf("Parsed AST Structure:\n");
        lisp_print(ast);
        printf("\n");

        gc_pop();

    } 
    else if (strcmp(command, "file") == 0) {

        if (argc < 4) {
            printf("Error: Missing arguments. Usage: file <input.lisp> <output.c>\n");
            return 1;
        }

        printf("Parsing file: %s\n", argv[2]);

        LispVal* ast = lisp_parse_file(argv[2]);
        gc_push(&ast);

        if (ast) {
            printf("AST generated successfully. Beginning transpilation...\n");

            TranspilerContext* ctx = transpiler_create();
            transpile_ast_to_file(ctx, ast, argv[3]);
            transpiler_free(ctx);
        }

        gc_pop();
    } 
    else {
        printf("Unknown command: %s\n", command);
        print_usage();
    }

    lisp_runtime_shutdown();
    return 0;
}