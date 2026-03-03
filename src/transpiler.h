#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "runtime/nucleus.h"
#include "builder.h"

// Context containing our code buckets
typedef struct {
    CodeBuilder* global_defs; // For #includes and generated C functions
    CodeBuilder* main_body;   // For statements that run inside main()
} TranspilerContext;

// Initialize context
TranspilerContext* transpiler_create();
void transpiler_free(TranspilerContext* ctx);

// Generate C code from the AST into the target file
void transpile_ast_to_file(TranspilerContext* ctx, LispVal* ast, const char* out_filename);

// Core translation function for a single expression.
// Returns the C code representation of the expression (e.g., "1 + 2").
// The caller is responsible for freeing the returned string.
char* transpile_expression(TranspilerContext* ctx, LispVal* expr);

#endif // TRANSPILER_H