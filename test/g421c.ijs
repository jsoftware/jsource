NB. x </. i.#x ----------------------------------------------------------

test=: 3 : 0
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~?1000  $#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~ 1000  $0
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~    0  $#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~?1000 2$#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~?1000 5$#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~?1000 5$#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~ 1000 5$0
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: y{~    0 5$#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: 0#y
 assert. (x</.i.#x) -: <:&.>x </.>:i.#x=: '' ($,)y
 1
)

test 0 1
test a.
test      i.900
test _450+i.900
test  1e6+i.900
test _1e6+i.900
test u: i.65536
test -:      i.900
test -: _450+i.900
test <"0 ?40$100

'length error' -: 1 2 3 </. etx i.4 

test1=: 3 : 0
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~?1000  $#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~ 1000  $0
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~    0  $#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~?1000 2$#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~?1000 5$#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~?1000 5$#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~ 1000 5$0
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: y{~    0 5$#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: 0#y
 assert. ((</.i.@#) x) -: <:&.>x </.>:i.#x=: '' ($,)y
 1
)

test1 0 1
test1 a.
test1      i.900
test1 _450+i.900
test1  1e6+i.900
test1 _1e6+i.900
test1 u: i.65536
test1 -:      i.900
test1 -: _450+i.900
test1 <"0 ?40$100


4!:55 ;:'test test1 x'

