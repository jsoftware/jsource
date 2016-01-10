NB. f/@:g for atomic verbs ----------------------------------------------

f=: ;: '! % * + - < = > ^ | <. <: >. >: +. +: *. *: ~: o.'

testsub=: 2 : 0
 xx=: y{~101 23 ?@$ #y
 yy=: y{~101 23 ?@$ #y
 assert. (u/ xx v yy) -: xx u/@:v yy
 assert. (u/ xx v&(0&{.) yy) -: xx u/@:v&(0&{.) yy
 assert. (u/ xx v&(1&{.) yy) -: xx u/@:v&(1&{.) yy
 assert. (u/ xx v&(2&{.) yy) -: xx u/@:v&(2&{.) yy
 x0=: (?#y){y
 y0=: (?#y){y
 assert. (u/ x0 v yy) -: x0 u/@:v yy
 assert. (u/ xx v y0) -: xx u/@:v y0
 1
)

test=: 2 : 0
 if. 'b' e. y do. u testsub v 0 1                          end.
 if. 'c' e. y do. u testsub v a.                           end.
 if. 'i' e. y do. u testsub v        _1000+  1000 ?@$ 2000 end.
 if. 'd' e. y do. u testsub v 4096%~*_1000+  1000 ?@$ 2000 end.
 if. 'z' e. y do. u testsub v j./    _1000+2 1000 ?@$ 2000 end.
)

+  test *  'bidz'
+  test *. 'bidz'
+  test +. 'bidz'
+  test <  'bid'
+  test <: 'bid'
+  test =  'bidz'
+  test ~: 'bidz'
+  test >  'bid'
+  test >: 'bid'

>. test *  'bid'
>. test +  'bid'

*. test +. 'b'
+. test *. 'b'
=  test *. 'b'
~: test *. 'b'

space=: 7!:2
x=: 53 7 ?@$ 1e6
y=: 53 7 ?@$ 1e6
p=: 13#x
q=: 13#y
300 > | -/ space 'p +/@:*     q',:'x +/@:*     y'
300 > | -/ space 'p ([: +/ *) q',:'x ([: +/ *) y'

space=: 7!:2
x=: 53 7 ?@$ 0
y=: 53 7 ?@$ 0
p=: 13#x
q=: 13#y
300 > | -/ space 'p +/@:*     q',:'x +/@:*     y'
300 > | -/ space 'p ([: +/ *) q',:'x ([: +/ *) y'

246  -: 2  +/@:* 123
_111 -: 12 */@:- 123


NB. integer overflow handling -------------------------------------------

x=: 23 7 ?@$ 1e4
y=: 23 7 ?@$ 1e5
(+/x*y) -: x +/@:* y

x=: 31 7 ?@$ >.imax%16
y=: 31 7 ?@$ >.imax%16
(+/x+y) -: x +/@:+ y

x=: (31$1 _1) * 31 7 ?@$ >.imax%16
y=: (31$1 _1) * 31 7 ?@$ >.imax%16
(-/x+y) -: x -/@:+ y

x=: 31 7 ?@$ 1e4
y=: 31 7 ?@$ 1e4
(+/x+ y) -: x +/@:+  y
(+/x>.y) -: x +/@:>. y

x=: 30 7 ?@$ 1e4
y=: 30 7 ?@$ 1e4
(+/x+ y) -: x +/@:+  y
(+/x>.y) -: x +/@:>. y

x=: 31 7 ?@$ >.imax%8
y=: 31 7 ?@$ >.imax%8
(+/x+ y) -: x +/@:+  y
(+/x>.y) -: x +/@:>. y

x=: 30 7 ?@$ >.imax%8
y=: 30 7 ?@$ >.imax%8
(+/x+ y) -: x +/@:+  y
(+/x>.y) -: x +/@:>. y

(4=3!:0 x) *. (imax-1) = x=: (2-1 1) +/@:* 1, imax-2
(4=3!:0 x) *. (imax-2) = x=: _1 2    +/@:* 1, x:^:_1 <.imax%2x


4!:55 ;:'f p q space test testsub x xx y yy'


