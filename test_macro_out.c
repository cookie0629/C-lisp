// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_x = NULL;
LispVal* lisp_var_counter = NULL;
LispVal* lisp_var_p = NULL;
LispVal* lisp_var_q = NULL;
LispVal* lisp_let_0(LispVal* lisp_var_tmp) {
    return (((lisp_var_p = lisp_var_q), (lisp_var_q = lisp_var_tmp)));
}


int main() {
    lisp_runtime_init(1024 * 1024);

    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Test 1: when macro (quasiquote) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_x = lisp_int(10)));
    lisp_trampoline((lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var_x)->data.int_val > lisp_trampoline(lisp_int(5))->data.int_val))) ? ((lisp_print(lisp_trampoline(lisp_string("x is greater than 5"))), printf("\n"), lisp_alloc(LISP_T))) : (lisp_alloc(LISP_NIL))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Test 2: unless macro (compile-time not) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_is_truthy(lisp_trampoline(lisp_bool(!lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var_x)->data.int_val == lisp_trampoline(lisp_int(0))->data.int_val)))))) ? ((lisp_print(lisp_trampoline(lisp_string("x is not zero"))), printf("\n"), lisp_alloc(LISP_T))) : (lisp_alloc(LISP_NIL))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Test 3: inc-by macro (compile-time code generation) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_counter = lisp_int(0)));
    lisp_trampoline((lisp_var_counter = lisp_int(lisp_trampoline(lisp_var_counter)->data.int_val + lisp_trampoline(lisp_int(5))->data.int_val)));
    lisp_trampoline((lisp_var_counter = lisp_int(lisp_trampoline(lisp_var_counter)->data.int_val + lisp_trampoline(lisp_int(3))->data.int_val)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_var_counter)), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Test 4: fast-print macro (direct C code generation) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline(lisp_int(puts("Hello_from_macro_generated_C")));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Test 5: swap macro (nested quasiquote) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_p = lisp_int(100)));
    lisp_trampoline((lisp_var_q = lisp_int(200)));
    lisp_trampoline(lisp_let_0(lisp_var_p));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_var_p)), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_var_q)), printf("\n"), lisp_alloc(LISP_T)));

    lisp_runtime_shutdown();
    return 0;
}
