(setq x 100)

(print "Global x is:")
(print x)

(print "Testing PROGN and LET:")

(let ((x 10) 
      (y 20))
  (progn
    (print "Local x in let block is:")
    (print x)
    (print "Sum of local x and y is:")
    (+ x y)))

(print "Global x after let is still:")
(print x)