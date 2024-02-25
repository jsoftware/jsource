prolog './gj.ijs'
NB. j. ------------------------------------------------------------------

jdot =: 0j1&*

(j. -: jdot) 0.1*_500+?10 20$1000
(j. -: jdot) (?40$100)*^0j1*?40$100

a =: 0.1 * _500 + ?10 20$1000
b =: 0.1 * _500 + ?10 20$1000
(a j. b) -: a+0j1*b
(a j.&(^@j.) b) -: (^0j1*a)+0j1*^0j1*b
(3 j. b ) -: 3+0j1*b
(a j. _4) -: a+0j1*_4

a -: [ &. j. a

'domain error' -: j. etx 'abc'
'domain error' -: j. etx <'abc'
'domain error' -: j. etx u:'abc'
'domain error' -: j. etx <u:'abc'
'domain error' -: j. etx 10&u:'abc'
'domain error' -: j. etx <10&u:'abc'

'domain error' -: 'abc' j. etx 3
'domain error' -: 'abc' j.~etx 3
'domain error' -: 4     j. etx <'abc'
'domain error' -: 4     j.~etx <'abc'
'domain error' -: (u:'abc') j. etx 3
'domain error' -: (u:'abc') j.~etx 3
'domain error' -: 4     j. etx <u:'abc'
'domain error' -: 4     j.~etx <u:'abc'
'domain error' -: (10&u:'abc') j. etx 3
'domain error' -: (10&u:'abc') j.~etx 3
'domain error' -: 4     j. etx <10&u:'abc'
'domain error' -: 4     j.~etx <10&u:'abc'

'length error' -: 3 4   j. etx 5 6 7
'length error' -: 3 4   j.~etx 5 6 7
'length error' -: 3 4   j. etx i.3 4
'length error' -: 3 4   j.~etx i.3 4


NB. complex numbers -----------------------------------------------------

16     =  type 3j4
9j8    -: +/2j3 7j5
2j_3   -: +2j3

_5j_2  -: -/2j3 7j5
_2j_3  -: -2j3

_1j31  -: */2j3 7j5
(*2j3) -: (%|) 2j3
t      -: *t=:0 0j1 _1 0j_1 1

(29j11%74) -: %/2j3 7j5
(2j_3%13)  -: %2j3

NB. -0
a =: 1 % __  NB. -0
__ -: % a
a = 0.
__ __ -: % +. a j. a
_ __ -: % +. 0 j. a
__ _ -: % +. a j. 0
_ _ -: % +. 0 j. 0
1 = # ~. , % +. a j. 3 # a
2 = # ~. , % +. a j. 3 # 0.

f =: (j./"1@:(+"1) -: +&:(j./"1))  NB. test cmplx add.  Last axis must be length 2
f2 =: {{  NB. x iterations, max axis length y
 for. i. x do.
  rs =. 1 >. ? y  NB. max shape
  xs =. (- 0.2 0.5 0.2 _ I. ?0) }. rs [ ys =. (- 0.2 0.5 0.2 _ I. ?0) }. rs
  xd =: (xs,2) ?@$ 0 [ yd =: (ys,2) ?@$ 0
  assert. xd u yd
 end.
1
}}
1000 (f f2) 5 5 5 20
f =: (j./"1@:(-"1) -: -&:(j./"1))  NB. test cmplx asub.  Last axis must be length 2
1000 (f f2) 5 5 5 20

0j0 = 1j0 % _j_

0!:_1 '?'  NB. discard sentences till NB.?
0
13!:5 ] 8
NB.?

NB. Tests for qp
'?' 0!:_1@[^:(-. 'SLEEF' +./@:E. 9!:14'') 1  NB. skip if not SLEEF

NB. tests requiring conforming arguments, qp vs xnum
argrand =: {{  NB. x is max value (extended).  y is (max sizes),:(cumulative freq for probability of each rank)  args are large random integers
xs =: (({: y) I. ?0) {. s [ys =: (({: y) I. ?0) {. s [ s =: ? {.y  NB. conforming shapes
sf =. 0 i.&1@:~: {: y
xs =: (sf {. ys) , sf }. xs
dx =: 8 c. qx =: 11 c. xx =: xs ?@$ x [ dy =: 8 c. qy =: 11 c. xy =: ys ?@$ x  NB. exactly representable integers
dx;xx;qx;dy;xy;qy
}}
argnear =: {{  NB. x is max value (extended).  y is (max sizes),:(cumulative freq for probability of each rank)  args are large random in a small range
xs =: (({: y) I. ?0) {. s [ys =: (({: y) I. ?0) {. s [ s =: ? {.y  NB. conforming shapes
base =. ?x  NB. random big #
dx =: 8 c. qx =: 11 c. xx =: base + xs ?@$ 10 [ dy =: 8 c. qy =: 11 c. xy =: base + ys ?@$ 10  NB. exactly representable integers
dx;xx;qx;dy;xy;qy
}}

f =: {{
'dx xx qx dy xy qy' =. y
NB. The tests
assert. qx -:!.0 xx
assert. xy -:!.0 qy
assert. (>:qy) -:!.0 >: xy
assert. (<:qy) -:!.0 <: xy
assert. (|qy) -:!.0 | xy
assert. (^.qy) -: ^.xy
assert. (+/"1 qy) -: +/"1 xy 
assert. (+/"2 qy) -: +/"2 xy 
assert. (+/"3 qy) -: +/"3 xy 
NB. assert. (0&<: *. qx&>:) qy - qx * (<.!.0 qy%qx)
NB. assert. (0&<: *. qx&>:) qy -~ qx * (>.!.0 qy%qx)
assert. ((0&<: *. qx&>:) qy - qx * (<.!.0 qy%qx)) +. ((0&<: *. qx&>:) qy -~ qx * (>.!.0 qy%qx))
assert. (+:qy) -:!.0 +: xy
assert. (-:qy) -:!.0 -: xy
assert. 2 > | (*:%:qy) - xy
assert. (>:qy) -:!.0 >: xy
assert. (<:qy) -:!.0 <: xy
assert. (%:qy) -: %:xy
assert. (qx + qy) -:!.0 xx+xy
assert. (qx - qy) -:!.0 xx-xy
assert. (qx * qy) -: xx*xy
assert. (qx % qy) -: xx%xy
assert. (qx + qy) -:!.0 xx+xy
assert. (dx = dy) -: qx = qy
assert. (dx ~: dy) -: qx ~: qy
assert. (dx < dy) -: qx < qy
assert. (dx <: dy) -: qx <: qy
assert. (dx > dy) -: qx > qy
assert. (dx >: dy) -: qx >: qy
assert. (qx =!.0 qy) -: xx = xy
assert. (qx ~:!.0 qy) -: xx ~: xy
assert. (qx <!.0 qy) -: xx < xy
assert. (qx <:!.0 qy) -: xx <: xy
assert. (qx >!.0 qy) -: xx > xy
assert. (qx >:!.0 qy) -: xx >: xy
assert. (qx >. qy) -:!.0 xx >. xy
assert. (qx <. qy) -:!.0 xx <. xy
assert. (qx | qy) -:!.0 qx |&(8&c.) qy
1
}}
fsmall =: {{
'dx xx qx dy xy qy' =. y
NB. The tests
assert. (*:qy) -:!.0 *: xy
assert. (qx >:!.0 qy) -: xx >: xy
1
}}

f (2^100x) argrand 10 10 10 10 ,: 0.05 0.3 0.6 0.9
f (2^100x) argnear 10 10 10 10 ,: 0.05 0.3 0.6 0.9
f (2^55x) argrand 10 10 10 10 ,: 0.05 0.3 0.6 0.9
f (2^55x) argnear 10 10 10 10 ,: 0.05 0.3 0.6 0.9
fsmall (2^40x) argrand 10 10 10 10 ,: 0.05 0.3 0.6 0.9

{{ (-:!.0&(+/))&>/ 4 5 { (2^90x) argrand y ,: 0 }}"0 (100) $ 200

{{ 1e_28 >  |@(-/"1 % >./) (+/@:*"1)&>/"1&((1 4,:2 5)&{) (2^90x) argrand y ,: 0 }}"0 (20) $ 40

NB. non-atomic arithmetic

(%. -: %.&(11&c.)) a =: 2 2 $ 3. 1 1 3
(-/ . * -: -/ . *&(11&c.)) a
(b =:2 2 $ 1 4 4 2) (%. -: %.&(11&c.)) a
b (-/ . * -: -/ . *&(11&c.)) a
b (+/ . * -: +/ . *&(11&c.)) a
(b =:4 2 $ 1 4 4 2 3 8 2 4) (%. -: %.&(11&c.)) a =: 4 2 $ 3. 1 1 3 2 4 7 6

'domain error' -: ". etx '? 2^20 +&(11&c.) 0.5'
1: ". etx '? 2^20 +&(11&c.) 0.'  NB. ok if integer

a =: 1. 2 3 +&(11&c.) 1e_17 * _0.5 + 3 ?@$ 0

b =: 1 +&(11&c.) 1e_17 * _0.5 + '' ?@$ 0
1e_27 >!.0 | (1 + b * 2 + b * 3) - 1 2 3 p. b
b =: 1 +&(11&c.) -4.3684341099157587388856542887e_18
1e_27 >!.0 | (1 + b * 2 + b * 3) - 1 2 3 p. b

NB. ordering

a =: , (1e15 + 1 2 3 5 6) +/&(11&c.) /:~ ~.!.0 ] 1e_10 * 10 ?@$ 0
b =: , (1e15 + 1 2 3 5 6) +/&(11&c.) 1e_10 * 10 ?@$ 0
a (I. -: +/@:(<:!.0)"1 0) b
a (I. -: (1: i.~ >!.0)"1 0) b
a (I. -: (1 i.~ >!.0)"1 0) b

a -:!.0 /:~ ({~ ?~@#) a
0 < 2 -~/\ /:~ ({~ ?~@#) a

NB. i.-family

a =: +/ |: ,/ 11 c. ((] ,"0"0 1 *)   (2e15 + 1e9 ?@$~ #)) (2^_53) * _0.5 + 100 100 ?@$ 0  NB. 100 rows, each row close together
b =: ((] ,"0&:(8&c.) -)   ([: 11&c. 8&c.)) a
a -:!.0 +/ |: (11) c. b  NB. verify perfect split
c =: +/ |: ,/ 11 c. ((] ,"0"0 1 *)   ([: - 2e15 + 1e9 ?@$~ #)) (2^_53) * _0.5 + 10 10 ?@$ 0  NB. 100 rows, each row close together  NB. some misses
d =: ((] ,"0 (8) c. -)   ([: 11&c. 8&c.)) c

(i.~ a) -: i.~ {."1 b
(i.!.0~ a) -: i.!.0~ b

p =: ?~ a +&# c  NB. perm of a,c
(b i.&:({."1) p { b,d) -: a i. p { a,c
(b i.!.0 p { b,d) -: a i.!.0 p { a,c

((+//. i.@#) {."1 p { b,d) -: (+//. i.@#) p { a,c
((+/(/.!.0) i.@#) p { b,d) -: (+/(/.!.0) i.@#) p { a,c

p =: ?~ #a
((p{b) i.&:({."1) 100{b) -: (p{a) i. 100 { a
((p{b) i.!.0 (100){b) -: (p{a) i.!.0 (100) { a

NB. circle functions
carg =. _2. _1. _0.5 _0.001 0 0.01 0.5 1 2
0&(o. = (o. 11&c.))"0 carg
1&(o. = (o. 11&c.))"0 carg
2&(o. = (o. 11&c.))"0 carg
3&(o. = (o. 11&c.))"0 carg
4&(o. = (o. 11&c.))"0 carg
5&(o. = (o. 11&c.))"0 carg
6&(o. = (o. 11&c.))"0 carg
7&(o. = (o. 11&c.))"0 carg
8&(o. = (o. 11&c.))"0 carg
9&(o. = (o. 11&c.))"0 carg
10&(o. = (o. 11&c.))"0 carg
11&(o. = (o. 11&c.))"0 carg
12&(o. = (o. 11&c.))"0 carg
_1&(o. = (o. 11&c.))"0 carg
_2&(o. = (o. 11&c.))"0 carg
_3&(o. = (o. 11&c.))"0 carg
_4&(o. = (o. 11&c.))"0 carg
_5&(o. =!.1e_11 (o. 11&c.))"0 carg
_6&(o. =!.1e_11 (o. 11&c.))"0 carg
_7&(o. = (o. 11&c.))"0 carg
_8&(o. = (o. 11&c.))"0 carg
_9&(o. = (o. 11&c.))"0 carg
_10&(o. = (o. 11&c.))"0 carg
_11&(o. = (o. 11&c.))"0 carg
_12&(o. = (o. 11&c.))"0 carg

1e_27 > | (- ^@^.)"0 ] 11 c. 1 + (%~ i:) 100
1e_27 > >./ | (- ^@^.)"0 ] 1 +&(11&c.) 1e_15 * 0.5 + 1000 ?@$ 0
a =: 1 +&(11&c.) 1e_20
1e_25 > (*~ - ^&2) a
1e_25 > ((* *~) - ^&3) a

1e_25 > >./ | 1 - (1&o. +&:*: 2&o.) +/ 11 c. 1. 1e_15 * _0.5 + 2 100 ?@$ 0.  NB. precision test

'3.141592653589793238462643383279' ([ -: #@[ {. ]) 0j30 ": o. 11 c. 1

'1.000000000000000000000000000000' -: 0j30 ": (*%) 11 c. 665142606648569600281099799288x

NB. qx =: 11 c. 665142606648569600281099799288x
NB. qy =: 11 c. 665142606648569600281099799280x
NB. t =: 9!:12
NB. 9!:11 ] 30
NB. qy%qx
NB. 9!:11 t

'domain error' -: 12 c. etx 1
'domain error' -: 5 c. etx 1
'domain error' -: 17 c. etx 1
'rank error' -: 4 8 c. etx 1
'domain error' -: 4.5 c. etx 1
'rank error' -: (,4) c. etx 1

NB.? end of skip



NB. int2 and int4
'fixed-precision overflow' -: 10000 + etx 6 c. 30000
'fixed-precision overflow' -: +~ etx 6 c. 30000
'fixed-precision overflow' -: _10000 - etx 6 c. 30000
'fixed-precision overflow' -: (6 c. _30000) - etx 6 c. 30000
'fixed-precision overflow' -: (6 c. 4) * etx 6 c. 10000
'fixed-precision overflow' -: 4 * etx 6 c. 10000
'fixed-precision overflow' -: | etx 6 c. _32768

'fixed-precision overflow' -: 1e9 + etx 7 c. 2e9
'fixed-precision overflow' -: +~ etx 7 c. 2e9
'fixed-precision overflow' -: _1e9 - etx 7 c. 2e9
'fixed-precision overflow' -: (7 c. _2e9) - etx 7 c. 2e9
'fixed-precision overflow' -: 4 * etx 7 c. 1e9
'fixed-precision overflow' -: (7 c. 4) * etx 7 c. 1e9
'fixed-precision overflow' -: | etx 7 c. _2147483648

100100 = (6 c. 100) + (7 c. 100000)

{{
xx =: y ?@$ 1000
assert. ((1&=@:#@[ +. (-: *. -:&(3!:0))&:(+/)) 6&c.) xx
assert. ((1&=@:#@[ +. (-: *. -:&(3!:0))&:(+/)) 7&c.) xx
assert. ((-: *. (1&>:@:# +. 0=(*/@:$))@] +.  -:&(3!:0))&:(+/\) 6&c.) xx
assert. ((-: *. (1&>:@:# +. 0=(*/@:$))@] +. -:&(3!:0))&:(+/\) 7&c.) xx
assert. ((-: *. (1&>:@:# +. 0=(*/@:$))@] +. -:&(3!:0))&:(+/\.) 6&c.) xx
assert. ((-: *. (1&>:@:# +. 0=(*/@:$))@] +. -:&(3!:0))&:(+/\.) 7&c.) xx
1 }} &> (<"0 i. 50) , <"1 (2 6 ?@$ 30)


4!:55 ;:'a argrand argnear b c carg d f f2 fsmall jdot p t xd yd dx dy xx xy qx qy s xs ys'



epilog''

