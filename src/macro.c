#include "macro.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _MSC_VER
    #define strdup _strdup
#endif

#define MAX_MACROS 256

typedef struct {
    char* name;
    LispVal* params;
    LispVal* body;
} Macro;

static Macro macros[MAX_MACROS];
static int macro_count = 0;

void macro_init() { macro_count = 0; }

int macro_is_definition(LispVal* expr)
{
    if (!expr || expr->type != LISP_CONS) return 0;
    LispVal* head = lisp_car(expr);
    if (head->type != LISP_SYMBOL) return 0;
    return strcmp(head->data.str_val, "defmacro") == 0;
}

void macro_register(LispVal* expr)
{
    LispVal* name_sym = lisp_car(lisp_cdr(expr));
    LispVal* params   = lisp_car(lisp_cdr(lisp_cdr(expr)));
    LispVal* body     = lisp_car(lisp_cdr(lisp_cdr(lisp_cdr(expr))));
    macros[macro_count].name   = strdup(name_sym->data.str_val);
    macros[macro_count].params = params;
    macros[macro_count].body   = body;
    macro_count++;
}

static Macro* find_macro(const char* name)
{
    for (int i = 0; i < macro_count; i++)
        if (strcmp(macros[i].name, name) == 0)
            return &macros[i];
    return NULL;
}

typedef struct Env {
    char*       name;
    LispVal*    val;
    struct Env* next;
} Env;

static Env* env_bind(Env* parent, const char* name, LispVal* val)
{
    Env* e = (Env*)malloc(sizeof(Env));
    e->name = strdup(name);
    e->val  = val;
    e->next = parent;
    return e;
}

static LispVal* env_lookup(Env* env, const char* name)
{
    for (Env* e = env; e; e = e->next)
        if (strcmp(e->name, name) == 0)
            return e->val;
    return NULL;
}

static void env_free_frame(Env* env, Env* stop)
{
    while (env && env != stop) {
        Env* next = env->next;
        free(env->name);
        free(env);
        env = next;
    }
}


static LispVal* eval(LispVal* expr, Env* env);
static LispVal* quasiquote_expand(LispVal* expr, Env* env);


static LispVal* quasiquote_expand(LispVal* expr, Env* env)
{
    if (!expr || expr->type != LISP_CONS)
        return expr;

    LispVal* head = lisp_car(expr);

    /* (unquote expr) -> evaluate expr */
    if (head->type == LISP_SYMBOL &&
        strcmp(head->data.str_val, "unquote") == 0)
        return eval(lisp_car(lisp_cdr(expr)), env);

    /* Walk list, handle ,@ splicing */
#define QQ_MAX 1024
    LispVal* elems[QQ_MAX];
    int count = 0;

    LispVal* curr = expr;
    while (curr && curr->type == LISP_CONS) {
        LispVal* item = lisp_car(curr);

        if (item->type == LISP_CONS) {
            LispVal* ih = lisp_car(item);
            if (ih->type == LISP_SYMBOL &&
                strcmp(ih->data.str_val, "unquote-splicing") == 0)
            {
                LispVal* spliced = eval(lisp_car(lisp_cdr(item)), env);
                while (spliced && spliced->type == LISP_CONS) {
                    if (count < QQ_MAX) elems[count++] = lisp_car(spliced);
                    spliced = lisp_cdr(spliced);
                }
                curr = lisp_cdr(curr);
                continue;
            }
        }

        if (count < QQ_MAX)
            elems[count++] = quasiquote_expand(item, env);
        curr = lisp_cdr(curr);
    }

    LispVal* result = lisp_alloc(LISP_NIL);
    for (int i = count - 1; i >= 0; i--)
        result = lisp_cons(elems[i], result);
    return result;
}

static LispVal* eval_list(LispVal* list, Env* env)
{
    if (!list || list->type != LISP_CONS) return lisp_alloc(LISP_NIL);
    return lisp_cons(eval(lisp_car(list), env),
                     eval_list(lisp_cdr(list), env));
}

static LispVal* eval(LispVal* expr, Env* env)
{
    if (!expr) return lisp_alloc(LISP_NIL);

    /* Self-evaluating types */
    if (expr->type == LISP_INT   || expr->type == LISP_FLOAT ||
        expr->type == LISP_STRING|| expr->type == LISP_NIL   ||
        expr->type == LISP_T)
        return expr;

    /* Symbol lookup */
    if (expr->type == LISP_SYMBOL) {
        LispVal* val = env_lookup(env, expr->data.str_val);
        return val ? val : expr;
    }

    if (expr->type != LISP_CONS) return expr;

    LispVal* head = lisp_car(expr);
    LispVal* args = lisp_cdr(expr);

    if (head->type != LISP_SYMBOL) return expr;
    const char* op = head->data.str_val;

    /* (quote x) */
    if (strcmp(op, "quote") == 0)
        return lisp_car(args);

    /* (quasiquote template) */
    if (strcmp(op, "quasiquote") == 0)
        return quasiquote_expand(lisp_car(args), env);

    /* (if cond then else?) */
    if (strcmp(op, "if") == 0) {
        LispVal* cond = eval(lisp_car(args), env);
        if (lisp_is_truthy(cond))
            return eval(lisp_car(lisp_cdr(args)), env);
        LispVal* eb = lisp_cdr(lisp_cdr(args));
        return (eb && eb->type == LISP_CONS)
               ? eval(lisp_car(eb), env)
               : lisp_alloc(LISP_NIL);
    }

    /* (progn e1 e2 ...) */
    if (strcmp(op, "progn") == 0) {
        LispVal* result = lisp_alloc(LISP_NIL);
        for (LispVal* c = args; c && c->type == LISP_CONS; c = lisp_cdr(c))
            result = eval(lisp_car(c), env);
        return result;
    }

    /* (let ((var val) ...) body...) */
    if (strcmp(op, "let") == 0) {
        LispVal* bindings = lisp_car(args);
        LispVal* body     = lisp_cdr(args);
        Env* frame = env;
        for (LispVal* b = bindings; b && b->type == LISP_CONS; b = lisp_cdr(b)) {
            LispVal* pair = lisp_car(b);
            frame = env_bind(frame, lisp_car(pair)->data.str_val,
                             eval(lisp_car(lisp_cdr(pair)), env));
        }
        LispVal* result = lisp_alloc(LISP_NIL);
        for (LispVal* c = body; c && c->type == LISP_CONS; c = lisp_cdr(c))
            result = eval(lisp_car(c), frame);
        env_free_frame(frame, env);
        return result;
    }

    /* (cons a b) */
    if (strcmp(op, "cons") == 0)
        return lisp_cons(eval(lisp_car(args), env),
                         eval(lisp_car(lisp_cdr(args)), env));

    /* (car x) / (cdr x) */
    if (strcmp(op, "car") == 0) return lisp_car(eval(lisp_car(args), env));
    if (strcmp(op, "cdr") == 0) return lisp_cdr(eval(lisp_car(args), env));

    /* (list e1 e2 ...) */
    if (strcmp(op, "list") == 0) return eval_list(args, env);

    /* (not x) */
    if (strcmp(op, "not") == 0)
        return lisp_bool(!lisp_is_truthy(eval(lisp_car(args), env)));

    /* Arithmetic: + - * / */
    if (strcmp(op,"+") == 0 || strcmp(op,"-") == 0 ||
        strcmp(op,"*") == 0 || strcmp(op,"/") == 0)
    {
        LispVal* a = eval(lisp_car(args), env);
        LispVal* b = eval(lisp_car(lisp_cdr(args)), env);
        long long av = (a && a->type == LISP_INT) ? a->data.int_val : 0;
        long long bv = (b && b->type == LISP_INT) ? b->data.int_val : 0;
        long long res = 0;
        if      (strcmp(op,"+") == 0) res = av + bv;
        else if (strcmp(op,"-") == 0) res = av - bv;
        else if (strcmp(op,"*") == 0) res = av * bv;
        else if (bv != 0)             res = av / bv;
        return lisp_int(res);
    }

    /* Comparison: = < > <= >= */
    if (strcmp(op,"=")  == 0 || strcmp(op,"<")  == 0 ||
        strcmp(op,">")  == 0 || strcmp(op,"<=") == 0 ||
        strcmp(op,">=") == 0)
    {
        LispVal* a = eval(lisp_car(args), env);
        LispVal* b = eval(lisp_car(lisp_cdr(args)), env);
        long long av = (a && a->type == LISP_INT) ? a->data.int_val : 0;
        long long bv = (b && b->type == LISP_INT) ? b->data.int_val : 0;
        bool res = false;
        if      (strcmp(op,"=")  == 0) res = av == bv;
        else if (strcmp(op,"<")  == 0) res = av <  bv;
        else if (strcmp(op,">")  == 0) res = av >  bv;
        else if (strcmp(op,"<=") == 0) res = av <= bv;
        else if (strcmp(op,">=") == 0) res = av >= bv;
        return lisp_bool(res);
    }

    /* (string-append s1 s2 ...) */
    if (strcmp(op, "string-append") == 0) {
        size_t total = 0;
        for (LispVal* c = args; c && c->type == LISP_CONS; c = lisp_cdr(c)) {
            LispVal* s = eval(lisp_car(c), env);
            if (s && s->type == LISP_STRING) total += strlen(s->data.str_val);
        }
        char* buf = (char*)malloc(total + 1);
        buf[0] = '\0';
        for (LispVal* c = args; c && c->type == LISP_CONS; c = lisp_cdr(c)) {
            LispVal* s = eval(lisp_car(c), env);
            if (s && s->type == LISP_STRING) strcat(buf, s->data.str_val);
        }
        LispVal* result = lisp_string(buf);
        free(buf);
        return result;
    }

    /* (symbol->string sym) */
    if (strcmp(op, "symbol->string") == 0) {
        LispVal* s = eval(lisp_car(args), env);
        return (s && s->type == LISP_SYMBOL) ? lisp_string(s->data.str_val) : lisp_string("");
    }

    /* (number->string n) */
    if (strcmp(op, "number->string") == 0) {
        LispVal* n = eval(lisp_car(args), env);
        char buf[64] = {0};
        if (n && n->type == LISP_INT)   snprintf(buf, sizeof(buf), "%lld", n->data.int_val);
        if (n && n->type == LISP_FLOAT) snprintf(buf, sizeof(buf), "%f",   n->data.float_val);
        return lisp_string(buf);
    }

    /* (c-code "raw C string") — signals transpiler to inject C directly */
    if (strcmp(op, "c-code") == 0) {
        LispVal* s = eval(lisp_car(args), env);
        /* Return tagged: (c-code . "the string") */
        return lisp_cons(lisp_symbol("c-code"), s);
    }

    /* Unknown form — return unevaluated */
    return expr;
}

LispVal* macro_expand(LispVal* expr)
{
    if (!expr || expr->type != LISP_CONS)
        return expr;

    LispVal* head = lisp_car(expr);

    if (head->type == LISP_SYMBOL) {
        Macro* m = find_macro(head->data.str_val);
        if (m) {
            /* Bind params to unevaluated call arguments */
            Env* env = NULL;
            LispVal* p = m->params;
            LispVal* a = lisp_cdr(expr);
            while (p && p->type == LISP_CONS) {
                env = env_bind(env, lisp_car(p)->data.str_val, lisp_car(a));
                p = lisp_cdr(p);
                a = lisp_cdr(a);
            }

            /* Evaluate macro body at compile time */
            LispVal* expanded = eval(m->body, env);
            env_free_frame(env, NULL);

            /* Recursively expand in case result contains more macros */
            return macro_expand(expanded);
        }
    }

    /* Not a macro call — recurse into sub-expressions */
    return lisp_cons(macro_expand(lisp_car(expr)),
                     macro_expand(lisp_cdr(expr)));
}
