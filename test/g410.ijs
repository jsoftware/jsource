prolog './g410.ijs'
NB. !y ------------------------------------------------------------------

fac=: 1:`(* $:@<:) @. * " 0

(! -: fac) i.10

1 1      -: ! 0 1
1 1 2    -: ! 0 1 2
_ _      -: ! 300 301
_ _      -: !    1e90 1e200
_ _      -: ! }: 1e90 1e200 0j1
_ __     -: ! _1 _2
__ 0 _ 0 -: ! _300+0.5*i.4
1 [ ! _. NB. succeeds if J doesn't hang or crash

(!3.5)   -: 3.5  *  !2.5
(!3.5)   -: 4.5  %~ !4.5
(!12.2)  -: 12.2 *  !11.2
(!12.2)  -: 13.2 %~ !13.2

(!x)     -: (!x-k) * */"1 x    -/i.k=: ?10 [ x=:    10+10*  20 ?@$ 0
(!x)     -: (!x-k) * */"1 x    -/i.k
(!x)     -: (!x+k) % */"1 (x+1)+/i.k=: ?10 [ x=: j./10+10*2 20 ?@$ 0
(!x)     -: (!x+k) % */"1 (x+1)+/i.k

1e_10 > | 0.8862269254 - !  0.5
1e_10 > | 1.2254167024 - ! _0.25
1e_10 > | 3.6256099082 - ! _0.75

2.678938534707747 = ! _2r3  NB. Knuth, Volume 1, Appendix B
1.772453850905516 = ! _1r2
1.354117939426400 = ! _1r3
1p0.5             = ! _1r2
0.5p0.5           = !  1r2

(!x) -: !x j.  1e_20 [ x=: 0.01 * 20 ?@$ 2000
(!x) -: !x j. _1e_20

'domain error' -: ! etx 'abc'
'domain error' -: ! etx <'abc'

NB. following limit error can be _ on some platforms (linux j32)
+./('limit error';_) = < ! etx   - 2    +2^53
+./('limit error';_) = < ! etx {.- 2 0j1+2^53

NB. Gamma Function ------------------------------------------------------

NB. Formulae from Abramowitz & Stegun

re =: {.@+.   NB. real part
im =: {:@+.   NB. imaginary part

c=:   0 1                 0.5772156649015329 _0.6558780715202538 
c=:c, _0.0420026350340952 0.1665386113822915 _0.0421977345555443
c=:c, _0.009621971527877  0.007218943246663  _0.0011651675918591 
c=:c, _0.0002152416741149 0.0001280502823882 _0.0000201348547807
c=:c, _0.0000012504934821 0.000001133027232  _0.0000002056338417
c=:c, 6.116095e_9         5.0020075e_9       _1.1812746e_9
c=:c, 1.043427e_10        7.7823e_12         _3.6968e_12
c=:c, 5.1e_13             _2.06e_14          _5.4e_15
c=:c, 1.4e_15             1e_16

NB. 6.1.34 Power Series
gps      =: % @ (c&p.)                              

NB. 6.1.15  Recurrence Formula
em       =: (<.@| + 0.5&<@(1&|)@| - 0&< *. (=<.))@re
grp      =: (gps@- * <:@[ ^!._1 ]) em
grm      =: (gps@+ %      ^!. 1  ) em
grecur   =: grm`grp@.(0:<re)

NB. 6.1.20  Gauss Multiplication Formula
Gauss    =: 2p1&^@-:@-.@[ * (^ -&0.5) * */@:(grecur"0)@:([ %~ i.@[ + ])

NB. 6.1.37  Stirling's Approximation
sbase    =: %:@(2p1&%) * %&1x1 ^ ]
scorr    =: 1 1r12 1r288 _139r51840 _571r2488320&p.@%
Stirling =: sbase * scorr

en       =: 1&>.@>.@(%&(%:3r4))@|@im
gamma    =: (en Gauss ]) ` Stirling @. (20&<@|@im) " 0

g        =: !@<:

(24$'_ __ ') -: ": g -i.10

(g@>: = [ * g) x=: _0.1 _0.9 0.1 0.9 _4.1 _4.9 4.1 4.9 
(g@>: = [ * g) z=: _0.1 _0.9 0.1 0.9 j./_0.4 _0.5 0.4 0.5 _4 _5 4 5
(g@>: = [ * g) z=: _4.1 _4.9 4.1 4.9 j./_0.4 _0.5 0.4 0.5 _4 _5 4 5

(g@+ = +@g) z=:j./1e_2*_1e3+?2 20$2e3  NB. 6.1.23
(g@+ = +@g) z=:j./3e_2*_1e3+?2 20$2e3

(g = gamma) >:i.4 5

(g = gamma) x=: _0.1 _0.9 0.1 0.9 j./_0.4 _0.5 0.4 0.5 _4 _5 4 5
(g = gamma) x=: _4.1 _4.9 4.1 4.9 j./_0.4 _0.5 0.4 0.5 _4 _5 4 5

(g =!.1e_12 gamma) 1.5
(g =!.1e_12 gamma) 1r3 1r2 2r3
(g =!.1e_12 gamma) x=:   1e_2*_7e2+?2 20$2e3
(g =!.1e_12 gamma) x=:   3e_2*_7e2+?2 20$2e3
(g =!.1e_12 gamma) z=:j./1e_2*_7e2+?2 20$2e3

sinh =: 5&o.
rm   =: *:@|@g@j. % 1p1&%@(* sinh@o.)                   NB. 6.1.29
1e_12 > | 1 - rm y=:(+ 0&=)0.001*_30000+?4 5$60000  NB. failed on 0

1e_10 > | 0.9513507699 - g 1.1                          NB. Table 6.1
1e_10 > | 0.8862269255 - g 1.5
1e_10 > | 0.9617658319 - g 1.9

1e_12 > | _0.190945499187j_0.244058298905 - ^. g 1j0.5  NB. Table 6.7
1e_12 > | _0.650923199302j_0.301640320468 - ^. g 1j1
1e_12 > | _1.876078786431j0.129646316310  - ^. g 1j2

x -: g^:_1 g x=:34
x -: g^:_1 g x=:3.4

NB. y -: g g^:_1 y=:1.5  NB. fails, 0 derivative
y -: g g^:_1 y=:3j4

NB. EEM, 1999-06-02
NB. coefficients in ascending order
NB. from Hart et al. Computer Approximations, 6.6
NB. for gamma (2+x), Index 5243, p. 248

p=:_42353.689509744089
p=:p,_20886.861789269888
p=:p,_8762.710297852149
p=:p,_2008.52740130727915
p=:p,_439.3304440600257
p=:p,_50.108693752970953
p=:p,_6.744950724592529
q=:_42353.689509744090
q=:q,_2980.385330925665
q=:q,9940.307415082771
q=:q,_1528.607273779522
q=:q,_499.028526621439
q=:q,189.498234157028016
q=:q,_23.081551524580124
q=:q,1.0

gm=: p&p. % q&p.

gamma=: 3 : 0 " 0
X=.1|y              NB.X is fractional part of y
N=._2+<.y           NB.X=(y-2)-N; N=_2+y-X=_2+<. y
l=.(N<0)+i.|N       NB.l is list of N integers
e=._1^N<0           NB._1 if N<0, 1 otherwise
w=.X+2+l*e          NB.w is list of N factors
r=.w^e              NB.1%w if N<0, w otherwise
*/r,gm X
)

(g = gamma) x=: _12.5 +i.5 5
(g = gamma) x=: _12.25+i.5 5
(g = gamma) x=: _12.75+i.5 5
(g = gamma) x=: _12.33+i.5 5


NB. !y Stirling's approximation, Abramowitz & Stegun --------------------

sbase  =: %:@(2p1&%) * %&1x1 ^ ]
scorr  =: 1 1r12 1r288 _139r51840 _571r2488320&p.@%
stirlg =: sbase * scorr                         NB. 6.1.37

stirlf =: ^@(1r12&%) * %:@(2p1&*) * %&1x1 ^ ]   NB. 6.1.38

g =: stirlg@>: |@-.@% !
f =: stirlf    |@-.@% !

1e_8 > g 10  +i.2 5
1e_8 > g 10.5+i.2 5
1e_8 > g 10  +10*i.3 5
1e_8 > g 10.5+10*i.3 5

5e_6 > f 10  +i.2 5
5e_6 > f 10.5+i.2 5
5e_6 > f 10  +10*i.3 5
5e_6 > f 10.5+10*i.3 5


NB. !^:_1 ---------------------------------------------------------------
x = !y=:!^:_1 x=:>:i.4 5
x = !y=:!^:_1 x=:>:?20$1e8

(=!.(2^_34) !@(!^:_1))     1+i.7
(=!.(2^_34) !@(!^:_1)) 2 ^   i.7
(=!.(2^_34) !@(!^:_1)) 10^50*i.7

NB. x!y -----------------------------------------------------------------

case =: 2&#. @ (0&>*.(=<.)) @ ([,],-~)
f000 =: !@] % !@[ * !@-~
f001 =: 0:
f010 =: f000
f011 =: _1&^@[ * [ ! (->:)
f100 =: 0:
f101 =: 'impossible'"0
f110 =: _1&^@-~ * !&|&>:~
f111 =: 0:
f    =: f000`f001`f010`f011`f100`f101`f110`f111 @. case " 0

 3.4 (! -: f) _5.6
1e_14 > | _3.6 (! - f) _5.6
_3.6 (! -: f) _5
 3   (! -: f) _5
_3   (! -: f)  5.6
_5   (! -: f) _2
_5   (! -: f) _9

a =: _10+i.21
b =: _10+i.11
c =: i.16
a (!/ -: f/) b
a (!/ -: f/) c

(a%3) (!/ -: f/) c
(a%2) (!/ -: f/) c%2
(a%2) (!/ -: f/) c%3
(a%2) (!/ -: f/) c%o._1
a     (!/ -: f/) c%_4

pascal0=: (0&,+,&0)^:(i.`1:)
(!/~@i. -: |:@pascal0) 32

pascal1=: +/\@(|.!.0)^:(i.`($&1))
(!/~@i. -: pascal1) 32

pascal2=: 1 1&([: +//. */)^:(i.`1:)
(!/~@i. -: |:@pascal2) 32

C =: <:`($:&<: + ($:<:))@.(< *. *@[)"0
(!/~@i. -: C/~@i.) 12

*./ 1 = 0 ! 0.5*_10+i.21
*./ 1 = !~  0.5*_10+i.21

_ -: 3.5 ! _5
_ -: 3.5 ! _6

1 = 0  ! (, -)         0.5*i.12
1 = 0  ! (, -) }. 0j1, 0.5*i.12
1 = 0  ! (, -)         10^20*i.11
1 = 0  ! (, -) }. 0j1, 10^20*i.11

(-: 1&!) (, -)         0.5*i.12
(-: 1&!) (, -) }. 0j1, 0.5*i.12
(-: 1&!) (, -)         10^20*i.11
(-: 1&!) (, -) }. 0j1, 10^20*i.11

test=: 4 : 0
 assert. (x!y) -: (!x) %~ */"1 y-/i.x
 1
)

(x=: i.10) test"0 _ y=: 1e9* 20?@$ 10

1 [ _. !  _. _ __ , (, -) 0.5*i.12  NB. succeeds if J doesn't hang or crash
1 [ _. !~ _. _ __ , (, -) 0.5*i.12  NB. succeeds if J doesn't hang or crash

'domain error' -: 'abc' ! etx 3 4 5
'domain error' -: 'abc' !~etx 3 4 5
'domain error' -: 3 4 5 ! etx <'abc'
'domain error' -: 3 4 5 !~etx <'abc'

'length error' -: 3 4 ! etx 4 5 6
'length error' -: 3 4 !~etx 4 5 6


NB.  x!y all size x combinations of i.y ---------------------------------

start =: i.@-.@-
count =: <:@[ ! <:@[ + |.@start
recur =: [: ; start ,.&.> -@count <@{."0 _ comb0&.<:
test  =: *@[ *. <
basis =: i.@(<: , [)
comb0 =: basis`recur @. test

seed  =: *@[ i.@{ 1 0&,:@(,&1)@>:@(-~)
c0    =: #;.1@~:@({."1"_)
grow  =: c0 ([: ; i.@#@[ ,.&.> -@(+/\.)@[ <@{."0 _ ]) >:
comb1 =: [ grow@]^:(0&>.@<:@[) seed

comb2 =: 4 : 0
 if. 0=x do.
  i.1 0
 else.
  k=.i.>:y-x
  c=.x!&<:x+|.k
  ;k,.&.>(-c){.&.><x comb2&.<:y
 end.
)

comb3 =: 4 : 0
 i=.1+x
 z=.1 0$k=.i.#c=.1,~(y-x)$0
 while. i=.<:i do. z=.;k,.&.>(-c=.+/\.c){.&.><1+z end.
)

comb4 =: 4 : 0
 z=.1 0$k=.i.#c=.1,~(y-x)$0
 for. i.x do. z=.;k,.&.>(-c=.+/\.c){.&.><1+z end.
)

seed5=: i.@(,&0)&.> @ ({.&1) @ <: @ -
cb   =: i.@# ,.&.> [: ,&.>/\. >:&.>
comb5=: [: ; [ cb@]^:[ seed5

comb6 =: 4 : 0
 k=. i.>:d=.y-x
 z=. (d$<i.0 0),<i.1 0
 for. i.x do. z=. k ,.&.> ,&.>/\. >:&.> z end.
 ; z
)

f =: 4 : 0
 'm n'=.x
 t=.y   NB. m comb n
 assert. ($t) -: (m!n),m
 assert. t e.i.n
 assert. m=#@~."1 t
 assert. t -: ~.t
 assert. (i.#t) -: /:t
 assert. (i.m)-:"1 /:"1 t
 1
)

(f comb0/)"1 (i.6),.5
(f comb0/) 4 4
(f comb0/) 0 4
(f comb0/) 0 0

(f comb1/)"1 (i.6),.5
(f comb1/) 4 4
(f comb1/) 0 4
(f comb1/) 0 0

(f comb2/)"1 (i.6),.5
(f comb2/) 4 4
(f comb2/) 0 4
(f comb2/) 0 0

(f comb3/)"1 (i.6),.5
(f comb3/) 4 4
(f comb3/) 0 4
(f comb3/) 0 0

(f comb4/)"1 (i.6),.5
(f comb4/) 4 4
(f comb4/) 0 4
(f comb4/) 0 0

(f comb5/)"1 (i.6),.5
(f comb5/) 4 4
(f comb5/) 0 4
(f comb5/) 0 0

(f comb6/)"1 (i.6),.5
(f comb6/) 4 4
(f comb6/) 0 4
(f comb6/) 0 0

6 (comb0 -: comb1) 11
6 (comb0 -: comb2) 11
6 (comb0 -: comb3) 11
6 (comb0 -: comb4) 11
6 (comb0 -: comb5) 11
6 (comb0 -: comb6) 11

NB. n ifc c      index from combination, the index of combination c
NB. n cfi i      combination from index, the i-th combination of i.n
NB. (m,n) iic c  (m comb0 n) i. c
NB. (m,n) ci i   i{(m comb0 n)

ifc=: 4 : 0 " 0 1
 (+/(i.#y)!x) + ((#y),x) iic y
)

iic=: 4 : 0 " 1
 'm n'=. x
 if. 1>:m do. {.y,0
 else. 
  k=. {.y
  i=. m-1
  j=. i+i.i-n
  (+/i!k{.j) + (x-1,1+k) iic (}.y-1+k)
 end.
)

cfi=: 4 : 0 " 0
 assert. 0<=y
 assert. y=<.y
 assert. y<2^x
 v=. +/\(i.x)!x
 m=. (y<v)i. 1
 (m,x) ci (y-m{0,v)
)

ci=: 4 : 0 " 1 0     
 'm n'=. x
 if. 0=m do.
  i.0
 else.
  v=. +/\ (m-1)!(1-m)}.i.-n
  k=. (v>y) i. 1
  k,(1+k)+(x-1,1+k)ci(y-k{0,v)
 end.
)

(x{5 comb0 9) -: 5 9 ci x=:?20$5!9
(  4 comb0 6) -: 4 6 ci i.!/4 6

(<:20!100x) -: 20 100x iic 80x+i.20
(79x+(3 2$0 1 0 2 1 2),"1]3+i.18) -: 20 100x ci (20!100x)-3 2 1

i0=: [: ; ] ifc&.> i.@>: comb0&.> ]
i1=: i.@(2&^)
i2=: ifc&.> i.@>: comb0&.> ]
i3=: (+/\@}:@(0&,) +&.> i.&.>) @ (i.@>: ! ])
j0=: ] cfi&.> i3
j1=: i.@>: comb0&.> ]

(i0 -: i1)"0 i.5
(i2 -: i3)"0 i.5
(j0 -: j1)"0 i.5

NB. Types, for singleton and not
4 -: (3!:0) 2 ! 10
4 -: (3!:0) 2 ! 2 # 10
8 -: (3!:0) 2 ! 1e15
8 -: (3!:0) 2 ! 2 # 1e15
4 -: (3!:0) 2 ! <. %: +: imax
4 -: (3!:0) 2 ! 2 # <. %: +: imax
8 -: (3!:0) 2 ! 2 + <. %: +: imax
8 -: (3!:0) 2 ! 2 # 2 + <. %: +: imax
(2 # 2 ! >: <. %: +: imax) -: 2 ! 2 # >: <. %: +: imax
(2 # _1.1 ! 1e6) -: _1.1 ! 2 # 1e6
(2 # _10.5 ! 1e6) -: _10.5 ! 2 # 1e6

4!:55 ;:'C a b basis c cfi ci c0 case cb'
4!:55 ;:'comb0 comb1 comb2 comb3 comb4 comb5 comb6'
4!:55 ;:'count em en '
4!:55 ;:'f f000 f001 f010 f011 f100 f101 f110 f111 '
4!:55 ;:'fac g gamma Gauss gm gps grecur grm grow grp '
4!:55 ;:'i iic ifc im i0 i1 i2 i3 j0 j1 k m n p'
4!:55 ;:'pascal0 pascal1 pascal2 q re'
4!:55 ;:'recur rm sbase scorr seed seed5 sinh start stirlf stirlg Stirling '
4!:55 ;:'t test x y z '



epilog''

