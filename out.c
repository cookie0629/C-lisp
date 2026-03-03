// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_global_num = NULL;
LispVal* lisp_func_multiply(LispVal* lisp_var_x, LispVal* lisp_var_y) {
    return lisp_int((lisp_var_x)->data.int_val * (lisp_var_y)->data.int_val);
}


int main() {
    lisp_runtime_init(1024 * 1024);

    (lisp_var_global_num = lisp_int(100));
    lisp_symbol("multiply");
    (lisp_print(lisp_string("Testing Variables:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_var_global_num), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_string("Testing Functions:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_func_multiply(lisp_var_global_num, lisp_int(5))), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
