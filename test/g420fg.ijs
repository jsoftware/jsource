prolog './g420fg.ijs'
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
x=: 53 700 ?@$ 1e3
y=: 53 700 ?@$ 1e3
p=: 13#x
q=: 13#y
300 > | -/ space 'p +/@:*     q',:'x +/@:*     y'  NB. special code doesn't depend on # items
300 > | -/ space 'p ([: +/ *) q',:'x ([: +/ *) y'
(_3500+*/ >.&.(2&^.)(IF64{4 8),$y) > | -/ space 'p +/@:*     q',:'x +/@:*     >: y'  NB. >: y makes a copy, but then no further copy and thus uses LESS space than the special code

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

NB. x u"r"r y --------------------------------------------------------------------

NB. y is lir, rir, lvr, rvr, lnr, rnr   ranks of verbs, ranks of arguments
f =: 3 : 0"1
y0 =: y
'lir rir lvr rvr lnr rnr' =. y
NB. effective rank is no more than the arg rank
'ler rer' =. (lvr,rvr) <. (lnr,rnr)
NB. Create inner frame
if =. >: 3 ?@$~ 0 >. (ler-lir) <. (rer-rir)
NB. Create outer frame
of =. >: 3 ?@$~ (lnr-ler) <. (rnr-rer)
NB. Create left shape: outer frame (extended as needed) , inner frame (extended as needed) , length of vector.  Empty if noun rank is 0
ls =. lnr {. (of , >: ((lnr-ler)-$of) ?@$ 4) , (if , >: (0 >. (ler-lir)-$if) ?@$ 4) , >: lir ?@$ 6
rs =. rnr {. (of , >: ((rnr-rer)-$of) ?@$ 4) , (if , >: (0 >. (rer-rir)-$if) ?@$ 4) , >: rir ?@$ 6
NB. Create arguments
xx =: ls ?@$ 0
yy =: rs ?@$ 0
tvb =. +&(+/@,)
tvb1 =. tvb"(lir,rir)
assert. (xx tvb1"(lvr,rvr) yy) -: (xx tvb"(lir,rir)"(lvr,rvr) yy)
1
)

f (#: i.@:(*/)) 6$5

NB. x +/@:*"1 y ---------------------------------------------------------

NB. y is lvr, rvr, lnr, rnr   ranks of verbs, ranks of arguments
f =: 3 : 0"1
y0 =: y
dot =. +/@:*
'lvr rvr lnr rnr' =. y
NB. effective rank is no more than the arg rank
'ler rer' =. (lvr,rvr) <. (lnr,rnr)
NB. Create inner frame
if =. >: 3 ?@$~ 0 >. 1 -~ ler <. rer
NB. Create outer frame
of =. >: 3 ?@$~ (lnr-ler) <. (rnr-rer)
NB. Create length of vector
vl =. >: ? 6
NB. Create left shape: outer frame (extended as needed) , inner frame (extended as needed) , length of vector.  Empty if noun rank is 0
ls =. lnr {. (of , >: ((lnr-ler)-$of) ?@$ 4) , (if , >: (0 >. (ler-1)-$if) ?@$ 4) , vl
rs =. rnr {. (of , >: ((rnr-rer)-$of) ?@$ 4) , (if , >: (0 >. (rer-1)-$if) ?@$ 4) , vl
NB. Create arguments
xx =: ls ?@$ 0
yy =: rs ?@$ 0
assert. (xx +/@:*"1"(lvr,rvr) yy) -: (xx dot"1"(lvr,rvr) yy)
1
)

f (#: i.@:(*/)) 4$5

NB. same but allow 0 in shape
f =: 3 : 0"1
y0 =: y
dot =. +/@:*
'lvr rvr lnr rnr' =. y
NB. effective rank is no more than the arg rank
'ler rer' =. (lvr,rvr) <. (lnr,rnr)
NB. Create inner frame
if =. 3 ?@$~ 0 >. 1 -~ ler <. rer
NB. Create outer frame
of =. 3 ?@$~ (lnr-ler) <. (rnr-rer)
NB. Create length of vector
vl =. ? 6
NB. Create left shape: outer frame (extended as needed) , inner frame (extended as needed) , length of vector.  Empty if noun rank is 0
ls =. lnr {. (of , ((lnr-ler)-$of) ?@$ 4) , (if , (0 >. (ler-1)-$if) ?@$ 4) , vl
rs =. rnr {. (of , ((rnr-rer)-$of) ?@$ 4) , (if , (0 >. (rer-1)-$if) ?@$ 4) , vl
NB. Create arguments
xx =: ls ?@$ 0
yy =: rs ?@$ 0
assert. (xx +/@:*"1"(lvr,rvr) yy) -: (xx dot"1"(lvr,rvr) yy)
1
)

f (#: i.@:(*/)) 4$5

NB. Test argument types
(20 4 ?@$ 100) (+/@:*"1 -: +/@:*"]"1) 20 4 ?@$ 100
(x: 20 4 ?@$ 100) (+/@:*"1 -: +/@:*"]"1) x: 20 4 ?@$ 100
(1r5 * x: 20 4 ?@$ 100) (+/@:*"1 -: +/@:*"]"1) 1r7 * x: 20 4 ?@$ 100
(3j0.5 * 20 4 ?@$ 100) (+/@:*"1 -: +/@:*"]"1) 4j2 * 20 4 ?@$ 100
(+/@:*"1 -: +/@:*"]"1)&>/"1 ?@$&2&.> 2&#"0 (i. 32) , (254*8) + i. 100
'domain error' -: +/@:*"1~ etx 'abc'
'domain error' -: +/@:*"1~ etx u: 'abc'

NB. Test varying sizes to catch all boundaries
(?@$&0 (+/@:*"1 -: +/@:*"]"1) ?@$&0)"1 ((4 + $ ?@$ 4:) ,. ]) i. 99
((((4 + $ ?@$ 4:) ,. ]) ?@$ 0:) (+/@:*"1/ -: +/@:*"]"1/) (((4 + $ ?@$ 4:) ,. ]) ?@$ 0:))"0  i. 99

NB. Verify double-speed on AVX2 when a repeated

NB. Test for NaN
0 _ 1 (+/@:*"1 -: +/@:*"]"1) _ 0 2   NB. removable NaN
'NaN error' -: _ 0 __ +/@:*"1 etx  1 1 1

NB. x +/@:*"1!.0 y ---------------------------------------------------------
f =: 3 : 0"1   NB. x is rank to use for left arg; y is len of product;rank;left frame;right frame
'len rnk lf rf' =. y
l =. <. -: len
maxin =. <. %: 1e15 % len  NB. max product to avoid loss of precision in sum
big1 =. ((lf,l) ?@$ maxin) + (lf,l) ?@$ 0
big2 =. ((rf,l) ?@$ maxin) + (rf,l) ?@$ 0
op1 =. big1 ,"1 (-big1) ,"1 (1e_5)  NB. the dot-product will cancel out, leaving 1e_10
op2 =. big2 ,"1 big2 ,"1 (1e_5)
if. 2 | len do. op1 =. op1 ,"1 (0) [ op2 =. op2 ,"1 (0) end.
perm =. ?~ >: len
NB. op1__ =: op1 [ op2__ =: op2 [ rnk__ =: rnk [ perm__ =: perm
op1 +/@:*"1!.0"rnk&:(perm&{"1) op2
)
1e_15 > | 1e_10 - f 1e5;1;'';''
1e_15 > | 1e_10 - f (, 1 1e5+/i. 17) ;"0 1 ]1;'';''
1e_15 > | 1e_10 - f (, 1 1e5+/i. 17) ;"0 1 ]1;'';2
1e_15 > | 1e_10 - f (, 1 1e5+/i. 17) ;"0 1 ]1;2;''
1e_15 > | 1e_10 - f (, 1 1e5+/i. 17) ;"0 1 ]2;2 2 2;''

NB. Test for NaN
'NaN error' -: 0 _ 1 (+/@:*"1!.0) etx _ 0 2   NB. in exact calculation, any infinity causes a NaN
'NaN error' -: _ 0 __ +/@:*"1 etx  1 1 1

4!:55 ;:'f p q space test testsub x xx y yy x0 y0'



epilog''

