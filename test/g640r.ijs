NB. ? different RNGs ----------------------------------------------------

NRNG=: 5  NB. number of RNGs

'length error' -: 9!:42 etx 4 5
'rank error'   -: 9!:42 etx 0
'rank error'   -: 9!:42 etx i.2 3

'domain error' -: 9!:43 etx 'a'
'domain error' -: 9!:43 etx <3
'domain error' -: 9!:43 etx 3.4
'domain error' -: 9!:43 etx 3j4
'domain error' -: 9!:43 etx _1

'rank error'   -: 9!:43 etx ,1
'rank error'   -: 9!:43 etx 3 4

'length error' -: 9!:44 etx 4 5
'rank error'   -: 9!:44 etx 0
'rank error'   -: 9!:44 etx i.2 3


NB. seed

test0=: 3 : 0
     j=: 9!:42 ''
 yy=: y
 9!:43 yy
 p2=: 9!:44 '' [ p1=: 1000 ?@$ 0 [ p0=: 9!:44 '' [ 9!:1 ]1000003
 q2=: 9!:44 '' [ q1=: 1000 ?@$ 0 [ q0=: 9!:44 '' [ 9!:1 ]1000003
 assert. p0 -: q0
 assert. p1 -: q1
 assert. p2 -: q2
     9!:43 j
 1
)

test0"0 i.NRNG

NB. state

test1=: 3 : 0
     j=: 9!:42 ''
 yarg=: y
 9!:43 y
 1000 ?@$ 0
 t=: 9!:44 '' [ xx=: 1000 ?@$ 1e6 [ s=: 9!:44 '' 
 u=: 9!:44 '' [ yy=: 1000 ?@$ 1e6 [ 9!:45 s
 assert. xx -: yy
 assert. t  -: u
     9!:43 j
 1
)

test1"0 i.NRNG


NB. preserving state under switching of RNGs

test2=: 4 : 0
     j=: 9!:42 ''
 xx=: x
 yy=: y
 2003 ?@$ 1e6 [ 9!:1 ]12345 [ 9!:43 x
 p0=:     17 ?@$ 1e6
 p0=: p0, 19 ?@$ 1e6
 p1=: 9!:44 ''
 2003 ?@$ 1e6 [ 9!:1 ]12345 [ 9!:43 x
 q0=:     17 ?@$ 1e6
 9!:43 y
 2003 ?@$ 1e6
 9!:43 x
 q0=: q0, 19 ?@$ 1e6
 q1=: 9!:44 ''
 assert. p0 -: q0
 assert. p1 -: q1
     9!:43 j
 1
)

test2/"1 (~:/"1 # ]) ,/,"0/~ i.NRNG

NB. preserving state under switching of RNGs

test2a=: 3 : 0
     j=: 9!:42 ''
 yy=: y
 2003 ?@$ 1e6 [ 9!:1 ]12345 [ 9!:43 y
 p0=:     17 ?@$ 1e6
 p0=: p0, 19 ?@$ 1e6
 p1=: 9!:44 ''
 2003 ?@$ 1e6 [ 9!:1 ]12345 [ 9!:43 y
 q0=:     17 ?@$ 1e6
 2003 ?.@$ 1e6
 q0=: q0, 19 ?@$ 1e6
 q1=: 9!:44 ''
 assert. p0 -: q0
 assert. p1 -: q1
     9!:43 j
 1
)

test2a"0 i.NRNG


4!:55 ;:'j NRNG p0 p1 p2 q0 q1 q2 s t '
4!:55 ;:'test0 test1 test2 test2a u xx yarg yy'


