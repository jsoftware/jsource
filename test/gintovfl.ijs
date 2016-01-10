NB. integer operations --------------------------------------------------

B =: IF64{31 63

V =: 1 : 'x ;  x& x:'
E =: 1 : 'x -: x&.x:'
EI=: 1 : 'x E *. 4&=@type@:x'
C =: 1 : 'x E *. x E~'
CI=: 1 : 'x C *. 4&=@type@:x'
xi=: x:^:_1

permute=: ?~@# { ]


NB. integers remain as integers  ----------------------------------------

<:   EI imax
>:   EI imin

f=: 3 : 0
 xx=: y $ xi imax <.@% x: y
 assert. +/   EI  xx
 assert. +/\  EI  xx
 assert. +/\. EI  xx
 assert. +/   EI -xx
 assert. +/\  EI -xx
 assert. +/\. EI -xx
 assert. +/   EI yy=: permute ,   xx  ,. -     (#xx) ?@$ 100
 assert. +/\  EI yy
 assert. +/\. EI yy
 assert. +/   EI yy=: permute , (-xx) ,.       (#xx) ?@$ 100
 assert. +/\  EI yy
 assert. +/\. EI yy
 assert. +/   EI yy=: permute ,   xx  (- ,. ]) (#xx) ?@$ 100
 assert. +/\  EI yy
 assert. +/\. EI yy
 assert. +/   EI yy=: permute , (-xx) (+ ,. ]) (#xx) ?@$ 100
 assert. +/\  EI yy
 assert. +/\. EI yy
 1
)

f 3
f 4
f"0 >: 5 ?@$ 100

+: EI x=: xi imax <.@% 2x
+: EI x=: xi imin >.@% 2x

f=: 3 : 0
 n=: <. (0=(2^.yy)|B) -~ yy ^. imax [ yy=: y
 assert. */   EI xx=: n $ yy
 assert. */\  EI xx
 assert. */\. EI xx
 assert. */   EI xx=: permute yy * n$1 _1
 assert. */\  EI xx
 assert. */\. EI xx
 assert. */   EI xx=: permute n # q: yy
 assert. */\  EI xx
 assert. */\. EI xx
 assert. */   EI xx=: (1,0=(<:#xx) ?@$ 10) */;.1 xx
 assert. */\  EI xx
 assert. */\. EI xx
 1
)

f"0 <. (2+10 ?@$ 20) %: imax
f"0 <. 2^1+i.20
f"0 ] 2 + 10 ?@$ 100
f"0 ] 2 + 10 ?@$ 1000

*/    EI x=: B$_2
*/    EI x=: (1,0=(B-1) ?@$ 4) */;.1 B$_2
*/\   EI x=: B$_2
*/\   EI x=: (1,0=(B-1) ?@$ 4) */;.1 B$_2
*/\.  EI x=: B$_2
*/\.  EI x=: (1,0=(B-1) ?@$ 4) */;.1 B$_2

*:    EI x=: <.   (%:2) * 2^IF64{15 31
*:    EI x=: >. - (%:2) * 2^IF64{15 31

-     EI imin+1 2 3
-     EI imax-1 2 3

-.    EI imin + 2

|     EI x=: imin + 1+10 ?@$ 100

+/ .* EI x=: (n,n)   $ <. n %: imax % !n=: 3
+/ .* EI x=: (n,n) ?@$ <. n %: imax % !n=: 5

-/ .* EI x=: 10 2 2 ?@$ <. 2 %: imax

#.    EI x=: (      IF64{31 63)   $ 1
#.    EI x=: (2 3 4,IF64{31 63) ?@$ 2

0 _1   +  CI imax
1      +  CI imax-1
x      +  CI imax - x=: 10 ?@$ 10
x      +  CI imax - x=: 10 ?@$ 1e6
0      +  CI imax,imin
0 1    +  CI imin
1      +  CI imin+1
(-x)   +  CI imin + x=: 10 ?@$ 10
(-x)   +  CI imin + x=: 10 ?@$ 1e6
imin   +  CI imax

imax   +. CI x=: imin
imax   +. CI x=: 0 1,imax
imax   +. CI x=: q: imax 
imax   +. CI x=: q: imax
imin   +. CI x=: 1 _1
imin   +. CI x=: 1 2 4 8 16
imin   +. CI x=: <. 2^i.IF64{30 62

0      -  EI x=: imax
imax   -  EI x=: 10 ?@$ 100
imax   -  EI x=: 10 ?@$ 1e6
x      -  EI imax [ x=: 10 ?@$ 5
x      -  EI imax [ x=: 10 ?@$ 100
x      -  EI imax [ x=: 10 ?@$ 1e6
imin   -  EI 0
imin   -  EI x=: - 10 ?@$ 5
imin   -  EI x=: - 10 ?@$ 100
imin   -  EI x=: - 10 ?@$ 1e6
imin   -  EI x=: - imax
x      -  EI imin [ x=: - 1 + 10 ?@$ 5 
x      -  EI imin [ x=: - 1 + 10 ?@$ 100 
x      -  EI imin [ x=: - 1 + 10 ?@$ 1e6 

0 1 _1 *  CI imax
0 1    *  CI imin
2      *  CI xi imax <.@% 2x
x      *  CI xi imax <.@% x: x=: 1+20 ?@$ 10
x      *  CI xi imax <.@% x: x=: 1+100 ?@$ 100
2      *  CI xi imin >.@% 2x
x      *  CI xi imin >.@% x: x=: 1+20 ?@$ 10
x      *  CI xi imin >.@% x: x=: 1+100 ?@$ 100

imax   *. CI 1,q: imax 
imin   *. CI <. 2^ 10 ?@$ IF64{30 62

imax   |  EI imin
imin   |  EI imax

n      #. EI 1 $~ <. n ^. 1 + imax * _1 + n=: 2 
n      #. EI 1 $~ <. n ^. 1 + imax * _1 + n=: 3
n      #. EI 1 $~ <. n ^. 1 + imax * _1 + n=: 4
n      #. EI 1 $~ <. n ^. 1 + imax * _1 + n=: 5
n (#. $&1:)"0 EI x=: <.n^.1+imax*_1+n=: 2+10 ?@$ 100

x   +/ .* CI y  [ x=: 2$1 [ y=: 2$xi imax <.@% 2x
x   +/ .* CI y  [ x=: 3$1 [ y=: 3$xi imax <.@% 3x
1 1 +/ .* CI y=: 1,imax-1
1 1 +/ .* CI y=: 1,imax-2
1 1 +/ .* CI y=: 1,imax-3

(2 - 1 1) +/@:* CI 1,imax-1
(2 - 1 1) +/@:* CI 1,imax-2
_1 2      +/@:* CI 1,x:^:_1 <.imax%2x


NB. integers overflowing into doubles -----------------------------------

- E imin

imax   +  C x=:   10 ?@$ 100
imin   +  C x=: - 10 ?@$ 100
(-x)   +  C imin + x=: 10 ?@$ 100

imin   +. C imin
imin   +. C 0

imax   -  E _1
imax   -  E x=: - 10 ?@$ 100
imin   -  E x=:   10 ?@$ 100

_1     *  C imin
imax   *  C x=: _50 + 10 ?@$ 100
imin   *  C x=: _50 + 10 ?@$ 100

| E imin

-/ .* E x=: (=i.2) * >. 2 %: imax
-/ .* E x=: 10 2 2 ?@$ <. 2^IF64{17 33

x   +/ .* C y  [ x=: 4 1 1 1  1 [ y=: 0 _1 0  1,imax
x   +/ .* C y  [ x=: 4 1 1 1  2 [ y=: 0  0 0  0,imax
x   +/ .* C y  [ x=: 4 1 1 1  1 [ y=: 0 _1 0 _1,imin
x   +/ .* C y  [ x=: 4 1 1 1 _1 [ y=: 0  0 0  0,imin
x   +/ .* E y  [ x=: 4 1 1 1  1 (?7)}7 5 ?@$ 100 [ y=: (0 _1 0  1,imax) (<a:;?11)}5 11 ?@$ 100
x   +/ .* E y  [ x=: 4 1 1 1  2 (?7)}7 5 ?@$ 100 [ y=: (0  0 0  0,imax) (<a:;?11)}5 11 ?@$ 100
x   +/ .* E y  [ x=: 4 1 1 1  1 (?7)}7 5 ?@$ 100 [ y=: (0 _1 0 _1,imin) (<a:;?11)}5 11 ?@$ 100
x   +/ .* E y  [ x=: 4 1 1 1 _1 (?7)}7 5 ?@$ 100 [ y=: (0  0 0  0,imin) (<a:;?11)}5 11 ?@$ 100


NB. integers overflowing into doubles, asm routines ---------------------

f=: 1 : 0
:
 'A bg'=. x [ nn=.y
 assert. u   f. E"1 data=: A i} bg {~(       nn,nn) ?@$ #bg [ i=. ,&.>~ i.nn
 assert. u   f. E"2 data=: A i} bg {~((*:nn),nn,nn) ?@$ #bg [ i=. (i.*:nn),&.>,{i.&.>nn,nn
 assert. u"1 f. E"2 data
 1
)

(imax; 1000 ?@$ 1e4) +/   f"1 0 >: i.7
(imax; 1000 ?@$ 1e4) +/\  f"1 0 >: i.7
(imax; 1000 ?@$ 1e4) +/\. f"1 0 >: i.7
(imin;-1000 ?@$ 1e4) +/   f"1 0 >: i.7
(imin;-1000 ?@$ 1e4) +/\  f"1 0 >: i.7
(imin;-1000 ?@$ 1e4) +/\. f"1 0 >: i.7

(imax; 1 2)          */   f"1 0 >: i.7
(imax; 1 2)          */\  f"1 0 >: i.7
(imax; 1 2)          */\. f"1 0 >: i.7
(imin; 1 2)          */   f"1 0 >: i.7
(imin; 1 2)          */\  f"1 0 >: i.7
(imin; 1 2)          */\. f"1 0 >: i.7

(imax; 1000 ?@$ 1e4) -/   f"1 0 >: i.7
(imax; 1000 ?@$ 1e4) -/\  f"1 0 >: i.7
(imax; 1000 ?@$ 1e4) -/\. f"1 0 >: i.7
(imax;-1000 ?@$ 1e4) -/   f"1 0 >: i.7
(imax;-1000 ?@$ 1e4) -/\  f"1 0 >: i.7
(imax;-1000 ?@$ 1e4) -/\. f"1 0 >: i.7
(imin; 1000 ?@$ 1e4) -/   f"1 0 >: i.7
(imin; 1000 ?@$ 1e4) -/\  f"1 0 >: i.7
(imin; 1000 ?@$ 1e4) -/\. f"1 0 >: i.7
(imin;-1000 ?@$ 1e4) -/   f"1 0 >: i.7
(imin;-1000 ?@$ 1e4) -/\  f"1 0 >: i.7
(imin;-1000 ?@$ 1e4) -/\. f"1 0 >: i.7


4!:55 ;:'B C CI data E EI f n nn permute V x xi xx y yy'


