NB. s: ------------------------------------------------------------------

x0=: ' Cogito , ergo sum.'
y0=: s: x0
y0 -: s: x0
y0 -: _1 s: x0
y0 -: _2 s: 1|.x0
y0 -: _4 s: >;:x0

z0=: ;:x0
(s:^:_1 y0) -: z0
(1 s: y0) -: ;'`',&.>z0
(2 s: y0) -: ;z0,&.>{.a.
(3 s: y0) -: z0,&>((>./ - ]) #&>z0)$&.>{.a.
(4 s: y0) -: >z0
(5 s: y0) -: z0
(, -: _1&s:@(1&s:)) y0
(, -: _2&s:@(2&s:)) y0
(] -: _3&s:@(3&s:)) y0
(] -: _4&s:@(4&s:)) y0
(] -: _5&s:@(5&s:)) y0
(] -: _6&s:@(6&s:)) y0

x1=: ":"0 ?133$12000
y1=: s: x1
y0 -: s: x0
y1 -: s: x1

z1=: (<"1 x1)-.&.>' '
(s:^:_1 y1) -: z1
(1 s: y1) -: ;'`',&.>z1
(2 s: y1) -: ;z1,&.>{.a.
(3 s: y1) -: z1,&>((>./ - ]) #&>z1)$&.>{.a.
(4 s: y1) -: x1
(5 s: y1) -: z1
(, -: _1&s:@(1&s:)) y1
(, -: _2&s:@(2&s:)) y1
(] -: _3&s:@(3&s:)) y1
(] -: _4&s:@(4&s:)) y1
(] -: _5&s:@(5&s:)) y1
(] -: _6&s:@(6&s:)) y1

x=: ;:'anaphoric boustrophedonic chthonic epigrammatic metonymic oxymoronic'
x2=: x,&.>/":&.>?100$150
y2=: s: x2
y0 -: s: x0
y1 -: s: x1
y2 -: s: x2

(s:^:_1 y2) -: x2
(1 s: y2) -: ;,'`',&.>x2
(2 s: y2) -: ;,x2,&.>{.a.
(3 s: y2) -: x2,&>((>./@, - ]) #&>x2)$&.>{.a.
(4 s: y2) -: >x2
(5 s: y2) -: x2
(, -: _1&s:@(1&s:)) y2
(, -: _2&s:@(2&s:)) y2
(] -: _3&s:@(3&s:)) y2
(] -: _4&s:@(4&s:)) y2
(] -: _5&s:@(5&s:)) y2
(] -: _6&s:@(6&s:)) y2

(,1) -: $ s: ' singleton'

(s: '') -: 0$ s: ' ab c'


NB. s: fill -------------------------------------------------------------

(,1) -: $ s: ' '
({.s: ' ') -: {: 5{.s: ' ab c'
0 = 6 s: s: ' '

(2 4$s: ' 0 1 2 3 4 5  ') -: > (s: ' 0 1 2 3'); s: ' 4 5'
(2 4$s: ' 0 1 2 3 4 5  ') -: s:"1 (<;._1 ' 0 1 2 3'),:<;._1 ' 4 5'


NB. 0 s: y --------------------------------------------------------------

0 s: 11

t=: 0 s: 0
4 -: type t
0 = #$t
1 <: t

t=: 0 s: 1
4 -: type t
0 = #$t
1 <: t

t=: 0 s: 2
4 -: type t
2 = #$t
(0 s: 0) <: #t
*./ (0<:i) *. i<: 0 s: 1 [ i=: (0 s: 0){.+/"1 (0 1){"1 t

t=: 0 s: 3
2 -: type t
1 = #$t
(0 s: 1) <: #t

t=: 0 s: 4
4 -: type t
1 = #$t
(0 s: 0) <: #t
*./ (_1=t) +. (0<:t)*.t<0 s: 0

t=: 0 s: 5
4 -: type t
0 = #$t
(0 <: t) *. (t < 0 s: 0)

t=: 0 s: 6
4 -: type t
0 = #$t
0 <: t

t=: 0 s: 7
4 -: type t
0 = #$t
0 <: t

t=: 0 s: 10
32 -: type t
1 = #$t
(,8) -: $t
t -: 0 s:&.>i.8

0 s: 11

t=: 0 s: 12
4 -: type t
1 = #$t
(,0 s: 0) -: $t
*./ 1 <: t
(0 s: 0) >: >./t


NB. 1 s: y --------------------------------------------------------------

x=: (;:'now is the time') ,&.>/ ":&.>?100$1e4
y=: s: x
(;,'`',&.>x) -: 1 s: y
(,y) -: _1 s: ;,'*',&.>x

t=: 1 s: y
2 -: type t
1 = #$t
'`' -: {.t

x=: <"1 ] 97+?31 13 4$26
x=: (?($x)$4){.&.>x
(, x{&.><a.) -: 5 s: _1 s: (; 32,&.>x){a.

(0$s: ' a') -: _1 s: ''


NB. 2 s: y --------------------------------------------------------------

x=: (;:'bou stro phe don ic') ,&.>/ ":&.>?2 25$1e4
y=: s: x
(;,x,&.>{.a.) -: 2 s: y
(,y) -: _2 s: ;,x,&.>'*'

t=: 2 s: y
2 -: type t
1 = #$t
({.a.) -: {:t

x=: <"1 ] 97+?31 13 4$26
x=: (?($x)$4){.&.>x
(, x{&.><a.) -: 5 s: _2 s: (; x,&.>0){a.

(0$s: ' a') -: _2 s: ''


NB. 3 s: y --------------------------------------------------------------

x=: (;:'el eem o syn ary') ,&.>/ ":&.>?2 3 4$1e4
y=: s: x
m=: >./,i=: #&>x
(>x,&.>(m-i)$&.>{.a.) -: 3 s: y
y -: _3 s: >x,&.>(m-i)$&.>{.a.

t=: 3 s: y
2 -: type t
(($x),m) = $t
(>x,&.>(m-i)$&.>{.a.) -: t

x=: <"1 ] 97+?31 13 4$26
x=: (?($x)$4){.&.>x
(x{&.><a.) -: 5 s: _3 s: (>x){a.


NB. 4 s: y --------------------------------------------------------------

x=: (;:'el eem o syn ary') ,&.>/ ":&.>?2 3 4$1e4
y=: s: x
(>x) -: 4 s: y
y -: _4 s: >x

t=: 4 s: y
2 -: type t
(($x),>./,#&>x) = $t
(>x) -: t

x=: <"1 ] 97+?31 13 4$26
x=: (?($x)$4){.&.>x
(x{&.><a.) -: 5 s: _4 s: >x{&.><a.


NB. 5 s: y --------------------------------------------------------------

x=: (;:'el eem o syn ary') ,&.>/ ":&.>?2 3 4$1e4
y=: s: x
x -: 5 s: y
y -: _5 s: x

t=: 5 s: y
32 -: type t
($y) = $t
x -: t


NB. 6 s: y --------------------------------------------------------------

x=: (;:'el eem o syn ary') ,&.>/ ":&.>?2 3 4$1e4
y=: s: x

t =: 6 s: y
4 -: type t
($y) = $t
y -: _6 s: t
*./ (0<:t) *.t< 0 s: 0


NB. 7 s: y --------------------------------------------------------------

x=: (;:'el eem o syn ary') ,&.>/ ":&.>?2 3 4$1e4
y=: s: x

t =: 7 s: y
4 -: type t
($y) = $t


NB. 10 s: y -------------------------------------------------------------

0 s: 11

x=: 0 s: 10

'domain error' -: 10 s: etx ($x)$0 1
'domain error' -: 10 s: etx ($x)$1 2 3 4
'domain error' -: 10 s: etx ($x)$1 2 3.4
'domain error' -: 10 s: etx ($x)$1 2 3j4
'domain error' -: 10 s: etx ($x)$1 2 3r4
'domain error' -: 10 s: etx ($x)$1 2 3 4x
'domain error' -: 10 s: etx ($x)$' 2 3 4'

'domain error' -: 10 s: etx }.x
'domain error' -: 10 s: etx }:x
'domain error' -: 10 s: etx ,:x
'domain error' -: 10 s: etx {.x

'domain error' -: 10 s: etx x 0}~< 'a'         
'domain error' -: 10 s: etx x 0}~< 100002.4 
'domain error' -: 10 s: etx x 0}~< 100002j4
'domain error' -: 10 s: etx x 0}~< 100002r4
'domain error' -: 10 s: etx x 0}~< ({.x),&.>0
'domain error' -: 10 s: etx x 0}~< 1+#>2{x
'domain error' -: 10 s: etx x 0}~< 1+#>4{x
'domain error' -: 10 s: etx x 0}~< _1e6

'domain error' -: 10 s: etx x 1}~< 'a'         
'domain error' -: 10 s: etx x 1}~< 100002.4    
'domain error' -: 10 s: etx x 1}~< 100002j4    
'domain error' -: 10 s: etx x 1}~< 100002r4    
'domain error' -: 10 s: etx x 1}~< (1{x),&.>0
'domain error' -: 10 s: etx x 1}~< 1+#>3{x   

'domain error' -: 10 s: etx x 3}~< 1234
'domain error' -: 10 s: etx x 3}~< ,:>2{x
'domain error' -: 10 s: etx x 3}~< ($>2{x)$'2'
'domain error' -: 10 s: etx x 3}~< ($>2{x)$2.3
'domain error' -: 10 s: etx x 3}~< ($>2{x)$2j3
'domain error' -: 10 s: etx x 3}~< ($>2{x)$2r3
'domain error' -: 10 s: etx x 3}~< }:"1 >2{x
'domain error' -: 10 s: etx x 3}~< (>2{x),.0


NB. s: errors -----------------------------------------------------------

'domain error' -: s: etx 0 1
'domain error' -: s: etx 0 1 2 3
'domain error' -: s: etx 0 1 2.3
'domain error' -: s: etx 0 1 2j3
'domain error' -: s: etx 0 1 2 3x
'domain error' -: s: etx 0 1 2r3

'domain error' -: s: etx 'abc';0 1
'domain error' -: s: etx 'abc';0 1 2 3
'domain error' -: s: etx 'abc';0 1 2.3
'domain error' -: s: etx 'abc';0 1 2j3
'domain error' -: s: etx 'abc';0 1 2 3x
'domain error' -: s: etx 'abc';0 1 2r3

'domain error' -: 77  s: etx s: ' a b c'
'domain error' -: _39 s: etx s: ' a b c'
'domain error' -: 'a' s: etx s: ' a b c'
'domain error' -: 2.3 s: etx s: ' a b c'
'domain error' -: 2j3 s: etx s: ' a b c'
'domain error' -: 2r3 s: etx s: ' a b c'
'domain error' -: (<2)s: etx s: ' a b c'

'domain error' -: 1   s: etx ' a bc d'
'domain error' -: 2   s: etx ' a bc d'
'domain error' -: 3   s: etx ' a bc d'
'domain error' -: 4   s: etx ' a bc d'
'domain error' -: 5   s: etx ' a bc d'

'domain error' -:  0  s: etx 413
'domain error' -:  0  s: etx 4.3
'domain error' -:  0  s: etx 4j3
'domain error' -:  0  s: etx 4r3
'domain error' -:  0  s: etx 'abc'
'domain error' -: _9  s: etx 0

'domain error' -: ex ' 0&s: b. _1'
'domain error' -: ex ' 7&s: b. _1'
'domain error' -: ex '_7&s: b. _1'

'rank error'   -:     s: etx 'abc';2 3$'foobar'
'rank error'   -: 1 2 s: etx s: ' a bc'


4!:55 ;:'i m t x x0 x1 x2 y y0 y1 y2 z0 z1'


