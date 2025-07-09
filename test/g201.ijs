prolog './g201.ijs'
NB. ^.y -----------------------------------------------------------------

log =: ^.@| j. 12&o.

f =: ^. -: log

f  3j4 3j_4 _3j4 _3j_4
f %3j4 3j_4 _3j4 _3j_4
f 0
f  3 _3 0j4 0j_4
f %3 _3 0j4 0j_4

__ 0 1 10 -: ^.0 1 2.71828182845904523536 22026.465794806717

1e_15 > | 0.69314718055994530941 - ^. 2
1e_15 > | 1.09861228866810969139 - ^. 3
1e_15 > | 2.30258509299404568401 - ^.10

'domain error' -: ^. etx 'abc'
'domain error' -: ^. etx <'abc'


NB. x^.y ----------------------------------------------------------------

NB. (__ j. 0) -:&(3!:3) {. 10^. 0 _1
(__ j. 0) -: {. 10^. 0 _1   NB. either imaginary 0 or -0 is good
(i.10) = x^."1] 1 , */\ 9#,:x=: 2+?15$20

'domain error' -: 3   ^. etx 'abc'
'domain error' -: 3   ^.~etx 'abc'
'domain error' -: 3   ^. etx u:'abc'
'domain error' -: 3   ^.~etx u:'abc'
'domain error' -: 3   ^. etx 10&u:'abc'
'domain error' -: 3   ^.~etx 10&u:'abc'
'domain error' -: 3   ^. etx s:@<"0 'abc'
'domain error' -: 3   ^.~etx s:@<"0 'abc'
'domain error' -: 2   ^. etx 2;4 5
'domain error' -: 2   ^.~etx 2;4 5

'length error' -: 2 3 ^. etx 4 5 6
'length error' -: 2 3 ^.~etx 4 5 6
'length error' -: 2 3 ^. etx i.4 5
'length error' -: 2 3 ^.~etx i.4 5



epilog''

