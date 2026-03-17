; === 4.3 Macro System Test ===

; --- Test 1: Simple quasiquote macro (when) ---
; (when cond body) expands to (if cond body nil)
(defmacro when (cond body)
  `(if ,cond ,body nil))

(print "=== Test 1: when macro (quasiquote) ===")
(setq x 10)
(when (> x 5)
  (print "x is greater than 5"))

; --- Test 2: Compile-time logic macro (unless) ---
(defmacro unless (cond body)
  `(if (not ,cond) ,body nil))

(print "=== Test 2: unless macro (compile-time not) ===")
(unless (= x 0)
  (print "x is not zero"))

; --- Test 3: Macro with compile-time arithmetic ---
; (inc-by var amount) expands to (setq var (+ var amount))
(defmacro inc-by (var amount)
  `(setq ,var (+ ,var ,amount)))

(print "=== Test 3: inc-by macro (compile-time code generation) ===")
(setq counter 0)
(inc-by counter 5)
(inc-by counter 3)
(print counter)

; --- Test 4: c-code macro — directly generates a raw C expression ---
; msg is a symbol; symbol->string converts it, then string-append builds the C call
(defmacro fast-print (msg)
  (c-code (string-append "lisp_int(puts(\"" (symbol->string msg) "\"))")))

(print "=== Test 4: fast-print macro (direct C code generation) ===")
(fast-print Hello_from_macro_generated_C)

; --- Test 5: Nested macro expansion ---
(defmacro swap (a b)
  `(let ((tmp ,a))
     (progn
       (setq ,a ,b)
       (setq ,b tmp))))

(print "=== Test 5: swap macro (nested quasiquote) ===")
(setq p 100)
(setq q 200)
(swap p q)
(print p)
(print q)
