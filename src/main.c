#include <stdio.h>
#include <string.h>
#include "runtime/nucleus.h"
#include "parser/parser.h"

void print_usage() {
    printf("Usage: c-lisp [command] [args...]\n");
    printf("Commands:\n");
    printf("  test                           Run internal tests\n");
    printf("  file <input.lisp>              Parse and print a Lisp file\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    // Initialize runtime (1MB heap placeholder)
    lisp_runtime_init(1024 * 1024);

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
        
        printf("Parsed AST Structure:\n");
        lisp_print(ast);
        printf("\n");
    } 
    else if (strcmp(command, "file") == 0) {
        if (argc < 3) {
            printf("Error: Missing file path.\n");
            return 1;
        }
        printf("Parsing file: %s\n", argv[2]);
        LispVal* ast = lisp_parse_file(argv[2]);
        if (ast) {
            printf("AST:\n");
            lisp_print(ast);
            printf("\n");
        }
    } 
    else {
        printf("Unknown command: %s\n", command);
        print_usage();
    }

    lisp_runtime_shutdown();
    return 0;
}