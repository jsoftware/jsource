1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g422os.ijs'
NB. x{/:~y (order statistics) -------------------------------------------

(/:~y) -: (i.#y) ({/:~)"0 1 y=:       100 ?@$ 60
(/:~y) -: (i.#y) ({/:~)"0 1 y=: 0.1 * 100 ?@$ 60
(/:~y) -: (i.#y) ({/:~)"0 1 y=:       100 ?@$ 10000
(/:~y) -: (i.#y) ({/:~)"0 1 y=: 0.1 * 100 ?@$ 10000
(/:~y) -: (i.#y) ({/:~)"0 1 y=:       100   $ 10000
(/:~y) -: (i.#y) ({/:~)"0 1 y=:       100   $ 34.5

y=: 1000 ?@$ 600
x=: n -~ 20 ?@$ +:n=: #y
(x{/:~y) -: x ({/:~)"0 1 y
y=: 0.1 * y
(x{/:~y) -: x ({/:~)"0 1 y

NB. Verify special code used
y=: 1000 ?@$ 1e6  NB. avoid small-range!
THRESHOLD +. 0.7 > %/ 10000&(6!:2) '5 ({ /:) y' ,: '5 ({ /:"_) y'    NB. 0.64 on vs2019 avx2
THRESHOLD +. 0.7 > %/ 10000&(6!:2) '5 ({ /:~) y' ,: '5 ({ /:~"_) y'  NB. 0.53 on raspi32 0.68 on vs2019 avx2
y=: 0.1 * y
THRESHOLD +. 0.7 > %/ 10000&(6!:2) '5 ({ /:) y' ,: '5 ({ /:"_) y'    NB. 0.66 on vs2019 avx2
THRESHOLD +. 0.75 > %/ 10000&(6!:2) '5 ({ /:~) y' ,: '5 ({ /:~"_) y' NB. 0.71 on vs2019 avx2

'domain error' -: 'a' ({/:~) etx y

'index error'  -:  1e6 ({/:~) etx y
'index error'  -: _1e6 ({/:~) etx y


4!:55 ;:'n x y'

