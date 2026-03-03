#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "runtime/nucleus.h"
#include "builder.h"

#define MAX_VARS 256

// Context containing our code buckets and symbol table
typedef struct {
    CodeBuilder* global_defs; // For #includes and generated C functions
    CodeBuilder* main_body;   // For statements that run inside main()
    
    // Simple symbol table to track declared global variables
    char* declared_vars[MAX_VARS];
    int var_count;
} TranspilerContext;

TranspilerContext* transpiler_create();
void transpiler_free(TranspilerContext* ctx);

void transpile_ast_to_file(TranspilerContext* ctx, LispVal* ast, const char* out_filename);
char* transpile_expression(TranspilerContext* ctx, LispVal* expr);

#endif // TRANSPILER_H