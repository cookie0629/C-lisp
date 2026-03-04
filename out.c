// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_x = NULL;
LispVal* lisp_let_0(LispVal* lisp_var_x, LispVal* lisp_var_y) {
    return (((lisp_print(lisp_string("Local x in let block is:")), printf("\n"), lisp_alloc(LISP_T)), (lisp_print(lisp_var_x), printf("\n"), lisp_alloc(LISP_T)), (lisp_print(lisp_string("Sum of local x and y is:")), printf("\n"), lisp_alloc(LISP_T)), lisp_int((lisp_var_x)->data.int_val + (lisp_var_y)->data.int_val)));
}


int main() {
    lisp_runtime_init(1024 * 1024);

    (lisp_var_x = lisp_int(100));
    (lisp_print(lisp_string("Global x is:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_var_x), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_string("Testing PROGN and LET:")), printf("\n"), lisp_alloc(LISP_T));
    lisp_let_0(lisp_int(10), lisp_int(20));
    (lisp_print(lisp_string("Global x after let is still:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_var_x), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
