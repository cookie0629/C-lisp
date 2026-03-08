(print "--- Stage 6: Inline C Magic ---")

;; 1. Injecting a C statement directly! 
;; We use native C's puts() function instead of our Lisp print.
(c-stmt "puts(\"Hello from Native C! I bypassed the Lisp runtime!\")")

;; 2. Injecting a C expression!
;; We write a raw C pointer and math operation, wrapped in lisp_int() so Lisp can read it.
(setq hacker_var (c-expr "lisp_int(1000 + 337)"))

(print "Lisp read the injected C result:")
(print hacker_var)