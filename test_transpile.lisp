(print "=== 1. Variables & Lexical Scope ===")
(setq global_var 100)
(let ((local_var 25))
  (progn
    (print "Local + Global:")
    (print (+ global_var local_var))))

(print "=== 2. Logic & Short-Circuit Evaluation ===")
(and (print "AND Test: Pass 1") nil (print "AND Test: FAIL (Should short-circuit!)"))
(or nil (print "OR Test: Pass 1") (print "OR Test: FAIL (Should short-circuit!)"))

(print "=== 3. Iteration (While) ===")
(setq i 0)
(while (< i 3)
  (progn
    (print i)
    (setq i (+ i 1))))

(print "=== 4. Lists & Higher-Order Functions ===")
(setq my_list (cons 10 (cons 20 (cons 30 nil))))
(print "CDR of CAR:")
(print (car (cdr my_list)))

(setq double_func (lambda (x) (* x 2)))
(setq apply_func (lambda (f x) (funcall f x)))
(print "Passing Function as Data:")
(print (funcall apply_func double_func 500))

(print "=== 5. Inline C Execution (Super Macro) ===")
(c-stmt "puts(\"Hello from native C layer! Pointers and registers welcome!\")")
(print "C Math Injected:")
(print (c-expr "lisp_int(1 << 10)"))

(print "=== 6. Tail Call Optimization (TCO) ===")
;; 这个函数在没有 TCO 的编译器上运行 50000 次会直接触发 C 栈溢出崩溃
(defun tail-sum (n acc)
  (if (<= n 0)
      acc
      (tail-sum (- n 1) (+ acc n))))

(print "Sum from 1 to 50000 (Testing Trampoline O(1) Memory):")
(print (tail-sum 50000 0))