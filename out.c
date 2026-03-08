// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_hacker_var = NULL;

int main() {
    lisp_runtime_init(1024 * 1024);

    (lisp_print(lisp_string("--- Stage 6: Inline C Magic ---")), printf("\n"), lisp_alloc(LISP_T));
    (puts("Hello from Native C! I bypassed the Lisp runtime!"), lisp_alloc(LISP_NIL));
    (lisp_var_hacker_var = lisp_int(1000 + 337));
    (lisp_print(lisp_string("Lisp read the injected C result:")), printf("\n"), lisp_alloc(LISP_T));
    (lisp_print(lisp_var_hacker_var), printf("\n"), lisp_alloc(LISP_T));

    lisp_runtime_shutdown();
    return 0;
}
