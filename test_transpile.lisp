;; 1. Testing Short-circuit Evaluation (Logic)
(print "--- Testing Logic ---")
(and (print "AND: This will print") 
     nil 
     (print "AND: This MUST NOT print! (Short-circuited)"))

(or (print "OR: This will print") 
    (print "OR: This MUST NOT print! (Short-circuited)"))

;; 2. Testing Iteration (While Loop)
(print "--- Testing Iteration ---")
(setq counter 0)

(while (< counter 5)
  (progn
    (print "Counter is currently:")
    (print counter)
    (setq counter (+ counter 1))))

(print "Loop finished!")