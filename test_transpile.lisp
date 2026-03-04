;; 1. Test Control Flow & Recursion (Fibonacci)
(defun fib (n)
  (if (< n 2)
      n
      (+ (fib (- n 1)) (fib (- n 2)))))

(print "Fibonacci of 6 is:")
(print (fib 6))

;; 2. Test List Operations
(setq my_list (cons 1 (cons 2 (cons 3 nil))))
(print "My List:")
(print my_list)

(print "CAR of My List:")
(print (car my_list))

(print "CDR of My List:")
(print (cdr my_list))