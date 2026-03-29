// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_i = NULL;
LispVal* lisp_while_0() {
    while (lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var_i)->data.int_val <= lisp_trampoline(lisp_int(5))->data.int_val)))) {
        lisp_trampoline((((lisp_print(lisp_trampoline(lisp_var_i)), printf("\n"), lisp_alloc(LISP_T)), (lisp_var_i = lisp_int(lisp_trampoline(lisp_var_i)->data.int_val + lisp_trampoline(lisp_int(1))->data.int_val)))));
    }
    return lisp_alloc(LISP_NIL);
}

LispVal* lisp_var__repeat_i = NULL;
LispVal* lisp_while_1() {
    while (lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var__repeat_i)->data.int_val < lisp_trampoline(lisp_int(3))->data.int_val)))) {
        lisp_trampoline((((lisp_print(lisp_trampoline(lisp_string("hello from repeat"))), printf("\n"), lisp_alloc(LISP_T)), (lisp_var__repeat_i = lisp_int(lisp_trampoline(lisp_var__repeat_i)->data.int_val + lisp_trampoline(lisp_int(1))->data.int_val)))));
    }
    return lisp_alloc(LISP_NIL);
}

LispVal* lisp_var_val = NULL;
LispVal* lisp_var_big_list = NULL;
LispVal* lisp_var_k = NULL;
LispVal* lisp_while_2() {
    while (lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_var_k)->data.int_val <= lisp_trampoline(lisp_int(300))->data.int_val)))) {
        lisp_trampoline((((lisp_var_big_list = lisp_cons(lisp_var_k, lisp_var_big_list)), (lisp_var_k = lisp_int(lisp_trampoline(lisp_var_k)->data.int_val + lisp_trampoline(lisp_int(1))->data.int_val)))));
    }
    return lisp_alloc(LISP_NIL);
}

LispVal* lisp_var_result = NULL;

int main() {
    lisp_runtime_init(1024 * 1024);

    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Macro Demo: for loop (1 to 5) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline(((lisp_var_i = lisp_int(1)), lisp_while_0()));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Macro Demo: repeat (3 times) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline(((lisp_var__repeat_i = lisp_int(0)), lisp_while_1()));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Macro Demo: square & max2 ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_val = lisp_int(7)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_int(lisp_trampoline(lisp_var_val)->data.int_val * lisp_trampoline(lisp_var_val)->data.int_val))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline((lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_int(3))->data.int_val > lisp_trampoline(lisp_int(9))->data.int_val))) ? (lisp_int(3)) : (lisp_int(9))))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline((lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_int(100))->data.int_val > lisp_trampoline(lisp_int(42))->data.int_val))) ? (lisp_int(100)) : (lisp_int(42))))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== Macro Demo: assert (should pass) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_is_truthy(lisp_trampoline(lisp_bool(!lisp_is_truthy(lisp_trampoline(lisp_bool(lisp_trampoline(lisp_int(10))->data.int_val > lisp_trampoline(lisp_int(0))->data.int_val)))))) ? (((lisp_print(lisp_trampoline(lisp_string("FAIL: 10 > 0 should be true"))), printf("\n"), lisp_alloc(LISP_T)), (exit(1), lisp_alloc(LISP_NIL)))) : (lisp_alloc(LISP_NIL))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("assert passed OK"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string(""))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== GC Demo: Heap state BEFORE allocation ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_print_stats(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_big_list = lisp_alloc(LISP_NIL)));
    lisp_trampoline(((lisp_var_k = lisp_int(1)), lisp_while_2()));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== GC Demo: Heap state AFTER building 300-element list ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_print_stats(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_big_list = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== GC Demo: Dropped reference. Forcing collection... ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_collect(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== GC Demo: Heap state AFTER GC collection ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_print_stats(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_result = lisp_int(lisp_trampoline(lisp_int(1000))->data.int_val + lisp_trampoline(lisp_int(337))->data.int_val)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Allocation after GC works fine:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_var_result)), printf("\n"), lisp_alloc(LISP_T)));

    lisp_runtime_shutdown();
    return 0;
}
