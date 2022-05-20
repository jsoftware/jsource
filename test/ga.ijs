prolog './ga.ijs'
NB. a. ------------------------------------------------------------------

randuni''

2           -: type a.
1           -: #$a.
256         -: #a.
(i.256)     -: i.~a.
32 48 65 97 -: a.i.' 0Aa'


NB. a: ------------------------------------------------------------------

0    -: #$a:
32   -: 3!:0 a:
($0) -: >a:
a:   -: <$0
a:   -: <''

'a:' -: 3 : '5!:5<''y''' a:


NB. A. ------------------------------------------------------------------

boxed =: 32&=@type
pind  =: ]`]`+@.(*@])"0
pfill =: [ ((i.@[-.]) , ]) pind

ord   =: >:@(>./)
base  =: >:@i.@-@#
rfd   =: +/@({.>}.)\.
dfr   =: /:^:2@,/

Adot1 =: (base #. rfd)@((ord pfill ])`C.@.boxed) " 1
Adot2 =: dfr@(base@] #: [) { ]

(A. -: Adot1) 7?12
(A. -: Adot1) x=:(1=1,?6$3) <;.1 (7?12)

(?!5) (A. -: Adot2) 'xyzab'
(?!5) (A. -: Adot2) u:'xyzab'
(?!5) (A. -: Adot2) 10&u:'xyzab'
(?!5) (A. -: Adot2) r.i.5 2

5 -: A.0 3 2 1
5 -: A.3 2 1
5 -: A.0;2;3 1
5 -: A.<3 1

(_1 A. y) -: |. y=:1=?300 2$2
(_1 A. y) -: |. y=:(?400$#a.){a.
(_1 A. y) -: |. y=:(?400$#adot1){adot1
(_1 A. y) -: |. y=:(?400$#adot2){adot2
(_1 A. y) -: |. y=:(?400$#sdot0){sdot0
(_1 A. y) -: |. y=:?300$2000
(_1 A. y) -: |. y=:o.?400 2$100
(_1 A. y) -: |. y=:^0j1*?400 2$100
(_1 A. y) -: |. y=:^0j1*?400 2$100
(_1 A. y) -: |. y=:(?400$#y){y=:;:'^0j1*?400 2$100'

(0 A. y) -: y=:1=?2
(0 A. y) -: y=:(?#a.){a.
(0 A. y) -: y=:(?#adot1){adot1
(0 A. y) -: y=:(?#adot2){adot2
(0 A. y) -: y=:(?#sdot0){sdot0
(0 A. y) -: y=:?20000
(0 A. y) -: y=:o.?20000
(0 A. y) -: y=:^0j1*?2000
(0 A. y) -: y=:<^0j1*?2000

(_1 A. y) -: y=:1=?2
(_1 A. y) -: y=:(?#a.){a.
(_1 A. y) -: y=:(?#adot1){adot1
(_1 A. y) -: y=:(?#adot2){adot2
(_1 A. y) -: y=:(?#sdot0){sdot0
(_1 A. y) -: y=:?20000
(_1 A. y) -: y=:o.?20000
(_1 A. y) -: y=:^0j1*?2000
(_1 A. y) -: y=:<^0j1*?2000

(0 A. y) -: y=:i.0 4 5
(0 A. y) -: y=:0 5$a.
(0 A. y) -: y=:0 5$adot1
(0 A. y) -: y=:0 5$adot2
(0 A. y) -: y=:0$<1234

([ -: 3&A.^:_1@(3&A.)) x=:?100$10000
([ -: 3&A.^:_1@(3&A.)) x=:?20 4$100
([ -: 3&A.^:_1@(3&A.)) x=:(?200$3){;:'Hey nonny nonny'
([ -: 3&A.^:_1@(3&A.)) x=:(?200$3){(u:&.>) ;:'Hey nonny nonny'
([ -: 3&A.^:_1@(3&A.)) x=:(?200$3){(10&u:&.>) ;:'Hey nonny nonny'
([ -: 3&A.^:_1@(3&A.)) x=:(?200$3){s:@<"0&.> ;:'Hey nonny nonny'
([ -: 3&A.^:_1@(3&A.)) x=:(?200$3){<"0@s: ;:'Hey nonny nonny'

([ -: A.&y^:_1@(A.&y)) x=:?100$#y=:~.'Antebellum'
([ -: A.&y^:_1@(A.&y)) x=:?100$#y=:~.u:'Antebellum'
([ -: A.&y^:_1@(A.&y)) x=:?100$#y=:~.10&u:'Antebellum'
([ -: A.&y^:_1@(A.&y)) x=:?100$#y=:~.s:@<"0 'Antebellum'
([ -: A.&y^:_1@(A.&y)) x=:?100$#y=:100?100

0 -: A. i.0
(i.1 0) -: (i.1) A. i.0

(3 4$0) -: A."0 ]3 4 ?@$ 100

'domain error' -: A. etx 'abcd'
'domain error' -: A. etx 3 4;'abc'
'domain error' -: A. etx 3.4 5
'domain error' -: A. etx 3j4 5

'domain error' -: 'ab' A. etx i.4 
'domain error' -: 3.5  A. etx i.4 
'domain error' -: 3j5  A. etx i.4 
'domain error' -: (<5) A. etx i.4
  
'domain error' -: A.&1 2 2^:_1 etx 4

'index error'  -:  24 A. etx 'abcd' 
'index error'  -: _25 A. etx 'abcd'
 

NB. A. encore -----------------------------------------------------------

p0 =: i.@! A. i.

j=:?~!5
p=:j{p0 5
j -: A.p
j -: A.@C.p
p -: j A.i.5

grow =: [: ,/ 0&,.@:>: {"2 1 \:"1@=@(_1&,)@{.

p1 =: 1 0&$`([: ,/ 0&,.@($:&.(<:"_)) {"2 1 \:"1@=@i.) @. *
p2 =: grow^:(]`(1 0&$))
p3 =: 3 : 'grow^:y i.1 0'

p4 =: 3 : 0
 z=. i.1 0
 for. i.y do. z=.,/(0,.1+z){"2 1\:"1=i.>:{:$z end.
)

(p0 -: p1)"0 i.6
(p0 -: p2)"0 i.6
(p0 -: p3)"0 i.6
(p0 -: p4)"0 i.6


4!:55 ;:'Adot1 Adot2 adot1 adot2 sdot0 base boxed dfr grow j ord p p0 '
4!:55 ;:'p1 p2 p3 p4 pfill pind rfd x y z '
randfini''


epilog''

