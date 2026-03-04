// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_func_fib(LispVal* lisp_var_n) {
    return (lisp_is_truthy(lisp_bool((lisp_var_n)->data.int_val < (lisp_int(2))->data.int_val)) ? (lisp_var_n) : (lisp_int((lisp_func_fib(lisp_int((lisp_var_n)->data.int_val - (lisp_int(1))->data.int_val)))->data.int_val + (lisp_func_fib(lisp_int((lisp_var_n)->data.int_val - (lisp_int(2))->data.int_val)))->data.int_val)));
}

LispVal* lisp_var_my_list = NULL;

int main() {
    lisp_runtime_init(1024 * 1024);

    lisp_symbol("fib");
    (lisp_print(lisp_string("Fibonacci of 6 is:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_func_fib(lisp_int(6))), printf("\n"), lisp_alloc(LISP_T));
    (lisp_var_my_list = lisp_cons(lisp_int(1), lisp_cons(lisp_int(2), lisp_cons(lisp_int(3), lisp_alloc(LISP_NIL)))));
    (lisp_print(lisp_string("My List:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_var_my_list), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_string("CAR of My List:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_car(lisp_var_my_list)), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_string("CDR of My List:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_cdr(lisp_var_my_list)), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
