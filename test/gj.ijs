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
'?' 0!:_1@[^:(-. +./ ('avx2';'avx512') +./@:E.&> <9!:14'') 1  NB. skip if not AVX2

NB. tests requiring conforming arguments, qp vs xnum
argrand =: {{  NB. x is max value (extended).  y is (max sizes),:(cumulative freq for probability of each rank)  args are large random integers
xs =: (({: y) I. ?0) {. s [ys =: (({: y) I. ?0) {. s [ s =: ? {.y  NB. conforming shapes
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
assert. (0&<: *. qx&>:) qy - qx * (<.!.0 qy%qx)
assert. (0&<: *. qx&>:) qy -~ qx * (>.!.0 qy%qx)
assert. (+:qy) -:!.0 +: xy
assert. (-:qy) -:!.0 -: xy
assert. 2 > | (*:%:qy) - xy
assert. (<:qy) -:!.0 <: xy
assert. (<:qy) -:!.0 <: xy
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

NB. ($ #: I.@,)@:~:
'1.000000000000000000000000000000' -: 0j30 ": (*%) 11 c. 665142606648569600281099799288x

NB. qx =: 11 c. 665142606648569600281099799288x
NB. qy =: 11 c. 665142606648569600281099799280x
NB. 9!:11 ] 30
NB. qy%qx

NB.? end of skip


4!:55 ;:'a argrand argnear b f f2 fsmall jdot t xd yd dx dy xx xy qx qy s xs ys'



epilog''

