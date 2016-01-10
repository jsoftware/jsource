NB. j. ------------------------------------------------------------------

jdot =. 0j1&*

(j. -: jdot) 0.1*_500+?10 20$1000
(j. -: jdot) (?40$100)*^0j1*?40$100

a =. 0.1 * _500 + ?10 20$1000
b =. 0.1 * _500 + ?10 20$1000
(a j. b) -: a+0j1*b
(a j.&(^@j.) b) -: (^0j1*a)+0j1*^0j1*b
(3 j. b ) -: 3+0j1*b
(a j. _4) -: a+0j1*_4

a -: [ &. j. a

'domain error' -: j. etx 'abc'
'domain error' -: j. etx <'abc'

'domain error' -: 'abc' j. etx 3
'domain error' -: 'abc' j.~etx 3
'domain error' -: 4     j. etx <'abc'
'domain error' -: 4     j.~etx <'abc'

'length error' -: 3 4   j. etx 5 6 7
'length error' -: 3 4   j.~etx 5 6 7
'length error' -: 3 4   j. etx i.3 4
'length error' -: 3 4   j.~etx i.3 4


NB. complex numbers -----------------------------------------------------

type =. 3!:0

16     =  type 3j4
9j8    -: +/2j3 7j5
2j_3   -: +2j3

_5j_2  -: -/2j3 7j5
_2j_3  -: -2j3

_1j31  -: */2j3 7j5
(*2j3) -: (%|) 2j3
t      -: *t=.0 0j1 _1 0j_1 1

(29j11%74) -: %/2j3 7j5
(2j_3%13)  -: %2j3

4!:55 ;:'a b jdot t'


