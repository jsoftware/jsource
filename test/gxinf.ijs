NB. x: and infinity -----------------------------------------------------

match=: -:&(3!:1)
xi   =: x:^:_1

( 64=type x), 5 _ -: x=: 5x  _
( 64=type x), 5 _ -: x=: 5r1 _

(128 -: type x), _  1r2 match x=:x:  _ 0.5
(128 -: type x), __ 1r2 match x=:x: __ 0.5

(8 -: type x), _ __ 225 match x=:xi _ __ 225x
(8 -: type x), _ __ 2   match x=:xi _ __ 2x
(8 -: type x), _ __ 0.5 match x=:xi _ __ 1r2

0r1 -:  5r_
0r1 -: _5r_

'ill-formed number' -: ex ' _r_ '
'ill-formed number' -: ex ' _r__'
'ill-formed number' -: ex '__r_ '
'ill-formed number' -: ex '__r__'


NB. infinite integers and rationals and comparatives --------------------

_  5x  = _  5x
__ 5x  = __ 5x
_  5r1 = _  5r1
__ 5r1 = __ 5r1
_  5x  = _  5r1
__ 5x  = __ 5r1

0 0 -: _  2x =  99999x
0 0 -: _  2x = _99999x
0 0 -: __ 2x =  99999x
0 0 -: __ 2x = _99999x

pinf=: {. _  5x
ninf=: {. __ 5x

pinf > ninf
pinf > _99999x
pinf > - 10^100x
pinf > 0x
pinf > 99999x
pinf > 10^100x

(  10^100x) > ninf
99999x      > ninf
0x          > ninf
_99999x     > ninf
(- 10^100x) > ninf

-. pinf > pinf
-. ( 10^100x) > pinf
-. (-10^100x) > pinf
-. ninf > pinf
-. ninf > 0x
-. ninf > ninf

pinf >: ninf
pinf >: _99999x
pinf >: - 10^100x
pinf >: 0x
pinf >: 99999x
pinf >: 10^100x
pinf >: pinf

(  10^100x) >: ninf
99999x      >: ninf
0x          >: ninf
_99999x     >: ninf
(- 10^100x) >: ninf
ninf        >: ninf

-. ( 10^100x) >: pinf
-. (-10^100x) >: pinf
-. ninf >: pinf
-. ninf >: 0x


NB. infinite integers and rationals and primitives ----------------------

_  5x match _  2x +  _ 3x
_  5x match _  2x +  5 3x
_  5x match _  2x + _5 3x
__ 5x match __ 2x + __ 3x
__ 5x match __ 2x +  5 3x
__ 5x match __ 2x + _5 3x

_  5r2 match _  2r2 +  _ 3r2
_  5r2 match _  2r2 +  5 3r2
_  5r2 match _  2r2 + _5 3r2
__ 5r2 match __ 2r2 + __ 3r2
__ 5r2 match __ 2r2 +  5 3r2
__ 5r2 match __ 2r2 + _5 3r2

'NaN error' -: _  2x  + etx __ 5x
'NaN error' -: _  2r1 + etx __ 5r4
'NaN error' -: __ 2x  + etx _  5x
'NaN error' -: __ 2r1 + etx _  5r4


y=: }. 5x _ __
'NaN error' -: y    +. etx  12x
'NaN error' -: y    +. etx _12x
'NaN error' -:  12x +. etx y
'NaN error' -: _12x +. etx y
'NaN error' -: y +. etx  y
'NaN error' -: y +.  etx |.y

y=: }. 5r1 _ __
'NaN error' -: y    +. etx  12x
'NaN error' -: y    +. etx _12x
'NaN error' -:  12x +. etx y
'NaN error' -: _12x +. etx y
'NaN error' -: y +. etx  y
'NaN error' -: y +.  etx |.y

_  5x match _  8x - __ 3x
_  5x match _  8x -  5 3x
_  5x match _  8x - _5 3x
__ 5x match __ 8x -  _ 3x
__ 5x match __ 8x -  5 3x
__ 5x match __ 8x - _5 3x

__ _5x match _  8x -~__ 3x
__ _5x match _  8x -~ 5 3x
__ _5x match _  8x -~_5 3x
_  _5x match __ 8x -~ _ 3x
_  _5x match __ 8x -~ 5 3x
_  _5x match __ 8x -~_5 3x

_  5r2 match _  8r2 - __ 3r2
_  5r2 match _  8r2 -  5 3r2
_  5r2 match _  8r2 - _5 3r2
__ 5r2 match __ 8r2 -  _ 3r2
__ 5r2 match __ 8r2 -  5 3r2
__ 5r2 match __ 8r2 - _5 3r2

__ _5r2 match _  8r2 -~__ 3r2
__ _5r2 match _  8r2 -~ 5 3r2
__ _5r2 match _  8r2 -~_5 3r2
_  _5r2 match __ 8r2 -~ _ 3r2
_  _5r2 match __ 8r2 -~ 5 3r2
_  _5r2 match __ 8r2 -~_5 3r2

'NaN error' -: _  2x  - etx _  5x
'NaN error' -: _  2r1 - etx _  5r4
'NaN error' -: __ 2x  - etx __ 5x
'NaN error' -: __ 2r1 - etx __ 5r4


_1 = * {. __ 2x
_1 = * {. __ 2r3
1  = * {. _  2x
1  = * {. _  2r3

0 0 -: 0 * _  2x
0 0 -: 0 * __ 2x
0 0 -: 0 * _  2r5
0 0 -: 0 * __ 2r5


y=: }. 5x _ __
'NaN error' -: y    *. etx  12x
'NaN error' -: y    *. etx _12x
'NaN error' -:  12x *. etx y
'NaN error' -: _12x *. etx y

x=: {. _  5x
y=: {. __ 5x

'NaN error' -: x *. etx x
'NaN error' -: x *. etx y
'NaN error' -: y *. etx x
'NaN error' -: y *. etx y

y=: }. 5r1 _ __
'NaN error' -: y    *. etx  1r2
'NaN error' -: y    *. etx _1r2
'NaN error' -:  1r2 *. etx y
'NaN error' -: _1r2 *. etx y

x=: {. _  5r2
y=: {. __ 5r2

'NaN error' -: x *. etx x
'NaN error' -: x *. etx y
'NaN error' -: y *. etx x
'NaN error' -: y *. etx y

 5r0 -:  %0x
_5r0 -: -%0x
 5r0 -:  %0r1
_5r0 -: -%0r1

x=: {. _ 5x
(64=type y),  x  = y=:  5x % 0x
(64=type y),(-x) = y=: _5x % 0x
(64=type y), 0x  = y=:  5x % x
(64=type y), 0x  = y=: _5x % x

x=: {. _  5x
y=: {. __ 5x
'NaN error' -: x % etx x
'NaN error' -: x % etx y
'NaN error' -: y % etx x
'NaN error' -: y % etx y

x=: {. _  5r2
y=: {. __ 5r2
'NaN error' -: x % etx x
'NaN error' -: x % etx y
'NaN error' -: y % etx x
'NaN error' -: y % etx y


pinf = | pinf,ninf

y=: }. 2x _ __
(<'NaN error') = 5 _5x | etx&.>/ y
(2 2$5 _ __ _5x) -: y |/ 5 _5x
y -: 0x | y

y=: }. 2r3 _ __
(<'NaN error') = 5 _5r1 |etx&.>/ y
(2 2$5 _ __ _5r1) -: y |/ 5 _5r1
y -: 0r1 | y

_ 2x match !  _ 2x
_ 2x match !  _ 2r1
_ 2x match ! __ 2x
_ 2x match ! __ 2r1

( 64 -: type x), '_ __ 5'   -: ": x=:_ __ 5x
(128 -: type x), '_ __ 5r3' -: ": x=:_ __ 5r3

'domain error' -: p: etx {. _  12x
'domain error' -: p: etx {. __ 12x
'domain error' -: p: etx {. _  12r7
'domain error' -: p: etx {. __ 12r7

'domain error' -: q: etx {. _  12x
'domain error' -: q: etx {. __ 12x
'domain error' -: q: etx {. _  12r7
'domain error' -: q: etx {. __ 12r7


4!:55 ;:'match ninf pinf x xi y'


