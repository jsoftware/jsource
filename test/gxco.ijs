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

(= x:) ,1 _1 */ 1e43 1e_43
(= x:) ,1 _1 */ 2^137 _137

(": -: ":@(+&(-~1r2)))@x:"0 ] ,1 _1 */ 1e43 1e_43
(": -: ":@(+&(-~1r2)))@x:"0 ] ,1 _1 */ 2^137 _137

'domain error'      -: 0.5 = etx   10^309x
'domain error'      -: 0.5 = etx - 10^309x

'ill-formed number' -: ex '1x ___'


NB. x: conversion from rationals to floats ------------------------------

f=: 3 : 0
 p=: (_1^10000 ?@$ 2) * 10000 ?@$ y
 q=:                1 + 10000 ?@$ y
 d=: (p%q) - p %&x: q
 assert. 0=d
 1
)

f"0 ]2*10^2 3 9

f1=: 3 : 0
 p=: (_1^1000 ?@$ 2) * 1000 ?@$ y
 q=: 2+($p) ?@$ 20
 e=: (_1^($p) ?@$ 2)*q^x:<._350*q^.10
 d=: p - _1 x: p + e
 assert. 0 = d
 1
)

NB. f1"0 ]10^2 3 9
f1"0 ]10^2 3


4!:55 ;:'d f f1 p q'


