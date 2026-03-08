// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_my_adder = NULL;
LispVal* lisp_lambda_0(LispVal* _args, LispVal* _env) {
    LispVal* lisp_var_x = lisp_car(_args);
    LispVal* lisp_var_y = lisp_car(lisp_cdr(_args));
    return (lisp_int((lisp_var_x)->data.int_val + (lisp_var_y)->data.int_val));
}

LispVal* lisp_var_execute_op = NULL;
LispVal* lisp_lambda_1(LispVal* _args, LispVal* _env) {
    LispVal* lisp_var_op = lisp_car(_args);
    LispVal* lisp_var_a = lisp_car(lisp_cdr(_args));
    LispVal* lisp_var_b = lisp_car(lisp_cdr(lisp_cdr(_args)));
    return ((((lisp_var_op)->type == LISP_CLOSURE) ? (lisp_var_op)->data.closure.func(lisp_cons(lisp_var_a, lisp_cons(lisp_var_b, lisp_alloc(LISP_NIL))), (lisp_var_op)->data.closure.env) : lisp_alloc(LISP_NIL)));
}


int main() {
    lisp_runtime_init(1024 * 1024);

    (lisp_var_my_adder = lisp_closure(lisp_lambda_0, lisp_alloc(LISP_NIL)));
    (lisp_print(lisp_string("Testing First-Class Lambda:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print((((lisp_var_my_adder)->type == LISP_CLOSURE) ? (lisp_var_my_adder)->data.closure.func(lisp_cons(lisp_int(100), lisp_cons(lisp_int(25), lisp_alloc(LISP_NIL))), (lisp_var_my_adder)->data.closure.env) : lisp_alloc(LISP_NIL))), printf("\n"), lisp_alloc(LISP_T));
    (lisp_var_execute_op = lisp_closure(lisp_lambda_1, lisp_alloc(LISP_NIL)));
    (lisp_print(lisp_string("Testing Higher-Order Function:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print((((lisp_var_execute_op)->type == LISP_CLOSURE) ? (lisp_var_execute_op)->data.closure.func(lisp_cons(lisp_var_my_adder, lisp_cons(lisp_int(50), lisp_cons(lisp_int(50), lisp_alloc(LISP_NIL)))), (lisp_var_execute_op)->data.closure.env) : lisp_alloc(LISP_NIL))), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
