NB. p. y ----------------------------------------------------------------

test =: 3 : 0
 c=: y
 x=:    1024 %~ _500+?  40$10000
 z=: j./1024 %~ _500+?2 40$10000
 r=: p. c
 d=: p. r
 assert. *./ 1e_6  > c (+/@:|@[ %~ |@]) c p. >{:r
 assert. *./ 1e_13 > c %&(+/@:|)~ -/c,:d
 assert. (c p. z) -:!.1e_12 r p. z
 assert. (c p. z) -:!.1e_12 d p. z
 assert. (c p. x) -:!.1e_12 r p. x
 assert. (c p. x) -:!.1e_12 d p. x
 assert. (p. c) -: p. c,0 0 0 0 0
 1
)

test _10+?11$25
test _10+?10$25
test _10+? 9$25

test j./_10+?2 11$25
test j./_10+?2 10$25
test j./_10+?2  9$25

4!:55 ;:'c d r x z' 

(1;n$_1)   -: p. n!~i.>:n=:>:?15
(n!~i.>:n) -: p. <n$_1
((|.(1+n)$1 _1)*n!~i.>:n) -: p. <n$1

1e_14 > | (p.&< p. ]) r=:1 0 1 
1e_14 > | (p.&< p. ]) r=:1 0 1 1
1e_14 > | (p.&< p. ]) r=:2 3 4
1e_14 > | (p.&< p. ]) r=:2 3 4 5
1e_14 > | (p.&< p. ]) r=:2 3 4.5 
1e_14 > | (p.&< p. ]) r=:2 3 4.5 6
1e_14 > | (p.&< p. ]) r=:1j2 3j4 5
1e_14 > | (p.&< p. ]) r=:1j2 3j4 5 6
1e_14 > | (p.&< p. ]) r=:5j7 5j_7 1j2

(p.<r) -: p. ({.1 1  );}.2,  r=:_7+?3$20
(p.<r) -: p. ({.1 1  );}.2.5,r
(p.<r) -: p. ({.1 1  );}.2j5,r
(p.<r) -: p. ({.1 2  );}.2,  r
(p.<r) -: p. ({.1 2  );}.2.5,r
(p.<r) -: p. ({.1 2  );}.2j5,r
(p.<r) -: p. ({.1 2  );}.2,  r
(p.<r) -: p. ({.1 2  );}.2.5,r
(p.<r) -: p. ({.1 2  );}.2j5,r
(p.<r) -: p. ({.1 2.5);}.2,  r
(p.<r) -: p. ({.1 2.5);}.2.5,r
(p.<r) -: p. ({.1 2.5);}.2j5,r
(p.<r) -: p. ({.1 2j5);}.2,  r
(p.<r) -: p. ({.1 2j5);}.2.5,r
(p.<r) -: p. ({.1 2j5);}.2j5,r

(0;'') -: p. ''
(0;'') -: p. i.0
(0;'') -: p. 0$<''
(0;'') -: p. 0
(0;'') -: p. ,0
(0;'') -: p. 0 0 0 0 0

(1;,r) -: p. 1,~-r=:o._400+?1000
(1;,r) -: p. 1,~-r=:r._400+?1000

f =: 3 : ('''a b c''=.|.y'; '(+:a)%~-b(+,-)%:(*:b)-4*a*c')
(f -:&(/:~) >@{:@p.) c=:(0.1*_500+?2$2000),>:?500
(f -:&(/:~) >@{:@p.) c=:r.(0.1*_500+?2$2000),>:?500

(-: p.^:2) 0j1 0j1
(-: p.^:2) 3j4;i._4

(,1) -: p. <''
(,1) -: p. <i.0
(,1) -: p. a:

'domain error' -: p. etx 'abcd'
'domain error' -: p. etx <'abcd'
'domain error' -: p. etx <<3 4 5
'domain error' -: p. etx 4;'abcd'
'domain error' -: p. etx 4;<<2 3 4
'domain error' -: p. etx 'a' ;3 4 5 6
'domain error' -: p. etx (<3);3 4 5 6
'domain error' -: p. etx 4
'domain error' -: p. etx 4 0 0 0 0 

'length error' -: p. etx 3;4 5 6;7 8 9 10

'rank error'   -: p. etx 3;i.3 4
'rank error'   -: p. etx 3;i.0 0
'rank error'   -: p. etx (,3);3 4 5 6
'rank error'   -: p. etx '';3 4 5 6
'rank error'   -: p. etx 2 3;3 4 5 6


NB. "difficult" roots ---------------------------------------------------

NB. examples from Nakano, Yamashita, and Nishikawa, Vector 21.2, Spring 2005

c=: ''
c=: c,< _4 6 _3 1
c=: c,< _9 9 _3 1
c=: c,< _2 6 _6 1
c=: c,< _18 18 _6 1
c=: c,< _3 9 _9 1
c=: c,< _12 18 _9 1
c=: c,< _1 6 _12 1
c=: c,< _4 12 _12 1
c=: c,< _9 18 _12 1
c=: c,< _5 15 _15 1
c=: c,< _20 30 _15 1
c=: c,< 12 _30 27 _10 1
c=: c,< 2 _3 _3 6 0 _3 1
c=: c,< 175 210 99 _40 _33 _10 1 0 1
r=: >@{:@p.&.> c
1e_12 > c >./@:|@:p.&> r

c=: 1 50000 1  NB. Andrew Nikitin
r=: >{:p. c
1e_6 > >./ | c p. r

test=: 3 : 0  NB. finding roots exactly
 assert. r1 -:!.0&(/:~) r=: >{:p. c=: p. <r1=: y
 1
)

test 2,10$1
test 1r2,10$1r3
test 1024,10$1r2
test 4 5#1r2 1r3
test 4 5#1r3 1r2
test 1+i.20  NB. Wilkinson monster

ptimes=: +//.@(*/)  NB. polynomial multiplication
c=: (p.<3j4 3j_4) ptimes p. <1r2,8$1r3
r=: >{:p. c
NB. r -:&(/:~) 3j4 3j_4,1r2,8$1r3


NB. p. roots of unity ---------------------------------------------------

rou =: ^@:j.@:o.@:+:@(%~i.)
rov =: >@{:@p.@(_1&,)@({.&1)@-
rs  =: \: +.

1e_12 > (rou (>./)@(<./)@:|@(-/) rov)"0 >:i.10

err =: - %&(+/)&:| ]
f   =: p.@<@rou
g   =: _1&,@({.&1)@-

1e_11 >| (f err g)"0 >:i.10


NB. x p. y --------------------------------------------------------------

pc =: ([ +/@:* i.@#@[ ^~ ])"1 0
pr =: (>@{.@[ * >@{:@[ */@:-~ ])"1 0
fc =: *./@,@:(p. -: pc)
fr =: *./@,@:(p. -: pr)

NB. Boolean

(x=:?5$2) fc y=:?7$2
(x=:?5$2) fc y=:_7+?2 3 4$13
(x=:?2$2) fc y=:o._7+?3 4$13
(x=:?6$2) fc y=:r._7+?2 1 4$13

NB. integer

(x=:_5+?5$11) fc y=:?7$2
(x=:_5+?5$11) fc y=:_7+?2 3 4$13
(x=:_5+?2$11) fc y=:o._7+?3 4$13
(x=:_5+?6$11) fc y=:r._7+?2 1 4$13

NB. real

(x=:_2.5+?5$11) fc y=:?7$2
(x=:_2.5+?5$11) fc y=:_7+?2 3 4$13
(x=:_2.5+?2$11) fc y=:o._7+?3 4$13
(x=:o._4+?6$11) fc y=:r._7+?2 1 4$13

NB. complex

(x=:j./_5+?2 5$11) fc y=:?7$2
(x=:r.?5$110)      fc y=:_7+?2 3 4$13
(x=:r.?2$110)      fc y=:o._7+?3 4$13
(x=:r._4+?6$11)    fc y=:r._7+?2 1 4$13

'' fc y=:?3 4$2
'' fc y=:?3 4 1$100
'' fc y=:o.?3 4$100
'' fc y=:r.?3 4$100

($0) fc y=:?3 4$2
($0) fc y=:?3 4 1$100
($0) fc y=:o.?3 4$100
($0) fc y=:r.?3 4$100

(( <1 2 3) p. x) -: (1;1 2 3) p. x=:?2 3 4$100
((,<1 2 3) p. x) -: (1;1 2 3) p. x=:?2 3 4$100

((2  ;1r7) p. x) -: 2   * x-1r7 [ x=: ?2 3 4$100
((2.5;1r7) p. x) -: 2.5 * x-1r7 [ x=: ?2 3 4$100
((2j5;1r7) p. x) -: 2j5 * x-1r7 [ x=: ?2 3 4$100

((2  ;1r7) p. x) -: 2   * x-1r7 [ x=: o.?2 3 4$100
((2.5;1r7) p. x) -: 2.5 * x-1r7 [ x=: o.?2 3 4$100
((2j5;1r7) p. x) -: 2j5 * x-1r7 [ x=: o.?2 3 4$100

((2  ;1r7) p. x) -: 2   * x-1r7 [ x=: j./?2 3 4$100
((2.5;1r7) p. x) -: 2.5 * x-1r7 [ x=: j./?2 3 4$100
((2j5;1r7) p. x) -: 2j5 * x-1r7 [ x=: j./?2 3 4$100

(i.2 3) fc"1 i.5

x=: 2 5 ?@$ 10
y=: 2   ?@$ 12
x (p."2 1 -: p."1 0"2 1) y

'domain error' -: 'abc'       p. etx 3    
'domain error' -: 4 5 6       p. etx 'ab' 
'domain error' -: 1 2 3       p. etx <4 5 6 
'domain error' -: 1 2 3       p. etx <3;4 5 6 
'domain error' -: (3;4 5)     p. etx 'ab' 
'domain error' -: ('a';_3 7)  p. etx 4 5  
'domain error' -: (3;'abc')   p. etx 4 5  
'domain error' -: (3;<<1 2 3) p. etx 4 5  
'domain error' -: ((<3);2 3)  p. etx 4 5  

'length error' -: (i.2 4)     p. etx 3 4 5   
'length error' -: (3;4 5;6 7) p. etx 1 2 3 4 

'rank error'   -: (3 4;1 2 3) p. etx 4 5     
'rank error'   -: (3;i.2 3)   p. etx 4 5     


NB. x p.!.s y -----------------------------------------------------------

f =: 4 : '+/x*y^!.s i.#x'
g =: 4 : '+/x*}:1,*/\y+(i.#x)*/s'

x=:-:_10+?7$20
y=:0.25*_15+?4$30

(x p.!.s y) -: x f"1 0 y [ s=:_1
(x p.!.s y) -: x g"1 0 y
(x p.!.s y) -: x f"1 0 y [ s=:3j4
(x p.!.s y) -: x g"1 0 y

'domain error' -: ex 'p.!.(<234)'
'domain error' -: ex 'p.!.''x'''

'domain error' -: 'abc' p.!.1 etx 5
'domain error' -: 2 3 4 p.!.1 etx 'x'
'domain error' -: (<2 3)p.!.1 etx 5

'length error' -: (i.2 3) p.!.1 etx 4 5 6


NB. p. model ------------------------------------------------------------

NB. Reference:  Press et al., "Numerical Recipes in C"

mt   =: i.@0:                           NB. empty vector constant function 
boxed=: (32&=)@(3!:0)                   NB. 1 iff boxed
div  =: % (+ 0&=)                       NB. like % but avoids division by 0

n    =: <:@#@[                          NB. degree
d1   =: }.@(* i.@#)@[ pdot ]            NB. 1st derivative
d2   =: 2&}.@(* +:@(2&!)@i.@#)@[ pdot ] NB. 2nd derivative
g    =: d1 % pdot
h    =: *:@g - d2 % pdot
sq   =: [: %: (n-1:) * (n * h) - *:@g
emm  =: {.@\:@:| { ]                    NB. element with maximum magnitude
del  =: n div [: emm g (+,-) sq         NB. corrective term
delta=: del`0:@.(0&=@pdot)              NB. corrective term, or 0 if at root
lg   =: ] - delta                       NB. one iteration of Laguerre's method

root =: lg^:_ >@{. lg^:_ 0:             NB. one root using Laguerre's method
rem  =: _2&}.@[ , _2&{@[ + _1&{@[ * ]   NB. remainder from one trial division
dfl  =: mt`((rem$:]),{:@[)@.(1&<@#@[)   NB. deflate by linear factor
step =: root ((dfl~>@{.) ; (,>@{:)) ]   NB. update one root
rfc  =: [ step^:n [;mt                  NB. roots from coefficients

pm   =: [: +//. */                      NB. polynomial multiplication
cfr  =: {.@>@{. * [: pm/ -@>@{: ,. 1:   NB. coefficients from roots

pdot1=: rfc`cfr @. boxed                NB. model of p. monad

evc  =: [ +/ .* ] ^ i.@#@[              NB. evaluate w.r.t. coefficients
evr  =: [: */ >@{.@[ , ] - >@{:@[       NB. evaluate w.r.t. roots
pdot2=: evc`evr @. (boxed@[)            NB. model of p. dyad

pdot =: pdot1 : pdot2 " 1 1 0           NB. model of p.

x=: (_5+?4$12),1
r=: _5+?5$12
c=: {.(_5+?5$12)-.0
y=:    0.01*_100+?  20$200
z=: j./0.01*_100+?2 20$200

NB. a=: p.   x
NB. b=: pdot x
NB. (>{.a) = >{.b
NB. a=:>{:a
NB. b=:>{:b
NB. (($a)-:$b) *. (*./a e. b) *. *./b e. a

NB. (p. -: pdot) c;r
NB. x (p. -: pdot) y
NB. x (p. -: pdot) z
NB. (c;r) (p. -: pdot) y
NB. (c;r) (p. -: pdot) z


4!:55 ;:'a b boxed c cfr d d1 d2 del delta dfl '
4!:55 ;:'div emm err evc evr f fc fr g h '
4!:55 ;:'lg mt n p pc pdot pdot1 pdot2 pm pr ptimes r r1'
4!:55 ;:'rem rfc root rou rov rs s sq step test x y z '


