
(defun 1+ (x)
  (+ x 1))

(defun 1- (x)
  (- x 1))

(defmacro incf (place)
  `(setq ,place (1+ ,place)))
