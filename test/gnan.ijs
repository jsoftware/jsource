NB. NaN -----------------------------------------------------------------

t=. }.&.> 3.4 _ __; 3j4 _ __; 34x _ __ ; 3r4 _ __
pinf=: {.&.> t
ninf=: {:&.> t
inf =: pinf,ninf
zero=: 0 ; -&.>~ 2 ; 3.4 ; 3j4 ; 3x ; 3r4
znan=: {. _. 3j4

NB. =<>_  +*-%  ^$~|  .:,;  #!/\  []{}  "`@&?


NB. =<>_ ----------------------------------------------------------------

=&>/~ pinf
=&>/~ ninf
-. pinf =&>/ ninf


NB. +*-% ----------------------------------------------------------------

(<'NaN error') = pinf + etx&.>/ ninf 
(<'NaN error') = ninf + etx&.>/ pinf

'NaN error' -: (1$.1e9;0;_) + etx 1$.1e9;0;__

(<'NaN error') = inf +.etx&.>/  x=: 20 ?@$ 2
(<'NaN error') = inf +.etx&.>/~ x
(<'NaN error') = inf +.etx&.>/  x=: 0,    _5e3+20 ?@$ 1e4
(<'NaN error') = inf +.etx&.>/~ x
(<'NaN error') = inf +.etx&.>/  x=: 0, o. _5e3+20 ?@$ 1e4
(<'NaN error') = inf +.etx&.>/~ x
(<'NaN error') = inf +.etx&.>/  x=: 0, r. _5e3+20 ?@$ 1e4
(<'NaN error') = inf +.etx&.>/~ x
(<'NaN error') = inf +.etx&.>/  x=: 0,  _5000 +20 ?@$ 10000x
(<'NaN error') = inf +.etx&.>/~ x
(<'NaN error') = inf +.etx&.>/  x=: 0, 4%~_5000 +20 ?@$ 10000x
(<'NaN error') = inf +.etx&.>/~ x

'domain error' -: _. +: etx 1
'domain error' -: 0  +: etx _.

(<0) = zero *&.>/ inf
(<0) = inf  *&.>/ zero

NB. x=:  4{. 2 (3!:3) 0j1
NB. t=: _4{. 2 (3!:3) _. __ 0 _
NB. y=: 3!:2&.> (<x),&.> (,{;~i.#t){&.><t
NB. (<0) = zero *&.> /  y 
NB. (<0) = zero *&.> /~ y

(<'NaN error') = * etx&.> j./~ _ __

'NaN error' -: 3j4  * etx _j__
'NaN error' -: 3j_4 * etx _j_

(<'NaN error') = inf *.etx&.>/  x=: 20 ?@$ 2
(<'NaN error') = inf *.etx&.>/~ x
(<'NaN error') = inf *.etx&.>/  x=: 0,    _5e3+20 ?@$ 1e4
(<'NaN error') = inf *.etx&.>/~ x
(<'NaN error') = inf *.etx&.>/  x=: 0, o. _5e3+20 ?@$ 1e4
(<'NaN error') = inf *.etx&.>/~ x
(<'NaN error') = inf *.etx&.>/  x=: 0, r. _5e3+20 ?@$ 1e4
(<'NaN error') = inf *.etx&.>/~ x
(<'NaN error') = inf *.etx&.>/  x=: 0,  _5000 +20 ?@$ 10000x
(<'NaN error') = inf *.etx&.>/~ x
(<'NaN error') = inf *.etx&.>/  x=: 0, 4%~_5000 +20 ?@$ 10000x
(<'NaN error') = inf *.etx&.>/~ x

'domain error' -: _. *: etx 1
'domain error' -: 0  *: etx _.

(<'NaN error') = pinf - etx&.>/ pinf 
(<'NaN error') = ninf - etx&.>/ ninf 

'NaN error' -: (1$.1e9;0;_) - etx 1$.1e9;0;_

(<0) = zero %&.>/ zero

(<'NaN error') = % etx&.>/~ inf

'NaN error' -: (1$.1e9;0;_) % etx 1$.1e9;0;__

0j_ _ -: %:        __ _
0j_ _ -: %: }. 3j4 __ _


NB. ^$~| ----------------------------------------------------------------

0  = ^ __
0  = ^ {. __ 3j4

(<'NaN error') = ^. etx&.>/~ zero

NB. funny business if moved as doubles
(a.{~32$240 255) -: 32 $ 240 255{a.

(($zero)$,:inf) -: zero |&.>/ inf

(<'NaN error') = inf | etx&.>/~ x=:        1+7 ?@$ 40 
(<'NaN error') = inf | etx&.>/~ x=:      - 1+7 ?@$ 40 
(<'NaN error') = inf | etx&.>/~ x=:  100%~ 1+7 ?@$ 40 
(<'NaN error') = inf | etx&.>/~ x=: _100%~ 1+7 ?@$ 40 
(<'NaN error') = inf | etx&.>/~ x=:       r. 7 ?@$ 40 
(<'NaN error') = inf | etx&.>/~ x=:        1+7 ?@$ 40x
(<'NaN error') = inf | etx&.>/~ x=:      - 1+7 ?@$ 40x
(<'NaN error') = inf | etx&.>/~ x=:  100%~ 1+7 ?@$ 40x
(<'NaN error') = inf | etx&.>/~ x=: _100%~ 1+7 ?@$ 40x

x ="1 pinf |&>/  x=:         1+7 ?@$ 40 
x ="1 pinf |&>/  x=:   100%~ 1+7 ?@$ 40
x ="1 pinf |&>/  x=:         1+7 ?@$ 40x 
x ="1 pinf |&>/  x=:   100%~ 1+7 ?@$ 40x 

pinf = pinf |&.>/ x=:      - 1+7 ?@$ 40
pinf = pinf |&.>/ x=: _100%~ 1+7 ?@$ 40
pinf = pinf |&.>/ x=:      - 1+7 ?@$ 40x 
pinf = pinf |&.>/ x=: _100%~ 1+7 ?@$ 40x 

ninf = ninf |&.>/ x=:        1+7 ?@$ 40
ninf = ninf |&.>/ x=:  100%~ 1+7 ?@$ 40
ninf = ninf |&.>/ x=:        1+7 ?@$ 40x 
ninf = ninf |&.>/ x=:  100%~ 1+7 ?@$ 40x 

x ="1 ninf |&>/ x=:       - 1+7 ?@$ 40
x ="1 ninf |&>/ x=:  _100%~ 1+7 ?@$ 40
x ="1 ninf |&>/ x=:       - 1+7 ?@$ 40x 
x ="1 ninf |&>/ x=:  _100%~ 1+7 ?@$ 40x 


NB. .:,; ----------------------------------------------------------------

'NaN error' -: _ _  +/ .* etx 1 _1
'NaN error' -: _ __ +/ .* etx 1  1

x=: o. 3 4 ?@$ 2
y=: _ 1 2 3 4 {~ 4 5 ?@$ 5
(x +/ .* y) -: x +/@(*"1 _) y

f=: +/ @ (*"1 _)
x=: 11 23 $ _ (43 ?@$ 253)} (253 ?@$ 0) * 253 ?@$ 2
y=: 23 13 $ _ (43 ?@$ 299)} (299 ?@$ 0) * 299 ?@$ 2
x (f -: +/ .*) y

det=: -/ .*

x=: x;(x+-~0j5);x: x=: (2 2$_ __ 1 1) ({;~0 1)}=i.4
_              = det    & > (2 2;3 3;4 4) {.&.>/ x
x=: x;(x+-~0j5);x: x=: (2 2$__ _ 1 1) ({;~0 1)}=i.4
__             = det    & > (2 2;3 3;4 4) {.&.>/ x
x=: x;(x+-~0j5);x: x=: (2 2$_ _  1 1) ({;~0 1)}=i.4
(<'NaN error') = det etx&.> (2 2;3 3;4 4) {.&.>/ x

3j4 _. _. _. -:&(3!:1) 3j4 , _. _. _.

(<'NaN error') = (<1 0) +/;.1 etx&.>  pinf ,&.>/ ninf
(<'NaN error') = (<1 0) %/;.1 etx&.>  pinf ,&.>/ ninf

(<'NaN error') = (<1 0) ([: ; <@(+/\);.1) etx&.> pinf ,&.>/ ninf


NB. #!/\ ----------------------------------------------------------------

NB. funny business if moved as doubles
(2#_1e6 _1e6)    -: _1e6 _1e6 _1e6 _1e6
(3 2$_834524)    -: 1 0 1 0 1 # 5 2$_834524
(3 8$240 255{a.) -: 1 0 1 0 1 # 5 8$240 255{a.

(<'NaN error') = ! etx&.>/~ pinf
(<'NaN error') = ! etx&.>/~ ninf

8  -: (3!:0) _.
16 -: (3!:0) _. 3j4

x -:&(3!:1) (3!:2) 3!:1 x=: _. __ _12.34 0 45.678 _
x -:&(3!:1) (3!:2) 3!:1 x=: _. __ _12.34 0 45.678 _ 3j4

x -:&(3!:1) _1 (3!:5) 1 (3!:5) x=: 3 4 _.
x -:&(3!:1) _2 (3!:5) 2 (3!:5) x=: 3 4 _.

((i.1e4) e. i) -: (128!:5) _. i}1e4 ?@$ 0 [ i=: 200 ?@$ 1e4
0 0 0 -: (128!:5) 0 1 2
0 0 0 -: (128!:5) 'abc'

(<'NaN error') = +/ etx&.> pinf ,&.>/ ninf
(<'NaN error') = -/ etx&.> ,&.>/~ pinf
(<'NaN error') = -/ etx&.> ,&.>/~ ninf
(<'NaN error') = %/ etx&.> ,&.>/~ inf

'NaN error' -: +/ etx _ __ (2?1e9)}1$.1e9
'NaN error' -: -/ etx _    (2?1e9)}1$.1e9
'NaN error' -: %/ etx _    (2?1e9)}1$.1e9;0;1.5-0.5

(<'NaN error') = (+/%#)etx&.>             pinf ,&.>/ ninf
(<'NaN error') = (+/%#)etx&.> (<2 3) ,&.> pinf ,&.>/ ninf

(<'NaN error') = +//. etx&.> 2 2&$@(1&,)&.> pinf ,&.>/ ninf

(<'NaN error') = (<1 1) +//.@(*/) etx&.> pinf ,&.>/ ninf

(<'NaN error') = (<'aa') +//. etx&.> pinf ,&.>/ ninf

(<'NaN error') = (<'aa') (+/%#)/. etx&.> pinf ,&.>/ ninf

(<'NaN error') = +/\ etx&.> pinf ,&.>/ ninf
(<'NaN error') = -/\ etx&.> ,&.>/~ pinf
(<'NaN error') = -/\ etx&.> ,&.>/~ ninf
(<'NaN error') = %/\ etx&.> ,&.>/~ inf

'NaN error' -: 3 +/\     etx _ 4 __ 5
'NaN error' -: 3 (+/%#)\ etx _ 4 __ 5

2 _ _ _ 5 -: 3 (+/%#)\ 1 2 3 _ 4 5 6
(<,_ ) = 3 +/\    &.> 3#&.> pinf
(<,__) = 3 +/\    &.> 3#&.> ninf
(<,_ ) = 3 (+/%#)\&.> 3#&.> pinf
(<,__) = 3 (+/%#)\&.> 3#&.> ninf

(<'NaN error') = +/\. etx&.> pinf ,&.>/ ninf
(<'NaN error') = -/\. etx&.> ,&.>/~ pinf
(<'NaN error') = -/\. etx&.> ,&.>/~ ninf
(<'NaN error') = %/\. etx&.> ,&.>/~ inf

3  (+/\. -: +/&>@(<\.)) 3 1 4 _ 1 4 9
_2 (+/\. -: +/&>@(<\.)) 3 1 4 _ 1 4 9


NB. []{} ----------------------------------------------------------------


NB. "`@&? ---------------------------------------------------------------

'_.' -: ": _.
'_.' -: ": znan

(<,'_') = ":&.> pinf
(<'__') = ":&.> ninf

x=: 1 _1 ; 3.5 _3.5 ; 3j4 _3j_4; 3 _3x ; 3r5 _3r5
(<'NaN error') = x +/@:* etx&.>/ 2#&.>inf

x=: 2#&.> 2; 3.5 ; 3j4 ; _3x ; 3r5
(<'NaN error') = x -/@:* etx&.>/ 2#&.>inf

(<0) = zero +/@:*&.>/ pinf ,&.>/ ninf

'NaN error' -: +/@, etx    2 2$1 _ __
'NaN error' -: +/@, etx $. 2 2$1 _ __

'domain error' -:    ? etx _. 4
'domain error' -: _. ? etx 5
'domain error' -: 5  ? etx _.
'domain error' -: _. ? etx _.


NB. a-z -----------------------------------------------------------------

'NaN error' -: _ __ H. '' etx  1
'NaN error' -: _ _  H. '' etx _1

'NaN error' -: _ __ H. '' etx  1+-~0j5
'NaN error' -: _ _  H. '' etx _1+-~0j5

'domain error' -: i. etx _. 

'NaN error' -: 0 _1 1 p. etx _
'NaN error' -: 0  1 1 p. etx __
'NaN error' -: 5 _ _ p. etx _1
_ = 5 _ _ p. 1

t=: (i:12) o. etx&.>/ , j./~ __ 0 _
-. +./@('_.'&E.)@":&> t

_1 1 = 7 o. _1e9 1e9
_1 1 = 7 o. __ _

((i: 12) o. etx&.> _ ) = (i:12) o. etx&.> {. _  0j1
((i: 12) o. etx&.> __) = (i:12) o. etx&.> {. __ 0j1

'NaN error' -: _ _  p.!.1 etx _1
'NaN error' -: _ __ p.!.1 etx  1

'NaN error' -: x: etx _.
'NaN error' -: x: etx 3 4 _.


4!:55 ;:'d det f i inf ninf pinf t x y zero znan'


