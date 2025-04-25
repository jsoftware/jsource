prolog './g202b.ijs'
NB. ^: with boxed right argument ----------------------------------------

(>:   ^:(i.1000)  1     ) -: >:   ^:(<1000)  1
(>:   ^:(i.1000)  1x    ) -: >:   ^:(<1000)  1x
(>:&.>^:(i.1000) <1     ) -: >:&.>^:(<1000) <1

NB. obsolete (>:   ^:(i.   0)  1     ) -: >:   ^:(<   0)  1
NB. obsolete (>:   ^:(i.   0)  1x    ) -: >:   ^:(<   0)  1x
NB. obsolete (>:&.>^:(i.   0) <1     ) -: >:&.>^:(<   0) <1
'domain error' -: ex '>:&.>^:(<   0) <1'

(>:   ^:(i.1000)  1 4 9 ) -: >:   ^:(<1000)  1 4 9
(>:   ^:(i.1000)  1 4 9x) -: >:   ^:(<1000)  1 4 9x
(>:&.>^:(i.1000) <1 4 9 ) -: >:&.>^:(<1000) <1 4 9

NB. obsolete (>:   ^:(i.   0)  1 4 9 ) -: >:   ^:(<   0)  1 4 9
NB. obsolete (>:   ^:(i.   0)  1 4 9x) -: >:   ^:(<   0)  1 4 9x
NB. obsolete (>:&.>^:(i.   0) <1 4 9 ) -: >:&.>^:(<   0) <1 4 9

f=: ^:a:
'^:a:' -: 5!:5 <'f'

spleak=: 3 : 0
 x2288=. 7!:0 ''
 y2288=. 7!:0 ''
 x2288=. 7!:0 ''
 ". y
 assert. 1  NB. Force xdefn to tpop
 y2288=. 7!:0 ''
 if. x2288 ~: y2288 do. 0 [ x2288__ =: x2288 [ y2288__ =: y2288 else. 1 end.
)

spleak '>:   ^:(<1000)  1x'
spleak '>:&.>^:(<1000) <1 '

'domain error' -: ex '>:^:(<2.5)'
'domain error' -: ex '>:^:(<2j5)'
'domain error' -: ex '>:^:(<2r5)'


f=: 1 : '] [ echo@(m ; < )'
x=: 2 : 'u^:(1:`(n f))'
test=: 1 (< 3 2 $ 0 0 0 1 1 2)} 1 $. 2 3 ; 0 1 ; 0
sbok=: {{try. 1 [ $.<0 catch. 0 end. }}''
1:^:sbok 'nonce error' -: (4&$. x 'mm') etx test

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

0 2 4 9 10 12 _1 -: (2 0 4 0 9 0 0 0 0 10 12 0 _1 {~^:a: 0)
0 2 4 -: (2 0 4 0 9 0 0 0 0 10 12 0 _1 {~^:(<3) 0)

(1 0$0) -: {&1 2 3^:a: $0

'limit error' -: {&1 2 3 0 4^:a: etx 0
'limit error' -: {&1 2 3 0 4^:a: etx 0 1

4!:55 ;:'boxed_rec f indexlim r randomfile rec rec1 sbok spleak'
4!:55 ;:'test x x2288 y2288'



epilog''

