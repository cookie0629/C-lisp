// --- GENERATED C-LISP CODE ---

#include "runtime/nucleus.h"
#include <stdio.h>

LispVal* lisp_var_n7 = NULL;
LispVal* lisp_var_n8 = NULL;
LispVal* lisp_var_n4 = NULL;
LispVal* lisp_var_n5 = NULL;
LispVal* lisp_var_n6 = NULL;
LispVal* lisp_var_n3 = NULL;
LispVal* lisp_var_n2 = NULL;
LispVal* lisp_var_n1 = NULL;
LispVal* lisp_var_tree_root = NULL;
LispVal* lisp_var_check = NULL;

int main() {
    lisp_runtime_init(1024 * 1024);

    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 1] Initial heap state ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_print_stats(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 2] Building 8-node binary tree ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_n7 = lisp_cons(lisp_int(7), lisp_cons(lisp_alloc(LISP_NIL), lisp_alloc(LISP_NIL)))));
    lisp_trampoline((lisp_var_n8 = lisp_cons(lisp_int(8), lisp_cons(lisp_alloc(LISP_NIL), lisp_alloc(LISP_NIL)))));
    lisp_trampoline((lisp_var_n4 = lisp_cons(lisp_int(4), lisp_cons(lisp_alloc(LISP_NIL), lisp_alloc(LISP_NIL)))));
    lisp_trampoline((lisp_var_n5 = lisp_cons(lisp_int(5), lisp_cons(lisp_alloc(LISP_NIL), lisp_alloc(LISP_NIL)))));
    lisp_trampoline((lisp_var_n6 = lisp_cons(lisp_int(6), lisp_cons(lisp_alloc(LISP_NIL), lisp_alloc(LISP_NIL)))));
    lisp_trampoline((lisp_var_n3 = lisp_cons(lisp_int(3), lisp_cons(lisp_var_n7, lisp_var_n8))));
    lisp_trampoline((lisp_var_n2 = lisp_cons(lisp_int(2), lisp_cons(lisp_var_n5, lisp_var_n6))));
    lisp_trampoline((lisp_var_n1 = lisp_cons(lisp_int(1), lisp_cons(lisp_var_n3, lisp_var_n4))));
    lisp_trampoline((lisp_var_tree_root = lisp_cons(lisp_int(0), lisp_cons(lisp_var_n1, lisp_var_n2))));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Tree built. Root value:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_car(lisp_trampoline(lisp_var_tree_root)))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Left child value:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_car(lisp_trampoline(lisp_car(lisp_trampoline(lisp_cdr(lisp_trampoline(lisp_var_tree_root)))))))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Right child value:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_car(lisp_trampoline(lisp_cdr(lisp_trampoline(lisp_cdr(lisp_trampoline(lisp_var_tree_root)))))))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("Left-Left child value:"))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_car(lisp_trampoline(lisp_car(lisp_trampoline(lisp_cdr(lisp_trampoline(lisp_car(lisp_trampoline(lisp_cdr(lisp_trampoline(lisp_var_tree_root)))))))))))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 3] Heap state AFTER building tree ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_print_stats(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 4] Dropping all references (cutting the root) ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_tree_root = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n1 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n2 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n3 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n4 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n5 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n6 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n7 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_var_n8 = lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("All references set to nil -- tree is now unreachable garbage."))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 5] Forcing GC -- watch the sweep log below ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_collect(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 6] Heap state AFTER GC ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_gc_print_stats(), lisp_alloc(LISP_NIL)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_string("=== [Step 7] Allocating after GC -- heap is healthy ==="))), printf("\n"), lisp_alloc(LISP_T)));
    lisp_trampoline((lisp_var_check = lisp_int(lisp_trampoline(lisp_int(100))->data.int_val + lisp_trampoline(lisp_int(200))->data.int_val)));
    lisp_trampoline((lisp_print(lisp_trampoline(lisp_var_check)), printf("\n"), lisp_alloc(LISP_T)));

    lisp_runtime_shutdown();
    return 0;
}
