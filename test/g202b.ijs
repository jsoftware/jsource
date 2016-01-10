NB. ^: with boxed right argument ----------------------------------------

(>:   ^:(i.1000)  1     ) -: >:   ^:(<1000)  1
(>:   ^:(i.1000)  1x    ) -: >:   ^:(<1000)  1x
(>:&.>^:(i.1000) <1     ) -: >:&.>^:(<1000) <1

(>:   ^:(i.   0)  1     ) -: >:   ^:(<   0)  1
(>:   ^:(i.   0)  1x    ) -: >:   ^:(<   0)  1x
(>:&.>^:(i.   0) <1     ) -: >:&.>^:(<   0) <1

(>:   ^:(i.1000)  1 4 9 ) -: >:   ^:(<1000)  1 4 9
(>:   ^:(i.1000)  1 4 9x) -: >:   ^:(<1000)  1 4 9x
(>:&.>^:(i.1000) <1 4 9 ) -: >:&.>^:(<1000) <1 4 9

(>:   ^:(i.   0)  1 4 9 ) -: >:   ^:(<   0)  1 4 9
(>:   ^:(i.   0)  1 4 9x) -: >:   ^:(<   0)  1 4 9x
(>:&.>^:(i.   0) <1 4 9 ) -: >:&.>^:(<   0) <1 4 9

f=: ^:a:
'^:a:' -: 5!:5 <'f'

spleak=: 3 : 0
 x2288=: 7!:0 ''
 y2288=: 7!:0 ''
 x2288=: 7!:0 ''
 ". y
 y2288=: 7!:0 ''
 assert. x2288 = y2288
 1
)

spleak '>:   ^:(<1000)  1x'
spleak '>:&.>^:(<1000) <1 '

'domain error' -: >:^:(<2.5) etx 1
'domain error' -: >:^:(<2j5) etx 1
'domain error' -: >:^:(<2r5) etx 1


NB. {&x^:n with boxed n -------------------------------------------------

x=: (# ,~ # <. ]) (+ i.@#) 1+1000 ?@$ 10

indexlim=: 4 : 0
 s=. 1+t=. y
 z=. ,:y
 while. -.s-:t do.
  z=. z, t=. t{x [ s=. t
 end.
 }: z
)

({&x^:a: -: x&indexlim) 0 
({&x^:a: -: x&indexlim) 6 4 9

x ({~^:a: -: indexlim) 0 
x ({~^:a: -: indexlim) 6 4 9

spleak '{&x^:a: 0'
spleak '{&x^:a: 6 4 9'

spleak 'x {~^:a: 0'
spleak 'x {~^:a: 6 4 9'

NB. Each record of a file begins with a byte indicating the record length 
NB. (excluding the record length byte itself), followed by the record contents. 
NB. Given a file, the verb rec below produces the list of boxed records.

rec=: 3 : 0
 n=. #y
 d=. _1 ,~ n<.1+(i.n)+a.i.y
 m=. {&d^:a: 0
 ((i.n) e. m) <;._1 y
)

rec1=: 3 : 0
 n=. #y
 d=. _1 ,~ n<.1+(i.n)+a.i.y
 m=. d {~^:a: 0
 ((i.n) e. m) <;._1 y
)

randomfile=: 3 : 0
 c  =. 1+y ?@$ 255           NB. record lengths
 rec=. {&a.&.> c ?@$&.> 256  NB. record contents
 (c{a.),&.> rec              NB. records with lengths
)

test=: 3 : 0
 boxed_rec=: randomfile y
 assert. r -: }.&.> boxed_rec [ r=: rec  ; boxed_rec
 assert. r -: }.&.> boxed_rec [ r=: rec1 ; boxed_rec
 1
)

test"0 ] 10^i.4


4!:55 ;:'boxed_rec f indexlim r randomfile rec rec1 spleak'
4!:55 ;:'test x x2288 y2288'


