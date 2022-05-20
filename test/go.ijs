prolog './go.ijs'
NB. x o.y for x e.i.13 --------------------------------------------------

NB. test failed on small ct
ct   =: 9!:18''
9!:19[2^_42

z =: (_10+?20$20) j. _4+?20$20
x =: 0.1 * _50+?20$100

cir0 =: -.&.(*~)
sin  =: (^ -:@- ^@-)&.j.
cos  =: (^ -:@+ ^@-)@j.
tan  =: %/&(1 2&(o./))
cir4 =: >:&.(*~)
sinh =: ^ -:@- ^@-
cosh =: ^ -:@+ ^@-
tanh =: %/&(5 6&(o./))
cir8 =: -&>:&.(*~)

1e_8 > >./|,(cir0 z) - 0 o.z
1e_8 > >./|,(cir0 x) - 0 o.x

1e_8 > >./|,(sin z) - 1 o.z
1e_8 > >./|,(sin x) - 1 o.x
1e_8 > | 0.841470984807896 - 1 o.1

1e_8 > >./|,(cos z) - 2 o.z
1e_8 > >./|,(cos x) - 2 o.x
1e_8 > | 0.540302305868139 - 2 o.1

1e_8 > >./|,(tan z) - 3 o.z
1e_8 > >./|,(tan x) - 3 o.x
1e_8 > >./| 0.54630249 1255.7655915 - 3 o.0.5 1.57

1e_8 > >./|,(cir4 z) - 4 o.z
1e_8 > >./|,(cir4 x) - 4 o.x

1e_8 > >./|,(sinh z) - 5 o.z
1e_8 > >./|,(sinh x) - 5 o.x
1e_8 > | 1.601919080 - 5 o.1.25

1e_8 > >./|,(cosh z) - 6 o.z
1e_8 > >./|,(cosh x) - 6 o.x
1e_8 > | 1.543080635 - 6 o.1

1e_8 > >./|,(tanh z) - 7 o.z
1e_8 > >./|,(tanh x) - 7 o.x
1e_8 > | 0.86924933 - 7 o.1.33

1e_8 > >./|,(cir8 z) - 8 o.z
1e_8 > >./|,(cir8 x) - 8 o.x

1e_8 > >./|,z -  _9 _11 +/ . o.  9 11 o./z
1e_8 > >./|,x -  _9 _11 +/ . o.  9 11 o./x

1e_8 > >./|,z - _10 _12 */ . o. 10 12 o./z
1e_8 > >./|,x - _10 _12 */ . o. 10 12 o./x

8 -: type 1 2   o. 1.5
8 -: type 1 2 3 o. 1.5


NB. x o.y for x e.-i.13 -------------------------------------------------

sin   =:  1&o.
asin  =: _1&o.
cos   =:  2&o.
acos  =: _2&o.
tan   =:  3&o.
atan  =: _3&o.
cir4m =: 3 : '(>:y)*%:(<:y)%(>:y)'
sinh  =:  5&o.
asinh =: _5&o.
cosh  =:  6&o.
acosh =: _6&o.
tanh  =:  7&o.
atanh =: _7&o.
cir8m =: %:&.-&>:&(*~)

z =: ^0j0.1*?20$100
x =: 0.02*_50+?20$100

1e_8 > >./|,z - sin asin z
1e_8 > >./|,x - sin asin x
1e_8 > >./|,z - asin sin z
1e_8 > >./|,x - asin sin x
1e_8 > /| 0.789 - asin 0.7096490720426565

z =: z * _1^0>9 o.z=:^0j0.1*?20$100
x =: 0.02*_50+?20$100

1e_8 > >./|, z - cos acos z
1e_8 > >./|, x - cos acos x
1e_8 > >./, (|z - acos cos z) <. | (-z) - acos cos -z
1e_8 > >./, (|x - acos cos x) <. | (-x) - acos cos -x
1e_8 > | 0.696 - acos 0.767412932432449

z =: r.?20$100
x =: 0.001 * _1500+?20$3000

1e_8 > >./|,z - tan atan z
1e_8 > >./|,x - tan atan x
1e_8 > >./|,z - atan tan z
1e_8 > >./|,x - atan tan x
1e_8 > | 0.572852247673 - atan 0.645

1e_8 > >./|,(cir4m z) - _4 o.z
1e_8 > >./|,(cir4m x) - _4 o.x

z =: r.?20$100
x =: 0.001 * _1e4+?20$1e5

1e_8 > >./|, z - sinh asinh z
1e_8 > >./|, x - sinh asinh x
1e_8 > >./|, z - asinh sinh z
1e_8 > >./|, x - asinh sinh x
1e_8 > >./| 3.9 5.9 - asinh 24.691103597 182.51736421

a =: (r.?10$100),i.9
a =: a *_1^0>9 o.a
b =: _9}.a

1e_8 > >./|, a - cosh acosh a
1e_8 > >./|, b - acosh cosh b
1e_8 > >./| 1.19 5.8 - acosh 1.795651236 165.151293732

a =: ^0j0.1*?18$100

1e_8 > >./|,a - atanh tanh a

x=:   1.99188402916   2.22881178784
y=: 201.71315737028 201.71563612246
1e_8 > >./| 1.44 6 - atanh (%/x),%/y

1e_8 > >./|,(cir8m a) - _8 o.a


NB. circle functions of the form f&.j.  ---------------------------------

sin  =: 1&o.
cos  =: 2&o.
tan  =: 3&o.

sinh =: 5&o.
cosh =: 6&o.
tanh =: 7&o.

asin  =: _1&o.
acos  =: _2&o.
atan  =: _3&o.

asinh =: _5&o.
acosh =: _6&o.
atanh =: _7&o.

jsinh =: sin&.j.
jcosh =: cos@ j.
jasin =: asinh&.j.
jatan =: atanh&.j.

a=:r.?20$1000

(sinh -: jsinh) a
(cosh -: jcosh) a
(asin -: jasin) a
(atan -: jatan) a


NB. o. ------------------------------------------------------------------

a =: (_4+?20$10) j. _4+?20$10
(o.a) -: a*o.1

p =: (0 4+/1 2 3)o.0.35
q =: 0.3428978074554513492 0.93937271284737892 0.36502849
q =: q,: 0.357189729 1.061877819 0.33637554
1e_8 > >./|, p - q

x =: 0.01*(+ 0j1&*)/_400+?2$900
(2 3 o. x) -: (}.3j4 2 3) o. x
(1 2 o. x) -: (}.3.5 1 2) o. x
(5 o. x)   -: ({:3j4 5) o. x
(6 o. x)   -: ({:3.7 6) o. x
(1 o. a)   -: (4-3) o. a


NB. model of x o. y  ----------------------------------------------------
NB. See Chapter 4 of Abramowitz and Stegun [1964].

pi     =: o.1
sin    =: 1&o.   NB. a function in the C library
cos    =: 2&o.   NB. a function in the C library
sinh   =: 5&o.   NB. a function in the C library
cosh   =: 6&o.   NB. a function in the C library

cir0   =: 1&+   %:@* 1&-
zp4    =: -&0j1 %:@* +&0j1
zp8    =: 0j1&+ %:@* 0j1&-
zm4    =: +&1 * -&1 %:@% +&1
real   =: -:@(++)
imag   =: %&0j2@(-+)
zarc   =: 0j_1&*@^.@*`0: @. (0&=)

zsin   =: ((sin@[ * cosh@]) j. (  cos@[ * sinh@]))/@+.
zcos   =: ((cos@[ * cosh@]) j. (-@sin@[ * sinh@]))/@+.
ztan   =: zsin % zcos
zsinh  =: zsin&.j.
zcosh  =: zcos@j.
ztanh  =: ztan&.j.

zasin  =: zasinh&.j.
zacos  =: (-:pi)&-@zasin
zatan  =: zatanh&.j.
zasinh =: (^.@+ zp4)`($:&.-) @. (0&>@real)
zacosh =: ]`(j.@|@imag)@.(0&>@real) @ (^.@+ zm4)
zatanh =: 1&+ -:@^.@% 1&-

cirp   =: (cir0@])`(zsin@])`(zcos@])`(ztan@])`(zp4@])`(zsinh@])`(zcosh@])`(ztanh@])`(zp8@])`(real@])`(|@])`(imag@])`(zarc@]) @. [
cirm   =: (cir0@])`(zasin@])`(zacos@])`(zatan@])`(zm4@])`(zasinh@])`(zacosh@])`(zatanh@])`(-@zp8@])`]`(+@])`(j.@])`(r.@]) @. (|@[)
circle =: cirp`cirm @. (0&>@[) " 0

a =: r.?20$1000
( i.13) (o./ = circle/) a

NB. ebi following can fail with certain arguments
NB. it fails in 602 and 701 the same way it fails in 83
NB.    z=.   0.99999999818263596j6.0288706691585265e_5
NB.    9!:11[18
NB.    _2 o. z
NB.    -2 circle z

(-i.13) (o./ = circle/) a

'domain error' -: o. etx 'Opposable Thumbs'
'domain error' -: o. etx ;:'sui generis'

'domain error' -: 'a'  o. etx 3.4 
'domain error' -: 3.5  o. etx 3.4
'domain error' -: 3j5  o. etx 3.4
'domain error' -: (<4) o. etx 3.4
'domain error' -: 17   o. etx 2  
'domain error' -: _17  o. etx 2 
 
'domain error' -: 1    o. etx 'abc' 
'domain error' -: 1    o. etx <'abc'

'length error' -: 2 3  o. etx 0.5 0.4 0.3


NB. o. on extreme arguments ---------------------------------------------

'limit error' -: 1 o. etx    9e9
'limit error' -: 1 o. etx o. 9e9
'limit error' -: 1 o. etx {. 9e9 0j1
'limit error' -: 1 o. etx   _9e9
'limit error' -: 1 o. etx o._9e9
'limit error' -: 1 o. etx {._9e9 0j1
'limit error' -: 1 o. etx   _
'limit error' -: 1 o. etx {._    0j1
'limit error' -: 1 o. etx   __
'limit error' -: 1 o. etx {.__   0j1
'limit error' -: 1 o. etx 1e9j2
'limit error' -: 1 o. etx _1e9j2

(j. -:^600)   -: 1 o. 0j600
(j.--:^600)   -: 1 o. 0j_600

NB. A&S 4.3.55  sin z = sin x cosh y + i cos x sinh y
((_*1 o. x) j. _  * 2 o. x)  = 1 o. x j.  999 [ x=: i:6
((_*1 o. x) j. __ * 2 o. x)  = 1 o. x j. _999 [ x=: i:6

'limit error' -: 2 o. etx    9e9
'limit error' -: 2 o. etx o. 9e9
'limit error' -: 2 o. etx {. 9e9 0j1
'limit error' -: 2 o. etx   _9e9
'limit error' -: 2 o. etx o._9e9
'limit error' -: 2 o. etx {._9e9 0j1
'limit error' -: 2 o. etx   _
'limit error' -: 2 o. etx {._    0j1
'limit error' -: 2 o. etx   __
'limit error' -: 2 o. etx {.__   0j1
'limit error' -: 2 o. etx 1e9j2
'limit error' -: 2 o. etx _1e9j2

(-:^600)      -: 2 o. 0j600
(-:^600)      -: 2 o. 0j_600

NB. A&S 4.3.56  cos z = cos x cosh y - i sin x sinh y
((_*2 o. x) j. __ * 1 o. x)  = 2 o. x j.  999 [ x=: i:6
((_*2 o. x) j. _  * 1 o. x)  = 2 o. x j. _999 [ x=: i:6

'limit error' -: 3 o. etx    9e9
'limit error' -: 3 o. etx o. 9e9
'limit error' -: 3 o. etx {. 9e9 0j1
'limit error' -: 3 o. etx   _9e9
'limit error' -: 3 o. etx o._9e9
'limit error' -: 3 o. etx {._9e9 0j1
'limit error' -: 3 o. etx _
'limit error' -: 3 o. etx __
'limit error' -: 3 o. etx 1e9j2
'limit error' -: 3 o. etx _1e9j2

_  = 5 o.    1000
_  = 5 o. {. 1000 0j5
_  = 5 o.    _
_  = 5 o. {. _    0j5
__ = 5 o.   _1000
__ = 5 o. {._1000 0j5
__ = 5 o.   __
__ = 5 o. {.__    0j5

'limit error' -: 5 o. etx 2j1e9
'limit error' -: 5 o. etx 2j_1e9

((_  * 2 o. y) j. _ * 1 o. y) = 5 o.  999 j.  y=: i:6
((__ * 2 o. y) j. _ * 1 o. y) = 5 o. _999 j.  y=: i:6

_ = 6 o.    1000
_ = 6 o. {. 1000 0j5
_ = 6 o.    _
_ = 6 o. {. _    0j5
_ = 6 o.   _1000
_ = 6 o. {._1000 0j5
_ = 6 o.   __
_ = 6 o. {.__    0j5

'limit error' -: 6 o. etx 2j1e9
'limit error' -: 6 o. etx 2j_1e9

((_ * 2 o. y) j. _  * 1 o. y) = 6 o.  999 j.  y=: i:6
((_ * 2 o. y) j. __ * 1 o. y) = 6 o. _999 j.  y=: i:6

1  -: 7 o.  1000
1  -: 7 o.  _
_1 -: 7 o. _1000
_1 -: 7 o. __

1  -: 7 o.  710j2
1  -: 7 o.  710j1e9
_1 -: 7 o. _710j2
_1 -: 7 o. _710j1e9

'limit error' -: 7 o. etx 2j6e8
'limit error' -: 7 o. etx 2j_6e8

0.5p1 _0.5p1 -: _3 o. 1e16 _1e16
0.5p1 _0.5p1 -: _3 o. _ __
(0*x)={:"1 +.   _3 o. x=:1e_10*_5000+?4 5$1e4

NB. NaN on arm clang
0 = _2&o. 1j0
_1.5707963267949 = _1 o. _1

9!:19 ct

4!:55 ;:'a acos acosh asin asinh atan atanh b cir0 cir4 '
4!:55 ;:'cir4m cir8 cir8m circle cirm cirp cos cosh ct imag '
4!:55 ;:'jasin jatan jcosh jsinh p pi q real sin sinh '
4!:55 ;:'tan tanh x y z zacos zacosh zarc zasin zasinh zatan '
4!:55 ;:'zatanh zcos zcosh zm4 zp4 zp8 zsin zsinh ztan ztanh '



epilog''

