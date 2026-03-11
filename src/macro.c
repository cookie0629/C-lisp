#include "macro.h"
#include <string.h>
#include <stdlib.h>

#define MAX_MACROS 256

typedef struct {
    char* name;
    LispVal* params;
    LispVal* body;
} Macro;

static Macro macros[MAX_MACROS];
static int macro_count = 0;

void macro_init()
{
    macro_count = 0;
}

int macro_is_definition(LispVal* expr)
{
    if (!expr || expr->type != LISP_CONS)
        return 0;

    LispVal* head = lisp_car(expr);

    if (head->type != LISP_SYMBOL)
        return 0;

    return strcmp(head->data.str_val, "defmacro") == 0;
}

void macro_register(LispVal* expr)
{
    LispVal* name_sym = lisp_car(lisp_cdr(expr));
    LispVal* params   = lisp_car(lisp_cdr(lisp_cdr(expr)));
    LispVal* body     = lisp_car(lisp_cdr(lisp_cdr(lisp_cdr(expr))));

    macros[macro_count].name = strdup(name_sym->data.str_val);
    macros[macro_count].params = params;
    macros[macro_count].body = body;

    macro_count++;
}

static Macro* find_macro(const char* name)
{
    for (int i = 0; i < macro_count; i++)
    {
        if (strcmp(macros[i].name, name) == 0)
            return &macros[i];
    }

    return NULL;
}

static LispVal* substitute(LispVal* expr, LispVal* params, LispVal* args)
{
    if (!expr)
        return expr;

    if (expr->type == LISP_SYMBOL)
    {
        LispVal* p = params;
        LispVal* a = args;

        while (p && p->type == LISP_CONS)
        {
            LispVal* param = lisp_car(p);
            LispVal* arg   = lisp_car(a);

            if (strcmp(expr->data.str_val,
                       param->data.str_val) == 0)
                return arg;

            p = lisp_cdr(p);
            a = lisp_cdr(a);
        }

        return expr;
    }

    if (expr->type == LISP_CONS)
    {
        LispVal* new_car =
            substitute(lisp_car(expr), params, args);

        LispVal* new_cdr =
            substitute(lisp_cdr(expr), params, args);

        return lisp_cons(new_car, new_cdr);
    }

    return expr;
}

static LispVal* expand_macro(Macro* m, LispVal* args)
{
    return substitute(m->body, m->params, args);
}

LispVal* macro_expand(LispVal* expr)
{
    if (!expr)
        return expr;

    if (expr->type != LISP_CONS)
        return expr;

    LispVal* head = lisp_car(expr);

    if (head->type == LISP_SYMBOL)
    {
        Macro* m = find_macro(head->data.str_val);

        if (m)
        {
            LispVal* args = lisp_cdr(expr);

            LispVal* expanded =
                expand_macro(m, args);

            return macro_expand(expanded);
        }
    }

    LispVal* new_car =
        macro_expand(lisp_car(expr));

    LispVal* new_cdr =
        macro_expand(lisp_cdr(expr));

    return lisp_cons(new_car, new_cdr);
}

