// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_counter = NULL;
LispVal* lisp_while_0() {
    while (lisp_is_truthy(lisp_bool((lisp_var_counter)->data.int_val < (lisp_int(5))->data.int_val))) {
        (((lisp_print(lisp_string("Counter is currently:")), printf("\n"), lisp_alloc(LISP_T)), (lisp_print(lisp_var_counter), printf("\n"), lisp_alloc(LISP_T)), (lisp_var_counter = lisp_int((lisp_var_counter)->data.int_val + (lisp_int(1))->data.int_val))));
    }
    return lisp_alloc(LISP_NIL);
}


int main() {
    lisp_runtime_init(1024 * 1024);

    (lisp_print(lisp_string("--- Testing Logic ---")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_is_truthy((lisp_print(lisp_string("AND: This will print")), printf("\n"), lisp_alloc(LISP_T))) ? ((lisp_is_truthy(lisp_alloc(LISP_NIL)) ? ((lisp_print(lisp_string("AND: This MUST NOT print! (Short-circuited)")), printf("\n"), lisp_alloc(LISP_T))) : lisp_alloc(LISP_NIL))) : lisp_alloc(LISP_NIL));
    (lisp_is_truthy((lisp_print(lisp_string("OR: This will print")), printf("\n"), lisp_alloc(LISP_T))) ? lisp_alloc(LISP_T) : ((lisp_print(lisp_string("OR: This MUST NOT print! (Short-circuited)")), printf("\n"), lisp_alloc(LISP_T))));
    (lisp_print(lisp_string("--- Testing Iteration ---")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_var_counter = lisp_int(0));
    lisp_while_0();
    (lisp_print(lisp_string("Loop finished!")), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
