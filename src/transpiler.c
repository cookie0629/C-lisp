#include "transpiler.h"
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
    #define strcasecmp _stricmp
    #define strdup _strdup
#endif

static char* sanitize_c_id(const char* name) {
    char* res = strdup(name);
    for (int i = 0; res[i]; i++) {
        char c = res[i];
        int is_valid = (c >= 'a' && c <= 'z') || 
                       (c >= 'A' && c <= 'Z') || 
                       (c >= '0' && c <= '9');
        if (!is_valid) {
            res[i] = '_';
        }
    }
    return res;
}

TranspilerContext* transpiler_create() {
    TranspilerContext* ctx = (TranspilerContext*)malloc(sizeof(TranspilerContext));
    ctx->global_defs = builder_create();
    ctx->main_body = builder_create();
    ctx->var_count = 0;
    ctx->let_counter = 0;
    
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

static bool is_var_declared(TranspilerContext* ctx, const char* safe_name) {
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->declared_vars[i], safe_name) == 0) return true;
    }
    return false;
}

static void declare_var(TranspilerContext* ctx, const char* name) {
    char* safe_name = sanitize_c_id(name); 
    if (!is_var_declared(ctx, safe_name) && ctx->var_count < MAX_VARS) {
        ctx->declared_vars[ctx->var_count++] = strdup(safe_name);
        builder_append(ctx->global_defs, "LispVal* lisp_var_%s = NULL;\n", safe_name);
    }
    free(safe_name);
}

// Forward Declarations
static char* handle_math(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_print(TranspilerContext* ctx, LispVal* args);
static char* handle_setq(TranspilerContext* ctx, LispVal* args);
static char* handle_defun(TranspilerContext* ctx, LispVal* args);
static char* handle_func_call(TranspilerContext* ctx, const char* func_name, LispVal* args);
static char* handle_progn(TranspilerContext* ctx, LispVal* args);
static char* handle_let(TranspilerContext* ctx, LispVal* args);
static char* handle_if(TranspilerContext* ctx, LispVal* args);
static char* handle_logic(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_list_ops(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_boolean_logic(TranspilerContext* ctx, const char* op, LispVal* args);
static char* handle_while(TranspilerContext* ctx, LispVal* args);
static char* handle_lambda(TranspilerContext* ctx, LispVal* args);
static char* handle_funcall(TranspilerContext* ctx, LispVal* args);
static char* handle_c_expr(TranspilerContext* ctx, LispVal* args);
static char* handle_c_stmt(TranspilerContext* ctx, LispVal* args);

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
        case LISP_SYMBOL: {
            char* safe_name = sanitize_c_id(expr->data.str_val); // 调用！
            builder_append(local_cb, "lisp_var_%s", safe_name);
            free(safe_name);
            break;
        }
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
                else if (strcasecmp(func_name, "progn") == 0) {
                    char* code = handle_progn(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "let") == 0) {
                    char* code = handle_let(ctx, tail);
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
                else if (strcasecmp(func_name, "and") == 0 || strcasecmp(func_name, "or") == 0 || strcasecmp(func_name, "not") == 0) {
                    char* code = handle_boolean_logic(ctx, func_name, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "while") == 0) {
                    char* code = handle_while(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "lambda") == 0) {
                    char* code = handle_lambda(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "funcall") == 0) {
                    char* code = handle_funcall(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "c-expr") == 0) {
                    char* code = handle_c_expr(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else if (strcasecmp(func_name, "c-stmt") == 0) {
                    char* code = handle_c_stmt(ctx, tail);
                    builder_append(local_cb, "%s", code);
                    free(code);
                }
                else {
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

static char* handle_setq(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    
    LispVal* sym_node = args->data.cons.car;
    LispVal* val_node = args->data.cons.cdr->data.cons.car;
    
    if (sym_node->type != LISP_SYMBOL) return strdup("lisp_alloc(LISP_NIL)");
    
    const char* var_name = sym_node->data.str_val;
    declare_var(ctx, var_name);
    
    char* safe_name = sanitize_c_id(var_name); // 调用！
    char* val_code = transpile_expression(ctx, val_node);
    
    CodeBuilder* cb = builder_create();
    builder_append(cb, "(lisp_var_%s = %s)", safe_name, val_code);
    
    free(safe_name);
    free(val_code);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

static char* handle_defun(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    
    LispVal* name_node = args->data.cons.car;
    LispVal* params_node = args->data.cons.cdr->data.cons.car;
    LispVal* body_node = args->data.cons.cdr->data.cons.cdr->data.cons.car;
    
    const char* func_name = name_node->data.str_val;
    declare_var(ctx, func_name);
    
    char* safe_name = sanitize_c_id(func_name); // 调用！
    CodeBuilder* func_cb = builder_create();
    builder_append(func_cb, "LispVal* lisp_func_%s(LispVal* _args, LispVal* _env) {\n", safe_name);
    
    int arg_idx = 0;
    LispVal* curr_param = params_node;
    while (curr_param && curr_param->type == LISP_CONS) {
        char* safe_param = sanitize_c_id(curr_param->data.cons.car->data.str_val); // 调用！
        builder_append(func_cb, "    LispVal* lisp_var_%s = lisp_car(", safe_param);
        for(int i=0; i<arg_idx; i++) builder_append(func_cb, "lisp_cdr(");
        builder_append(func_cb, "_args)");
        for(int i=0; i<arg_idx; i++) builder_append(func_cb, ")");
        builder_append(func_cb, ";\n");
        free(safe_param);
        curr_param = curr_param->data.cons.cdr;
        arg_idx++;
    }
    
    char* body_code = transpile_expression(ctx, body_node);
    builder_append(func_cb, "    return %s;\n", body_code);
    free(body_code);
    builder_append(func_cb, "}\n\n");
    
    builder_append(ctx->global_defs, "%s", builder_get(func_cb));
    builder_free(func_cb);
    
    CodeBuilder* res_cb = builder_create();
    builder_append(res_cb, "(lisp_var_%s = lisp_closure(lisp_func_%s, lisp_alloc(LISP_NIL)), lisp_symbol(\"%s\"))", 
                   safe_name, safe_name, func_name); 
    free(safe_name);
    
    char* res = strdup(builder_get(res_cb));
    builder_free(res_cb);
    return res;
}

static char* handle_func_call(TranspilerContext* ctx, const char* func_name, LispVal* args) {
    CodeBuilder* args_cb = builder_create();
    LispVal* curr_arg = args;
    if (!curr_arg || curr_arg->type == LISP_NIL) {
        builder_append(args_cb, "lisp_alloc(LISP_NIL)");
    } else {
        int close_parens = 0;
        while (curr_arg && curr_arg->type == LISP_CONS) {
            char* arg_val = transpile_expression(ctx, curr_arg->data.cons.car);
            builder_append(args_cb, "lisp_cons(%s, ", arg_val);
            free(arg_val);
            close_parens++;
            curr_arg = curr_arg->data.cons.cdr;
        }
        builder_append(args_cb, "lisp_alloc(LISP_NIL)");
        for(int i=0; i<close_parens; i++) builder_append(args_cb, ")");
    }
    
    char* safe_name = sanitize_c_id(func_name); // 调用！
    CodeBuilder* call_cb = builder_create();
    builder_append(call_cb, "lisp_thunk(lisp_func_%s, %s, lisp_alloc(LISP_NIL))", safe_name, builder_get(args_cb));
    
    free(safe_name);
    char* res = strdup(builder_get(call_cb));
    builder_free(args_cb);
    builder_free(call_cb);
    return res;
}

static char* handle_math(TranspilerContext* ctx, const char* op, LispVal* args) {
    CodeBuilder* cb = builder_create();
    builder_append(cb, "lisp_int(");
    LispVal* current = args;
    bool first = true;
    while (current && current->type == LISP_CONS) {
        if (!first) builder_append(cb, " %s ", op);
        char* arg_code = transpile_expression(ctx, current->data.cons.car);
        builder_append(cb, "lisp_trampoline(%s)->data.int_val", arg_code);
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
    builder_append(cb, "(lisp_print(lisp_trampoline(%s)), printf(\"\\n\"), lisp_alloc(LISP_T))", arg_code);
    free(arg_code);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

static char* handle_progn(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    CodeBuilder* cb = builder_create();
    builder_append(cb, "(");
    LispVal* curr = args;
    bool first = true;
    while (curr && curr->type == LISP_CONS) {
        if (!first) builder_append(cb, ", ");
        char* expr_code = transpile_expression(ctx, curr->data.cons.car);
        builder_append(cb, "%s", expr_code);
        free(expr_code);
        curr = curr->data.cons.cdr;
        first = false;
    }
    builder_append(cb, ")");
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

static char* handle_let(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    LispVal* bindings = args->data.cons.car;
    LispVal* body = args->data.cons.cdr; 
    int let_id = ctx->let_counter++;
    CodeBuilder* func_cb = builder_create();
    CodeBuilder* call_cb = builder_create();
    builder_append(func_cb, "LispVal* lisp_let_%d(", let_id);
    builder_append(call_cb, "lisp_let_%d(", let_id);
    LispVal* curr_bind = bindings;
    bool first = true;
    while (curr_bind && curr_bind->type == LISP_CONS) {
        LispVal* pair = curr_bind->data.cons.car; 
        char* safe_var = sanitize_c_id(pair->data.cons.car->data.str_val); // 调用！
        LispVal* val_expr = pair->data.cons.cdr->data.cons.car;
        if (!first) {
            builder_append(func_cb, ", ");
            builder_append(call_cb, ", ");
        }
        builder_append(func_cb, "LispVal* lisp_var_%s", safe_var);
        char* val_code = transpile_expression(ctx, val_expr);
        builder_append(call_cb, "%s", val_code);
        free(val_code);
        free(safe_var);
        curr_bind = curr_bind->data.cons.cdr;
        first = false;
    }
    if (first) builder_append(func_cb, "void");
    builder_append(func_cb, ") {\n");
    builder_append(call_cb, ")");
    char* body_code = handle_progn(ctx, body);
    builder_append(func_cb, "    return %s;\n", body_code);
    free(body_code);
    builder_append(func_cb, "}\n\n");
    builder_append(ctx->global_defs, "%s", builder_get(func_cb));
    char* res = strdup(builder_get(call_cb));
    builder_free(func_cb);
    builder_free(call_cb);
    return res;
}

static char* handle_if(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    LispVal* cond_node = args->data.cons.car;
    LispVal* then_node = args->data.cons.cdr ? args->data.cons.cdr->data.cons.car : NULL;
    LispVal* else_node = (args->data.cons.cdr && args->data.cons.cdr->data.cons.cdr) ? args->data.cons.cdr->data.cons.cdr->data.cons.car : NULL;
    char* cond_code = transpile_expression(ctx, cond_node);
    char* then_code = then_node ? transpile_expression(ctx, then_node) : strdup("lisp_alloc(LISP_NIL)");
    char* else_code = else_node ? transpile_expression(ctx, else_node) : strdup("lisp_alloc(LISP_NIL)");
    CodeBuilder* cb = builder_create();
    builder_append(cb, "(lisp_is_truthy(lisp_trampoline(%s)) ? (%s) : (%s))", cond_code, then_code, else_code);
    free(cond_code); free(then_code); free(else_code);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

static char* handle_logic(TranspilerContext* ctx, const char* op, LispVal* args) {
    if (!args || args->type != LISP_CONS || !args->data.cons.cdr) return strdup("lisp_alloc(LISP_T)");
    LispVal* arg1 = args->data.cons.car;
    LispVal* arg2 = args->data.cons.cdr->data.cons.car;
    char* code1 = transpile_expression(ctx, arg1);
    char* code2 = transpile_expression(ctx, arg2);
    CodeBuilder* cb = builder_create();
    if (strcmp(op, "=") == 0) op = "==";
    builder_append(cb, "lisp_bool(lisp_trampoline(%s)->data.int_val %s lisp_trampoline(%s)->data.int_val)", code1, op, code2);
    free(code1); free(code2);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

static char* handle_list_ops(TranspilerContext* ctx, const char* op, LispVal* args) {
    CodeBuilder* cb = builder_create();
    if (strcasecmp(op, "car") == 0) {
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        builder_append(cb, "lisp_car(lisp_trampoline(%s))", arg_code);
        free(arg_code);
    } 
    else if (strcasecmp(op, "cdr") == 0) {
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        builder_append(cb, "lisp_cdr(lisp_trampoline(%s))", arg_code);
        free(arg_code);
    } 
    else if (strcasecmp(op, "cons") == 0) {
        char* car_code = transpile_expression(ctx, args->data.cons.car);
        char* cdr_code = transpile_expression(ctx, args->data.cons.cdr->data.cons.car);
        builder_append(cb, "lisp_cons(%s, %s)", car_code, cdr_code);
        free(car_code); free(cdr_code);
    }
    else if (strcasecmp(op, "quote") == 0) {
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        builder_append(cb, "%s", arg_code);
        free(arg_code);
    }
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

static char* handle_boolean_logic(TranspilerContext* ctx, const char* op, LispVal* args) {
    if (strcasecmp(op, "not") == 0) {
        if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
        char* arg_code = transpile_expression(ctx, args->data.cons.car);
        CodeBuilder* cb = builder_create();
        builder_append(cb, "lisp_bool(!lisp_is_truthy(lisp_trampoline(%s)))", arg_code);
        free(arg_code);
        char* res = strdup(builder_get(cb));
        builder_free(cb);
        return res;
    }
    else if (strcasecmp(op, "and") == 0) {
        if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_T)"); 
        LispVal* arg1 = args->data.cons.car;
        LispVal* rest = args->data.cons.cdr;
        char* code1 = transpile_expression(ctx, arg1);
        if (!rest || rest->type == LISP_NIL) return code1; 
        char* code_rest = handle_boolean_logic(ctx, "and", rest);
        CodeBuilder* cb = builder_create();
        builder_append(cb, "(lisp_is_truthy(lisp_trampoline(%s)) ? (%s) : lisp_alloc(LISP_NIL))", code1, code_rest);
        free(code1); free(code_rest);
        char* res = strdup(builder_get(cb));
        builder_free(cb);
        return res;
    }
    else if (strcasecmp(op, "or") == 0) {
        if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)"); 
        LispVal* arg1 = args->data.cons.car;
        LispVal* rest = args->data.cons.cdr;
        char* code1 = transpile_expression(ctx, arg1);
        if (!rest || rest->type == LISP_NIL) return code1;
        char* code_rest = handle_boolean_logic(ctx, "or", rest);
        CodeBuilder* cb = builder_create();
        builder_append(cb, "(lisp_is_truthy(lisp_trampoline(%s)) ? lisp_alloc(LISP_T) : (%s))", code1, code_rest);
        free(code1); free(code_rest);
        char* res = strdup(builder_get(cb));
        builder_free(cb);
        return res;
    }
    return strdup("lisp_alloc(LISP_NIL)");
}

static char* handle_while(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    LispVal* cond_node = args->data.cons.car;
    LispVal* body_node = args->data.cons.cdr;
    int while_id = ctx->let_counter++; 
    CodeBuilder* func_cb = builder_create();
    builder_append(func_cb, "LispVal* lisp_while_%d() {\n", while_id);
    char* cond_code = transpile_expression(ctx, cond_node);
    builder_append(func_cb, "    while (lisp_is_truthy(lisp_trampoline(%s))) {\n", cond_code);
    char* body_code = handle_progn(ctx, body_node);
    builder_append(func_cb, "        lisp_trampoline(%s);\n", body_code); 
    free(body_code);
    builder_append(func_cb, "    }\n");
    builder_append(func_cb, "    return lisp_alloc(LISP_NIL);\n");
    builder_append(func_cb, "}\n\n");
    builder_append(ctx->global_defs, "%s", builder_get(func_cb));
    builder_free(func_cb);
    free(cond_code);
    CodeBuilder* call_cb = builder_create();
    builder_append(call_cb, "lisp_while_%d()", while_id);
    char* res = strdup(builder_get(call_cb));
    builder_free(call_cb);
    return res;
}

static char* handle_lambda(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    LispVal* params = args->data.cons.car;
    LispVal* body = args->data.cons.cdr;
    int lambda_id = ctx->let_counter++; 
    CodeBuilder* func_cb = builder_create();
    builder_append(func_cb, "LispVal* lisp_lambda_%d(LispVal* _args, LispVal* _env) {\n", lambda_id);
    int arg_idx = 0;
    LispVal* curr_param = params;
    while (curr_param && curr_param->type == LISP_CONS) {
        char* safe_param = sanitize_c_id(curr_param->data.cons.car->data.str_val); // 调用！
        builder_append(func_cb, "    LispVal* lisp_var_%s = lisp_car(", safe_param);
        for(int i=0; i<arg_idx; i++) builder_append(func_cb, "lisp_cdr(");
        builder_append(func_cb, "_args)");
        for(int i=0; i<arg_idx; i++) builder_append(func_cb, ")");
        builder_append(func_cb, ";\n");
        free(safe_param);
        curr_param = curr_param->data.cons.cdr;
        arg_idx++;
    }
    char* body_code = handle_progn(ctx, body);
    builder_append(func_cb, "    return %s;\n", body_code);
    free(body_code);
    builder_append(func_cb, "}\n\n");
    builder_append(ctx->global_defs, "%s", builder_get(func_cb));
    builder_free(func_cb);
    CodeBuilder* res_cb = builder_create();
    builder_append(res_cb, "lisp_closure(lisp_lambda_%d, lisp_alloc(LISP_NIL))", lambda_id);
    char* res = strdup(builder_get(res_cb));
    builder_free(res_cb);
    return res;
}

static char* handle_funcall(TranspilerContext* ctx, LispVal* args) {
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    char* func_code = transpile_expression(ctx, args->data.cons.car);
    CodeBuilder* args_cb = builder_create();
    LispVal* curr_arg = args->data.cons.cdr;
    if (!curr_arg || curr_arg->type == LISP_NIL) {
        builder_append(args_cb, "lisp_alloc(LISP_NIL)");
    } else {
        int close_parens = 0;
        while (curr_arg && curr_arg->type == LISP_CONS) {
            char* arg_val = transpile_expression(ctx, curr_arg->data.cons.car);
            builder_append(args_cb, "lisp_cons(%s, ", arg_val);
            free(arg_val);
            close_parens++;
            curr_arg = curr_arg->data.cons.cdr;
        }
        builder_append(args_cb, "lisp_alloc(LISP_NIL)");
        for(int i=0; i<close_parens; i++) builder_append(args_cb, ")");
    }
    CodeBuilder* call_cb = builder_create();
    builder_append(call_cb, "(((lisp_trampoline(%s))->type == LISP_CLOSURE) ? lisp_thunk((lisp_trampoline(%s))->data.closure.func, %s, (lisp_trampoline(%s))->data.closure.env) : lisp_alloc(LISP_NIL))", 
                   func_code, func_code, builder_get(args_cb), func_code);
    free(func_code);
    char* res = strdup(builder_get(call_cb));
    builder_free(args_cb);
    builder_free(call_cb);
    return res;
}

static char* handle_c_expr(TranspilerContext* ctx, LispVal* args) {
    (void)ctx;
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    LispVal* str_node = args->data.cons.car;
    if (str_node->type != LISP_STRING) return strdup("lisp_alloc(LISP_NIL)");
    return strdup(str_node->data.str_val);
}

static char* handle_c_stmt(TranspilerContext* ctx, LispVal* args) {
    (void)ctx;
    if (!args || args->type != LISP_CONS) return strdup("lisp_alloc(LISP_NIL)");
    LispVal* str_node = args->data.cons.car;
    if (str_node->type != LISP_STRING) return strdup("lisp_alloc(LISP_NIL)");
    CodeBuilder* cb = builder_create();
    builder_append(cb, "(%s, lisp_alloc(LISP_NIL))", str_node->data.str_val);
    char* res = strdup(builder_get(cb));
    builder_free(cb);
    return res;
}

void transpile_ast_to_file(TranspilerContext* ctx, LispVal* ast, const char* out_filename) {
    LispVal* current = ast;
    while (current && current->type == LISP_CONS) {
        char* expr_code = transpile_expression(ctx, current->data.cons.car);
        builder_append(ctx->main_body, "    lisp_trampoline(%s);\n", expr_code);
        free(expr_code);
        current = current->data.cons.cdr;
    }

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
