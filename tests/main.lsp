(def n 2)
(debug n)
(debug n)
(set n 3)
(debug n)
(set n 7)
(debug n)

(debug 5 #t n)

(def k #f)

(def m (+ 2 (- 3 7)))

(debug m)

(debug (/ m (- 3 11)))

(def circle_area (* 3.142 7 7))

(debug circle_area)

(debug (if #t 1 0))

(fun add (a b) ((+ a b)))

(def n (add 99 3))

(debug n (add 0.1 0.2))
