prolog './gxco2.ijs'
NB. extended precision integers -----------------------------------------

NB. create test data

x1=: (1-1e4)+10 11 ?@$ 2e4-1
y1=: (1-1e4)+10 11 ?@$ 2e4-1
x2=: (1-1e8)+10 11 ?@$ 2e8-1
y2=: (1-1e8)+10 11 ?@$ 2e8-1
x3=: (1-1e9)+10 11 ?@$ 2e9-1
y3=: (1-1e9)+10 11 ?@$ 2e9-1


NB. o. ------------------------------------------------------------------

0x = o. 0x
0x = <.@o. 0x
0x = >.@o. 0x

0 -: o. x: 0
0 -: <.@o. x: 0
0 -: >.@o. x: 0

(<.!.0@o. -: <.@o.@x:) y1
(<.!.0@o. -: <.@o.@x:) y2
(<.!.0@o. -: <.@o.@x:) y3

(>.!.0@o. -: >.@o.@x:) y1
(>.!.0@o. -: >.@o.@x:) y2
(>.!.0@o. -: >.@o.@x:) y3
   
31415926535897932384626433832795028841971x = <.@o. 10^40x
31415926535897932384626433832795028841972x = >.@o. 10^40x

x=: '3 14159 26535 89793 23846 26433 83279 50288 41971 '
y=: '3 ', , _5 (,&' ')\ }. ": <.@o. 10x^40
x -: y

f=: }:@":@(<.@o.)
g=: (<.&# {. [) -: (<.&# {. ])

(10x^ 50) g&f 10x^(QKTEST{300 30)
(10x^100) g&f 10x^(QKTEST{300 30)
(10x^150) g&f 10x^(QKTEST{300 30)
(10x^200) g&f 10x^(QKTEST{300 30)

(o.  1) -: o. 1x
(o. _2) -: o. _2x

( 0 o. 5) -:  0 o. x: 5
( 1 o. 5) -:  1 o. x: 5
( 2 o. 5) -:  2 o. x: 5
( 3 o. 5) -:  3 o. x: 5
( 4 o. 5) -:  4 o. x: 5
( 5 o. 5) -:  5 o. x: 5
( 6 o. 5) -:  6 o. x: 5
( 7 o. 5) -:  7 o. x: 5
(_1 o. 5) -: _1 o. x: 5
(_2 o. 5) -: _2 o. x: 5
(_3 o. 5) -: _3 o. x: 5
(_4 o. 5) -: _4 o. x: 5
(_5 o. 5) -: _5 o. x: 5
(_6 o. 5) -: _6 o. x: 5
(_7 o. 5) -: _7 o. x: 5





epilog''

