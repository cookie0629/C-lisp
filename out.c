// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_global_var = NULL;
LispVal* lisp_let_0(LispVal* lisp_var_local_var) {
    return (((lisp_print(lisp_trampoline(lisp_string("Local + Global:"))), printf("\n"), lisp_alloc(LISP_T)), (lisp_print(lisp_trampoline(lisp_int(lisp_trampoline(lisp_var_global_var)->data.int_val + lisp_trampoline(lisp_var_local_var)->data.int_val))), printf("\n"), lisp_alloc(LISP_T))));
}

LispVal* lisp_var_i = NULL;
LispVal* lisp_while_1() {
    while (lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var_i)->data.int_val < lisp_trampoline(lisp_int(3))->data.int_val)))) {
        lisp_trampoline((((lisp_print(lisp_trampoline(lisp_var_i)), printf("\n"), lisp_alloc(LISP_T)), (lisp_var_i = lisp_int(lisp_trampoline(lisp_var_i)->data.int_val + lisp_trampoline(lisp_int(1))->data.int_val)))));
    }
    return lisp_alloc(LISP_NIL);
}

LispVal* lisp_var_my_list = NULL;
LispVal* lisp_var_double_func = NULL;
LispVal* lisp_lambda_2(LispVal* _args, LispVal* _env) {
    LispVal* lisp_var_x = lisp_car(_args);
    return (lisp_int(lisp_trampoline(lisp_var_x)->data.int_val * lisp_trampoline(lisp_int(2))->data.int_val));
}

LispVal* lisp_var_apply_func = NULL;
LispVal* lisp_lambda_3(LispVal* _args, LispVal* _env) {
    LispVal* lisp_var_f = lisp_car(_args);
    LispVal* lisp_var_x = lisp_car(lisp_cdr(_args));
    return ((((lisp_trampoline(lisp_var_f))->type == LISP_CLOSURE) ? lisp_thunk((lisp_trampoline(lisp_var_f))->data.closure.func, lisp_cons(lisp_var_x, lisp_alloc(LISP_NIL)), (lisp_trampoline(lisp_var_f))->data.closure.env) : lisp_alloc(LISP_NIL)));
}

LispVal* lisp_var_tail_sum = NULL;
LispVal* lisp_func_tail_sum(LispVal* _args, LispVal* _env) {
    LispVal* lisp_var_n = lisp_car(_args);
    LispVal* lisp_var_acc = lisp_car(lisp_cdr(_args));
    return (lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var_n)->data.int_val <= lisp_trampoline(lisp_int(0))->data.int_val))) ? (lisp_var_acc) : (lisp_thunk(lisp_func_tail_sum, lisp_cons(lisp_int(lisp_trampoline(lisp_var_n)->data.int_val - lisp_trampoline(lisp_int(1))->data.int_val), lisp_cons(lisp_int(lisp_trampoline(lisp_var_acc)->data.int_val + lisp_trampoline(lisp_var_n)->data.int_val), lisp_alloc(LISP_NIL))), lisp_alloc(LISP_NIL))));
}


int main() {
    lisp_runtime_init(1024 * 1024);

    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== 1. Variables & Lexical Scope ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_global_var = lisp_int(100)));
    lisp_trampoline(lisp_let_0(lisp_int(25)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== 2. Logic & Short-Circuit Evaluation ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_is_truthy(lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("AND Test: Pass 1"))), printf("\n"), lisp_alloc(LISP_T)))) ? ((lisp_is_truthy(lisp_trampoline(lisp_alloc(LISP_NIL))) ? ((lisp_print(lisp_trampoline(lisp_string("AND Test: FAIL (Should short-circuit!)"))), printf("\n"), lisp_alloc(LISP_T))) : lisp_alloc(LISP_NIL))) : lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_is_truthy(lisp_trampoline(lisp_alloc(LISP_NIL))) ? lisp_alloc(LISP_T) : ((lisp_is_truthy(lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("OR Test: Pass 1"))), printf("\n"), lisp_alloc(LISP_T)))) ? lisp_alloc(LISP_T) : ((lisp_print(lisp_trampoline(lisp_string("OR Test: FAIL (Should short-circuit!)"))), printf("\n"), lisp_alloc(LISP_T)))))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== 3. Iteration (While) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_i = lisp_int(0)));
    lisp_trampoline(lisp_while_1());
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== 4. Lists & Higher-Order Functions ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_my_list = lisp_cons(lisp_int(10), lisp_cons(lisp_int(20), lisp_cons(lisp_int(30), lisp_alloc(LISP_NIL))))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("CDR of CAR:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_car(lisp_trampoline(lisp_cdr(lisp_trampoline(lisp_var_my_list)))))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_double_func = lisp_closure(lisp_lambda_2, lisp_alloc(LISP_NIL))));
    lisp_trampoline((lisp_var_apply_func = lisp_closure(lisp_lambda_3, lisp_alloc(LISP_NIL))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Passing Function as Data:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline((((lisp_trampoline(lisp_var_apply_func))->type == LISP_CLOSURE) ? lisp_thunk((lisp_trampoline(lisp_var_apply_func))->data.closure.func, lisp_cons(lisp_var_double_func, lisp_cons(lisp_int(500), lisp_alloc(LISP_NIL))), (lisp_trampoline(lisp_var_apply_func))->data.closure.env) : lisp_alloc(LISP_NIL)))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== 5. Inline C Execution (Super Macro) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((puts("Hello from native C layer! Pointers and registers welcome!"), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("C Math Injected:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_int(1 << 10))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== 6. Tail Call Optimization (TCO) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_tail_sum = lisp_closure(lisp_func_tail_sum, lisp_alloc(LISP_NIL)), lisp_symbol("tail-sum")));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Sum from 1 to 50000 (Testing Trampoline O(1) Memory):"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_thunk(lisp_func_tail_sum, lisp_cons(lisp_int(50000), lisp_cons(lisp_int(0), lisp_alloc(LISP_NIL))), lisp_alloc(LISP_NIL)))), printf("\n"), lisp_alloc(LISP_T)));

    lisp_runtime_shutdown();
    return 0;
}
