; ================================================================
;  DEMO: GC Garbage Collection + Complex Macro Definitions
; ================================================================


; ================================================================
;  PART 1 — Complex Macro Definitions
; ================================================================

; --- Macro 1: for loop ---
; (for var from to body) compiles to (setq var from) + while loop
(defmacro for (var from to body)
  `(progn
     (setq ,var ,from)
     (while (<= ,var ,to)
       (progn
         ,body
         (setq ,var (+ ,var 1))))))

; --- Macro 2: assert ---
; (assert cond msg) — if cond is false, prints error message and exits
(defmacro assert (cond msg)
  `(if (not ,cond)
       (progn
         (print ,msg)
         (c-stmt "exit(1)"))
       nil))

; --- Macro 3: repeat ---
; (repeat n body) — execute body n times
(defmacro repeat (n body)
  `(progn
     (setq _repeat_i 0)
     (while (< _repeat_i ,n)
       (progn
         ,body
         (setq _repeat_i (+ _repeat_i 1))))))

; --- Macro 4: square ---
; Inline arithmetic — expands to (* x x) at compile time
(defmacro square (x)
  `(* ,x ,x))

; --- Macro 5: max2 ---
; Returns the larger of two values via compile-time if expansion
(defmacro max2 (a b)
  `(if (> ,a ,b) ,a ,b))


; ================================================================
;  PART 2 — Calling the Complex Macros
; ================================================================

(print "=== Macro Demo: for loop (1 to 5) ===")
(for i 1 5
  (print i))

(print "=== Macro Demo: repeat (3 times) ===")
(repeat 3
  (print "hello from repeat"))

(print "=== Macro Demo: square & max2 ===")
(setq val 7)
(print (square val))
(print (max2 3 9))
(print (max2 100 42))

(print "=== Macro Demo: assert (should pass) ===")
(assert (> 10 0) "FAIL: 10 > 0 should be true")
(print "assert passed OK")


; ================================================================
;  PART 3 — GC Garbage Collection Demo
; ================================================================

(print "")
(print "=== GC Demo: Heap state BEFORE allocation ===")
(c-stmt "lisp_gc_print_stats()")

; Build a large temporary list of 300 cons cells inside a for loop.
; After the loop, big_list is still referenced by the global var,
; so we explicitly set it to nil to drop the reference, then force GC.
(setq big_list nil)
(for k 1 300
  (setq big_list (cons k big_list)))

(print "=== GC Demo: Heap state AFTER building 300-element list ===")
(c-stmt "lisp_gc_print_stats()")

; Drop the reference — big_list is now unreachable garbage
(setq big_list nil)
(print "=== GC Demo: Dropped reference. Forcing collection... ===")
(c-stmt "lisp_gc_collect()")

(print "=== GC Demo: Heap state AFTER GC collection ===")
(c-stmt "lisp_gc_print_stats()")

; Prove the heap is healthy — allocate and compute normally after GC
(setq result (+ 1000 337))
(print "Allocation after GC works fine:")
(print result)
