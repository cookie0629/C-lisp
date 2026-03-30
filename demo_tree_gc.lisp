; ================================================================
;  GC Tree Demo -- build a tree, drop the root, watch recursive sweep
; ================================================================
;
;  Tree structure (cons-encoded binary tree):
;
;              root(0)
;             /       \
;           n1(1)     n2(2)
;          /    \    /    \
;        n3(3) n4(4) n5(5) n6(6)
;        /  \
;      n7(7) n8(8)
;
;  Each node = (cons value (cons left right))
; ================================================================

; --- Helper macros ---
(defmacro make-node (val left right)
  `(cons ,val (cons ,left ,right)))

(defmacro node-val (node)
  `(car ,node))

(defmacro node-left (node)
  `(car (cdr ,node)))

(defmacro node-right (node)
  `(cdr (cdr ,node)))

; ================================================================
;  Step 1: initial heap state
; ================================================================
(print "=== [Step 1] Initial heap state ===")
(c-stmt "lisp_gc_print_stats()")

; ================================================================
;  Step 2: build the 8-node binary tree
; ================================================================
(print "=== [Step 2] Building 8-node binary tree ===")

(setq n7 (make-node 7 nil nil))
(setq n8 (make-node 8 nil nil))
(setq n4 (make-node 4 nil nil))
(setq n5 (make-node 5 nil nil))
(setq n6 (make-node 6 nil nil))
(setq n3 (make-node 3 n7 n8))
(setq n2 (make-node 2 n5 n6))
(setq n1 (make-node 1 n3 n4))
(setq tree_root (make-node 0 n1 n2))

(print "Tree built. Root value:")
(print (node-val tree_root))
(print "Left child value:")
(print (node-val (node-left tree_root)))
(print "Right child value:")
(print (node-val (node-right tree_root)))
(print "Left-Left child value:")
(print (node-val (node-left (node-left tree_root))))

(print "=== [Step 3] Heap state AFTER building tree ===")
(c-stmt "lisp_gc_print_stats()")

; ================================================================
;  Step 4: drop ALL references -- the entire tree becomes garbage
; ================================================================
(print "=== [Step 4] Dropping all references (cutting the root) ===")
(setq tree_root nil)
(setq n1 nil)
(setq n2 nil)
(setq n3 nil)
(setq n4 nil)
(setq n5 nil)
(setq n6 nil)
(setq n7 nil)
(setq n8 nil)
(print "All references set to nil -- tree is now unreachable garbage.")

; ================================================================
;  Step 5: force GC -- the sweep log will show every reclaimed node
; ================================================================
(print "=== [Step 5] Forcing GC -- watch the sweep log below ===")
(c-stmt "lisp_gc_collect()")

(print "=== [Step 6] Heap state AFTER GC ===")
(c-stmt "lisp_gc_print_stats()")

; ================================================================
;  Step 7: prove heap is healthy after GC
; ================================================================
(print "=== [Step 7] Allocating after GC -- heap is healthy ===")
(setq check (+ 100 200))
(print check)
