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
    ctx->var_count = 0;
    
    builder_append(ctx->global_defs, "#include \"runtime/nucleus.h\"\n");
    builder_append(ctx->global_defs, "#include <stdio.h>\n\n");
    
    return ctx;
}

void transpiler_free(TranspilerContext* ctx) {
    if (ctx) {
        builder_free(ctx->global_defs);
        builder_free(ctx->main_body);
        for (int i = 0; i < ctx->var_count; i++) {
            free(ctx->declared_vars[i]);
        }
        free(ctx);
    }
}

// Check if a variable is already declared
static bool is_var_declared(TranspilerContext* ctx, const char* name) {
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->declared_vars[i], name) == 0) return true;
    }
    return false;
}

// Mark a variable as declared
static void declare_var(TranspilerContext* ctx, const char* name) {
    if (!is_var_declared(ctx, name) && ctx->var_count < MAX_VARS) {
        ctx->declared_vars[ctx->var_count++] = strdup(name);
        // Add global declaration to the C file
        builder_append(ctx->global_defs, "LispVal* lisp_var_%s = NULL;\n", name);
    }
}

// Forward declarations of Handlers
static char* handle_math(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_print(TranspilerContext* ctx, LispVal* args);
static char* handle_setq(TranspilerContext* ctx, LispVal* args);
static char* handle_defun(TranspilerContext* ctx, LispVal* args);
static char* handle_func_call(TranspilerContext* ctx, const char* func_name, LispVal* args);
static char* handle_if(TranspilerContext* ctx, LispVal* args);
static char* handle_logic(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_list_ops(TranspilerContext* ctx, const char* op, LispVal* args);

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
            // All Lisp variables are prefixed with lisp_var_ in C
            builder_append(local_cb, "lisp_var_%s", expr->data.str_val);
            break;
        case LISP_CONS: {
            LispVal* head = expr->data.cons.car;
            LispVal* tail = expr->data.cons.cdr;

            if (head->type == LISP_SYMBOL) {
                const char* func_name = head->data.str_val;

                if (strcmp(func_name, "+") == 0 || strcmp(func_name, "-") == 0 ||
                    strcmp(func_name, "*") == 0 || strcmp(func_name, "/") == 0) {
                    char* code = handle_math(ctx, func_name, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                } 
                else if (strcasecmp(func_name, "print") == 0) {
                    char* code = handle_print(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "setq") == 0) {
                    char* code = handle_setq(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "defun") == 0) {
                    char* code = handle_defun(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "if") == 0) {
                    char* code = handle_if(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcmp(func_name, ">") == 0 || strcmp(func_name, "<") == 0 || 
                         strcmp(func_name, "=") == 0 || strcmp(func_name, ">=") == 0 || 
                         strcmp(func_name, "<=") == 0) {
                    char* code = handle_logic(ctx, func_name, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "car") == 0 || strcasecmp(func_name, "cdr") == 0 ||
                         strcasecmp(func_name, "cons") == 0 || strcasecmp(func_name, "quote") == 0) {
                    char* code = handle_list_ops(ctx, func_name, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else {
                    // Custom function call
                    char* code = handle_func_call(ctx, func_name, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
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

// Handler: (setq var_name value)
static char* handle_setq(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    
    LispVal* sym_node = args->data.cons.car;
    LispVal* val_node = args->data.cons.cdr->data.cons.car;
    
    if (sym_node->type != LISP_SYMBOL) return strdup("lisp_alloc(LISP_NIL)");
    
    const char* var_name = sym_node->data.str_val;
    declare_var(ctx, var_name); // Ensure it's in global_defs
    
    char* val_code = transpile_expression(ctx, val_node);
    
    CodeBuilder* cb = builder_create();
    // C assignment expression: (lisp_var_X = value)
    builder_append(cb, "(lisp_var_%s = %s)", var_name, val_code);
    
    free(val_code);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

// Handler: (defun func_name (args...) body...)
static char* handle_defun(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    
    LispVal* name_node = args->data.cons.car;
    LispVal* params_node = args->data.cons.cdr->data.cons.car;
    LispVal* body_node = args->data.cons.cdr->data.cons.cdr->data.cons.car; // Simplified: assumes 1 body expr
    
    const char* func_name = name_node->data.str_val;
    
    CodeBuilder* func_cb = builder_create();
    builder_append(func_cb, "LispVal* lisp_func_%s(", func_name);
    
    // Parse parameters: (a b c) -> LispVal* lisp_var_a, LispVal* lisp_var_b
    LispVal* curr_param = params_node;
    bool first = true;
    while (curr_param && curr_param->type == LISP_CONS) {
        if (!first) builder_append(func_cb, ", ");
        builder_append(func_cb, "LispVal* lisp_var_%s", curr_param->data.cons.car->data.str_val);
        curr_param = curr_param->data.cons.cdr;
        first = false;
    }
    if (first) builder_append(func_cb, "void"); // No args
    
    builder_append(func_cb, ") {\n");
    
    // Parse body (return the evaluated expression)
    char* body_code = transpile_expression(ctx, body_node);
    builder_append(func_cb, "    return %s;\n", body_code);
    free(body_code);
    
    builder_append(func_cb, "}\n\n");
    
    // Write the function to global definitions
    builder_append(ctx->global_defs, "%s", builder_get(func_cb));
    builder_free(func_cb);
    
    // In Lisp, defun returns the symbol of the function name
    CodeBuilder* res_cb = builder_create();
    builder_append(res_cb, "lisp_symbol(\"%s\")", func_name);
    char* res = strdup(builder_get(res_cb));
    builder_free(res_cb);
    return res;
}

// Handler: custom function call (my_func a b)
static char* handle_func_call(TranspilerContext* ctx, const char* func_name, LispVal* args) {
    CodeBuilder* cb = builder_create();
    builder_append(cb, "lisp_func_%s(", func_name);
    
    LispVal* curr = args;
    bool first = true;
    while (curr && curr->type == LISP_CONS) {
        if (!first) builder_append(cb, ", ");
        char* arg_code = transpile_expression(ctx, curr->data.cons.car);
        builder_append(cb, "%s", arg_code);
        free(arg_code);
        curr = curr->data.cons.cdr;
        first = false;
    }
    
    builder_append(cb, ")");
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

// ... (keep handle_math and handle_print as they were previously)
static char* handle_math(TranspilerContext* ctx, const char* op, LispVal* args) {
    CodeBuilder* cb = builder_create();
    builder_append(cb, "lisp_int(");
    LispVal* current = args;
    bool first = true;
    while (current && current->type == LISP_CONS) {
        if (!first) builder_append(cb, " %s ", op);
        char* arg_code = transpile_expression(ctx, current->data.cons.car);
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

static char* handle_print(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    CodeBuilder* cb = builder_create();
    char* arg_code = transpile_expression(ctx, args->data.cons.car);
    builder_append(cb, "(lisp_print(%s), printf(\"\\n\"), lisp_alloc(LISP_T))", arg_code);
    free(arg_code);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

void transpile_ast_to_file(TranspilerContext* ctx, LispVal* ast, const char* out_filename) {
    LispVal* current = ast;
    while (current && current->type == LISP_CONS) {
        LispVal* expr = current->data.cons.car;
        char* expr_code = transpile_expression(ctx, expr);
        builder_append(ctx->main_body, "    %s;\n", expr_code);
        free(expr_code);
        current = current->data.cons.cdr;
    }

    FILE* out = fopen(out_filename, "w");
    if (!out) return;

    fprintf(out, "// --- GENERATED C-LISP CODE ---\n\n");
    fprintf(out, "%s\n", builder_get(ctx->global_defs));
    fprintf(out, "int main() {\n");
    fprintf(out, "    lisp_runtime_init(1024 * 1024);\n\n");
    fprintf(out, "%s\n", builder_get(ctx->main_body));
    fprintf(out, "    lisp_runtime_shutdown();\n");
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
    fclose(out);
}

// Handler: (if condition then-expr else-expr)
static char* handle_if(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");

    LispVal* cond_node = args->data.cons.car;
    LispVal* then_node = args->data.cons.cdr ? args->data.cons.cdr->data.cons.car : NULL;
    LispVal* else_node = (args->data.cons.cdr && args->data.cons.cdr->data.cons.cdr) ? 
                          args->data.cons.cdr->data.cons.cdr->data.cons.car : NULL;

    char* cond_code = transpile_expression(ctx, cond_node);
    char* then_code = then_node ? transpile_expression(ctx, then_node) : strdup("lisp_alloc(LISP_NIL)");
    char* else_code = else_node ? transpile_expression(ctx, else_node) : strdup("lisp_alloc(LISP_NIL)");

    CodeBuilder* cb = builder_create();
    // Translate to C ternary operator: (lisp_is_truthy(cond) ? (then) : (else))
    builder_append(cb, "(lisp_is_truthy(%s) ? (%s) : (%s))", cond_code, then_code, else_code);

    free(cond_code);
    free(then_code);
    free(else_code);

    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

// Handler: (> a b), (= a b)
static char* handle_logic(TranspilerContext* ctx, const char* op, LispVal* args) {
    if (!args || args->type != LISP_CONS || !args->data.cons.cdr) return strdup("lisp_alloc(LISP_T)");

    LispVal* arg1 = args->data.cons.car;
    LispVal* arg2 = args->data.cons.cdr->data.cons.car;

    char* code1 = transpile_expression(ctx, arg1);
    char* code2 = transpile_expression(ctx, arg2);

    CodeBuilder* cb = builder_create();
    // Note: For simplicity, assuming both are integers here. 
    // Real implementation requires type checking at runtime.
    if (strcmp(op, "=") == 0) op = "=="; // Map Lisp '=' to C '=='
    
    builder_append(cb, "lisp_bool((%s)->data.int_val %s (%s)->data.int_val)", code1, op, code2);

    free(code1);
    free(code2);

    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

// Handler: (car x), (cdr x), (cons a b), (quote x)
static char* handle_list_ops(TranspilerContext* ctx, const char* op, LispVal* args) {
    CodeBuilder* cb = builder_create();

    if (strcasecmp(op, "car") == 0) {
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        builder_append(cb, "lisp_car(%s)", arg_code);
        free(arg_code);
    } 
    else if (strcasecmp(op, "cdr") == 0) {
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        builder_append(cb, "lisp_cdr(%s)", arg_code);
        free(arg_code);
    } 
    else if (strcasecmp(op, "cons") == 0) {
        char* car_code = transpile_expression(ctx, args->data.cons.car);
        char* cdr_code = transpile_expression(ctx, args->data.cons.cdr->data.cons.car);
        builder_append(cb, "lisp_cons(%s, %s)", car_code, cdr_code);
        free(car_code);
        free(cdr_code);
    }
    else if (strcasecmp(op, "quote") == 0) {
        // Simple quoting implementation: if it's a list, we need to construct it
        // For now, if we quote an atom like 'a, it just returns symbol "a".
        // Real list quoting requires deep construction, handled in parse_quote in Stage 1.
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        builder_append(cb, "%s", arg_code);
        free(arg_code);
    }

    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}