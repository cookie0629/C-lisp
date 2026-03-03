#include "transpiler.h"
#include <string.h>

/* --- Windows Compatibility Fix --- */
#ifdef _MSC_VER
    #define strcasecmp _stricmp
    #define strdup _strdup
#endif

TranspilerContext* transpiler_create() {
    TranspilerContext* ctx = (TranspilerContext*)malloc(sizeof(TranspilerContext));
    ctx->global_defs = builder_create();
    ctx->main_body = builder_create();
    
    // Add nucleus runtime inclusion to the top of generated file
    builder_append(ctx->global_defs, "#include \"runtime/nucleus.h\"\n");
    builder_append(ctx->global_defs, "#include <stdio.h>\n\n");
    
    return ctx;
}

void transpiler_free(TranspilerContext* ctx) {
    if (ctx) {
        builder_free(ctx->global_defs);
        builder_free(ctx->main_body);
        free(ctx);
    }
}

// Forward declaration of handlers
static char* handle_math(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_print(TranspilerContext* ctx, LispVal* args);

// Recursively translate a single Lisp expression to C code
char* transpile_expression(TranspilerContext* ctx, LispVal* expr) {
    if (!expr) return strdup("NULL");

    CodeBuilder* local_cb = builder_create();
    char* result = NULL;

    switch (expr->type) {
        case LISP_INT:
            builder_append(local_cb, "lisp_int(%lld)", expr->data.int_val);
            break;
            
        case LISP_FLOAT:
            builder_append(local_cb, "lisp_float(%f)", expr->data.float_val);
            break;
            
        case LISP_STRING:
            builder_append(local_cb, "lisp_string(\"%s\")", expr->data.str_val);
            break;
            
        case LISP_SYMBOL:
            // In a real transpiler, we need to map Lisp variables to C LispVal* variables
            builder_append(local_cb, "%s", expr->data.str_val);
            break;
            
        case LISP_CONS: {
            LispVal* head = expr->data.cons.car;
            LispVal* tail = expr->data.cons.cdr;

            if (head->type == LISP_SYMBOL) {
                const char* func_name = head->data.str_val;

                // Simple Math Handlers
                if (strcmp(func_name, "+") == 0 || strcmp(func_name, "-") == 0 ||
                    strcmp(func_name, "*") == 0 || strcmp(func_name, "/") == 0) {
                    char* math_code = handle_math(ctx, func_name, tail);
                    builder_append(local_cb, "%s", math_code);
                    free(math_code);
                } 
                // Print Handler
                else if (strcasecmp(func_name, "print") == 0) {
                    char* print_code = handle_print(ctx, tail);
                    builder_append(local_cb, "%s", print_code);
                    free(print_code);
                }
                else {
                    // Default function call handler logic
                    builder_append(local_cb, "/* Unhandled call: %s */ lisp_alloc(LISP_NIL)", func_name);
                }
            }
            break;
        }
        
        default:
            builder_append(local_cb, "lisp_alloc(LISP_NIL)");
            break;
    }

    result = strdup(builder_get(local_cb));
    builder_free(local_cb);
    return result;
}

// Handler for Math Operations (+ a b ...)
// Note: We need a C helper function in nucleus.c to add two LispVals, 
// but for simplicity in Stage 2 start, we generate naive inline C math.
static char* handle_math(TranspilerContext* ctx, const char* op, LispVal* args) {
    // Note: For stage 2 prototype, we just translate (+ 1 2) to lisp_int(1 + 2)
    // A robust transpiler would call a runtime function: lisp_add(val1, val2)
    CodeBuilder* cb = builder_create();
    
    builder_append(cb, "lisp_int(");
    
    LispVal* current = args;
    bool first = true;
    while (current && current->type == LISP_CONS) {
        if (!first) builder_append(cb, " %s ", op);
        
        char* arg_code = transpile_expression(ctx, current->data.cons.car);
        // Extremely naive: assuming the argument generated code returns a LispVal*,
        // we extract the integer data directly for the C math operator.
        builder_append(cb, "(%s)->data.int_val", arg_code);
        free(arg_code);
        
        current = current->data.cons.cdr;
        first = false;
    }
    
    builder_append(cb, ")");
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

// Handler for (print expr)
static char* handle_print(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    
    CodeBuilder* cb = builder_create();
    char* arg_code = transpile_expression(ctx, args->data.cons.car);
    
    // We utilize the runtime's lisp_print function, then print a newline
    builder_append(cb, "(lisp_print(%s), printf(\"\\n\"), lisp_alloc(LISP_T))", arg_code);
    
    free(arg_code);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

// Process the whole top-level AST
void transpile_ast_to_file(TranspilerContext* ctx, LispVal* ast, const char* out_filename) {
    // Iterate through top-level expressions
    LispVal* current = ast;
    while (current && current->type == LISP_CONS) {
        LispVal* expr = current->data.cons.car;
        
        char* expr_code = transpile_expression(ctx, expr);
        
        // Output expressions as statements in the main body
        builder_append(ctx->main_body, "    %s;\n", expr_code);
        free(expr_code);
        
        current = current->data.cons.cdr;
    }

    // Assemble the final C file
    FILE* out = fopen(out_filename, "w");
    if (!out) {
        fprintf(stderr, "Could not open %s for writing\n", out_filename);
        return;
    }

    fprintf(out, "// --- GENERATED C-LISP CODE ---\n\n");
    fprintf(out, "%s\n", builder_get(ctx->global_defs));
    fprintf(out, "int main() {\n");
    fprintf(out, "    lisp_runtime_init(1024 * 1024);\n\n");
    fprintf(out, "%s\n", builder_get(ctx->main_body));
    fprintf(out, "    lisp_runtime_shutdown();\n");
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");

    fclose(out);
    printf("Transpilation successful. Output written to %s\n", out_filename);
}
