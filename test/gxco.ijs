prolog './gxco.ijs'
NB. x: ------------------------------------------------------------------

(x: 4) -: x: {: 3.4 4
(x: 4) -: x: {: 3j4 4

7r2     -: x: 3.5
3r5     -: x: 3r5
7r100   -: x: 0.07
271r100 -: x: 2.71

64 -: type x: 0
64 -: type x: 1
64 -: type x: 0 1
64 -: type x: _5 999999
64 -: type x: 2147483647 _2147483648 0 9

128 -: type x: -~2.5
128 -: type x: o. 0
128 -: type x: 1 2 3 4 5 6.5

128 -: type x: _
128 -: type x: __

(2^ 53x) = x: 2^ 53
(2^_53x) = x: 2^_53
(=<.)   x:   !20+i.30
(=<.) % x: % !20+i.30

'domain error' -: x: etx 3j5
'domain error' -: x: etx '345'
'domain error' -: x: etx <1 2 3

NB. 'domain error' -: x: etx _.

'domain error' -: 'a' x: etx 1.5
'domain error' -: 3.4 x: etx 1.5
'domain error' -: 3j4 x: etx 1.5
'domain error' -: 3   x: etx 1.5
'domain error' -: _3  x: etx 1.5
'domain error' -: 0   x: etx 1.5

(0 1 0 1 ,. 1x) -: 2 x: 0 1 0 1
(1 2 3 4 ,. 1x) -: 2 x: 1 2 3 4
(3 5 7 9 ,. 2x) -: 2 x: 1 2 3 4+0.5
(1 2 3 4 ,. 1x) -: 2 x: 1 2 3 4+-~0j1

((,%)    ! 10*i.10) -: x:^:_1   (,%)    ! 10*i.10x
(12345 % ! 10*i.10) -: x:^:_1 ] 12345 % ! 10*i.10x

(+/&.(x:!.0)"1 -:!.0 ([:_1&x: [:+/"1 x:!.0)) (QKTEST{::20 20;10 10)?@$0
(+/&.(x:!.0)   -:!.0 ([:_1&x: [:+/   x:!.0)) (QKTEST{1000 100)?@$0     NB.todo bump this to a more reasonable value--in particular, one that can properly stress the large superaccumulator--once extended-precision operations are fast


(= x:) ,1 _1 */ 1e43 1e_43
(= x:) ,1 _1 */ 2^137 _137

(": -: ":@(+&(-~1r2)))@x:"0 ] ,1 _1 */ 1e43 1e_43
(": -: ":@(+&(-~1r2)))@x:"0 ] ,1 _1 */ 2^137 _137

_ = 10^309x
0 = 1.2 % 10^309
'ill-formed number' -: ex '1x ___'

3333333333333333 3333333333333334 3333333333333335 3333333333333335 3333333333333337 3333333333333338 3333333333333339 3333333333333340 3333333333333340 3333333333333342 -: <. 10000000000000000 * x:!.0 ] (1%3) + 1e_16 * i. 10
(1%3) = 0. + x:!.0 (1%3)
(1e100) = 0. + x:!.0 (1e100)
(1e_100) = 0. + x:!.0 (1e_100)
(_1%3) = 0. + x:!.0 (_1%3)
(_1e100) = 0. + x:!.0 (_1e100)
(_1e_100) = 0. + x:!.0 (_1e_100)


NB. x: conversion from rationals to floats ------------------------------

f=: 4 : 0
 p=: (_1^x ?@$ 2) * x ?@$ y
 q=:                1 + x ?@$ y
 d=: (p%q) - p %&x: q
 assert. 0=d
 1
)

(((QKTEST{10000 100),4) {~ 9!:57 (0) [ 9!:57 (1)) f"0 ]2*10^(QKTEST{::2 3 9;2 3 3)

f1=: 4 : 0
 p=: (_1^x ?@$ 2) * x ?@$ y
 q=: 2+($p) ?@$ 20
 e=: (_1^($p) ?@$ 2)*q^x:<._350*q^.10
 d=: p - _1 x: p + e
 assert. 0 = d
 1
)

5 = 0.0+*:_1+2*(+%)/1e3#1x

NB. e =: 1 + 1000 ?@$ 10 ^ (QKTEST{100x 10x) [ d =: 1 + 1000 ?@$ 10 ^ (QKTEST{100x 10x) NB. test conversion to float
e =: 1 + (QKTEST{1000 10) ?@$ 10 ^ (QKTEST{100x 10x) [ d =: 1 + (QKTEST{1000 10) ?@$ 10 ^ (QKTEST{100x 10x) NB. test conversion to float
3.2e_16 > >./ x:^:_1 | 1x - (%   [: x:!.0 x:^:_1) e%d

NB. f1"0 ]10^2 3 9
NB. This test seems to be valid but the prior k.c would
NB. silently truncate small rationals to zero in the
NB. part of jtDfromQ which this test exercises.
NB.
NB. FIXME: make this work right
NB. (1000 4 {~ 9!:57 (0) [ 9!:57 (1)) f1"0 ]10^2 3


4!:55 ;:'e d f f1 p q'



epilog''

