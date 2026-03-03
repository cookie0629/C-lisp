// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>


int main() {
    lisp_runtime_init(1024 * 1024);

    (lisp_print(lisp_string("Hello from Transpiler!")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_int((lisp_int(10))->data.int_val + (lisp_int(20))->data.int_val)), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
