NB. p: dyad -------------------------------------------------------------

isprime=: (1 = #@q:) :: 0: "0

( 1&p: -: isprime  ) x=: 1+2*100 ?@$ 2e5
( 0&p: -: -.@(1&p:)) x
(_1&p: -: p:^:_1   ) x

x=: <._1+2^31
(1&p: -: isprime) x-  i.50
(1&p: -: isprime) x-  i.50x
(1&p: -: isprime) x-2*i.50
(1&p: -: isprime) x-2*i.50x
(1&p: -: isprime) x+  i:50
(1&p: -: isprime) x+2*i:50

1 p: x=: p: 10 ?@$ 1e7
1 p: x=: {:@q: 10 #. 6 18 ?@$ 10x

1 0 0 -: 1 p: 2x 1r2 1r3

x=: i: 30
b=: x e. i.&.(p:^:_1) 30
b -: 1 p: x=: i: 30
b -: 1 p: x: x
b -: 1 p: x+-~0.5
b -: 1 p: x+-~0j5
b -: 1 p: x+-~1r5

f1=: 3 : 0
 c=. 4 p: y
 assert. c -: 4 p: x: y
 assert. 1 p: c
 assert. 0 p: ; (1+y) +&.> i.&.>c-1+y
 1
)

2 3      -: 4 p: __ 2
2 3      -: 4 p: __ 2x
(($x)$2) -: 4 p: x=: 100 ?@$ 2
(p: i.x) -: 4&p:^:(<x) 2 [ x=: ?200

f1 x=:    100 ?@$ 1e8
f1 x=: p: 100 ?@$ 1e6

2147483659x -: 4 p: 2147483647
0 = 1 p: 2147483647x+1+i.11
   
2147483629 -: _4 p: 2147483647
0 = 1 p: 2147483629x+1+i.17

f2=: 3 : 0
 c=. _4 p: y
 assert. c -: _4 p: x: y
 assert. 1 p: c
 assert. 0 p: ; (1+c) +&.> i.&.>y-1+c
 1
)

f2 x=:    3+100 ?@$ 1e8
f2 x=: p: 1+100 ?@$ 1e6

(_4 p: 2.1) -: _4 p: 3

tot=: 3 : '+/1=y+.i.y' " 0
0 1 1 = 5 p: 0 1 2
(tot -: 5&p:) i.4 5
(tot -: 5&p:) i.4 5x
(tot -: 5&p:) x=: 100 ?@$ 1000

tot=: 3 : 0 " 0
 'p e'=. __ q: y
 */(p-1)*p^e-1
)

(tot -: 5&p:) x=: 100 ?@$ 2e9
(tot -: 5&p:) x=: 1 + */"1 ] 5 10 ?@$ 100x

x=: 100
y=: +/1000$0.1
c=: _4 _1 0 1 2 3 4 5
(c p:&.>x) -: c p:&.>y

'4&p:'               -: _4&p: b. _1
'p:'                 -: _1&p: b. _1
'*/@(^/)"2 :.(2&p:)' -:  2&p: b. _1
'*/'                 -:  3&p: b. _1
'_4&p:'              -:  4&p: b. _1

'domain error' -: 12   p: etx 4 5 6
'domain error' -: _2   p: etx 4 5 6
'domain error' -: 3j4  p: etx 4 5 6
'domain error' -: 1r2  p: etx 4 5 6
'domain error' -: '1'  p: etx 4 5 6
'domain error' -: (<1) p: etx 4 5 6

'domain error' -: 1    p: etx '123'
'domain error' -: 1    p: etx u: '123'
'domain error' -: 1    p: etx 1;2;3

'domain error' -: 1    p: etx 13x _
'domain error' -: 1    p: etx 13x __
'domain error' -: 1    p: etx 1.3 _
'domain error' -: 1    p: etx 1.3 __
'domain error' -: 1    p: etx 1j3 _
'domain error' -: 1    p: etx 1j3 __

'domain error' -: 4    p: etx 2 3j4
'domain error' -: 4    p: etx '234'
'domain error' -: 4    p: etx 2;34
'domain error' -: 4    p: etx u: 2 3 4

'domain error' -: _4   p: etx 4 3 2 
'domain error' -: _4   p: etx 4 3 2x
'domain error' -: _4   p: etx 1.9 12 
'domain error' -: _4   p: etx _5 12 
'domain error' -: _4   p: etx _5 12x
'domain error' -: _4   p: etx 0 1 0 0 
'domain error' -: _4   p: etx '456' 
'domain error' -: _4   p: etx 1 2 3j4 
'domain error' -: _4   p: etx u: 2 3 4 

'domain error' -: 5    p: etx 2 3 4 _5
'domain error' -: 5    p: etx 2 3 4 _5x

'rank error'   -: 0 1  p: etx 4 5 6


4!:55 ;:'b c f1 f2 isprime tot x y'


