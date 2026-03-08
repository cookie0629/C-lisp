;; Assign a lambda to a variable
(setq my_adder (lambda (x y) (+ x y)))

(print "Testing First-Class Lambda:")
;; Execute the lambda via funcall
(print (funcall my_adder 100 25))

;; Passing a lambda dynamically
(setq execute_op (lambda (op a b) (funcall op a b)))
(print "Testing Higher-Order Function:")
(print (funcall execute_op my_adder 50 50))