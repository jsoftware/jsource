NB. ?y ------------------------------------------------------------------

tick =: [ <.@%~ (* 3 : 'qrl=:(<:2^31)|(7^5)*qrl')@]
roll =: (<:2^31)&tick"0

NB. qrl  =: 9!:0 ''
NB. (? -: roll) 1000000
NB. (? -: roll) 2 3 4 $987654321
NB. qrl -: 9!:0 ''

test=: 3 : 0
 r=: ?y
 assert. ($r) -: $y
 assert. 0<:r
 assert. r<y
 1
)

test 1+?2 3 4$100
test 1+?100
test 2 0 3$100

0 -: ?1
1 -: 3!:0 ?2 3 4 5$2

(2^_53) = +./ ? 5e3 $ 0
1 = +./@:?@:(5e3&$)"0 ] 10^1 2 6 8 9
1 = +./@:?@:(5e3&$)"0 ]  2 ^1 3 5 7 8 13 23 30
1 = +./@:?@:(5e3&$)"0 ]  IF64#<.10^10 11 12 13 18
1 = +./@:?@:(5e3&$)"0 ]  IF64#<.2 ^32 33 34 43 47 53 62

zz  =: 3.90  NB. Z(3.90)=0.99995
mean=: +/ % #
var =: <:@# %~ +/@:*:@:(- mean)

testmean=: 4 : 0
 m=: x
 t=: y
 c=: zz * (var t)%&%:#t
 d=: (mean t) - |-:<:m
 assert. c > | d 
 1
)

test1=: 3 : 0
 y testmean 1e4 ?@$ y
 y testmean ? 1e4 $ y
)

test1     0
test1"0 ]   10^1 2 6 7 8 9
test1"0 ] 2*10^1 2 6 7 8 9
test1"0 ]    2^1 2 3 4 5 28 29 30 
test1"0 ]  1+2^1 2 3 4 5 28 29 30 
test1"0 ] IF64#<.  10^10 11 17 18
test1"0 ] IF64#<.4*10^10 11 17 18
test1"0 ] IF64#<.   2^3 7 9 32 33 47 53 62
test1"0 ] IF64#<. 1+2^3 7 9 32 33 47 53 62
test1"0 ] IF64#<._1+2^3 7 9 32 33 47 53 62

test1"0 x: 5 555 55555

64 = 3!:0 ?10$20x

'domain error' -: ? etx 'abc'
'domain error' -: ? etx 2 3 _4
'domain error' -: ? etx 2 3.4 5
'domain error' -: ? etx 2 3j4 5
'domain error' -: ? etx 2 3;4 5

'domain error' -: ?@(*/) etx 'abc'


NB. x?y -----------------------------------------------------------------

bigdeal=: 4 : 0  
 t=. 0 $ v=. y $~ <.1.11*x
 while. x > #t do. t=. ~. roll v end.
 x {. t
)

roll =: ?.

rix  =: i.@[ ([ ,. [ + roll@:-~) ]
deal1=: [ {. <@~."1@|.@rix C. i.@-@]
deal =: deal1 ` bigdeal @. (< 0.01&*)   NB. pre J 5.03

deal =: [ {. <@~."1@|.@rix C. i.@]      NB. J 5.03 or later

10 (?. -: deal) 100
10 (?. -: deal) 1000
10 (?. -: deal) 10000

NB. qrl =: 9!:0 ''
NB. (? -: deal)~ 100
NB. 300 (? -: deal) 1000
NB. a =. 10 20 30
NB. b =. +: a
NB. (a?b) -: (a deal b)
NB. qrl -: 9!:0 ''

test1=: 4 : 0
 r=. x?y
 assert. ($r) -: ,x
 assert. 0<:r
 assert. r<y
 assert. r -: ~.r
 1
)

10 test1 15
10 test1 30
10 test1 45
4  test1 4+?100
4  test1 4+?10000
1  test1 1+?100
1  test1 1+?10000
0  test1 ?100
0  test1 ?10000
0  test1 0

10 test1 IF64{1e9 1e18
10 test1 IF64{<:2^31 63
10 test1 IF64{5+2^10 31

100 test1 100

'' -: 0?0
'' -: 0?9

64 = 3!:0 ]20x?100
64 = 3!:0 ]20 ?100x
64 = 3!:0 ]20x?100x

'domain error' -: 2 ? etx 1                           
'domain error' -: 4 ? etx 0                           
'domain error' -: 4 ? etx 3                           
'domain error' -: 3 ? etx 'abc'                       
'domain error' -: 3 ?~etx 'abc'                       
'domain error' -: 1 ? etx 2 3 _4                      
'domain error' -: 5 ?~etx 2 3 _4                      
'domain error' -: 2 ? etx 2 3.4 5                     
'domain error' -: 5 ?~etx 2 3.4 5                     
'domain error' -: 2 ? etx 2 3j4 5                     
'domain error' -: 5 ?~etx 2 3j4 5                     
'domain error' -: 2 ? etx 2 3;4 5                     
'domain error' -: 2 ?~etx 2 3;4 5

'domain error' -: ?~@(*/) etx 'abc'

'length error' -: 2 3 ? etx 4 5 6


NB. Model of gb_flip ----------------------------------------------------

mod_diff=: (<._1+2^31)&(17 b.)@:- 

gb_next_rand=: 3 : 0
 if. 0 > gb_i do. gb_i=: <: # gb_A=: gb_flip_cycle gb_A end.
 (gb_i=: <:gb_i) ] gb_i{gb_A
)

gb_flip_cycle=: 3 : 0
 +/ (0 1 2=/<.(i.55)%24) * (mod_diff 31&|.)^:1 2 3~y
)

gb_init_rand1=: 3 : 0
 prev=. s=. y mod_diff 0  NB. strip off the sign
 seed=. (31$2) #: s
 next=. 1
 z=. ''
 for. i.54 do.
  z=. z,next
  t=. next
  seed=. _1 |. seed        NB. cyclic shift right 1
  next=. (prev mod_diff next) mod_diff #. seed
  prev=. t
 end.
 s,~z /: 55|+/\54$21
)

gb_init_rand=: 3 : 0
 0 0 $ gb_i=: _2 + # gb_A=: gb_flip_cycle^:5 gb_init_rand1 y
)

roll=: 3 : 0
 assert. 0~:y
 z=. i.0
 for_t. y (] - |) <.2^31 do.
  while. t<:r=. gb_next_rand '' do. end.
  z=. z,r
 end.
 ($y)$y|z
)

roll0=: 3 : 0
 assert. 0=y
 (2^53) %~ (2^31) #. |."1 (0 2^22)|"1 gb_next_rand"0 (($y),2)$0
)

gb_init_rand _314159
119318998 -: gb_next_rand ''
1 [ gb_next_rand^:133 ''
748103812 -: roll 16#.8#5

gb_init_rand 7^5
IF64 +. (?. -: roll) 10^i.7

NB. gb_init_rand 7^5
NB. (?. -: roll0) 2 5$0


NB. Model of lcg --------------------------------------------------------

lcg=: 3 : '2147483647&|@(16807&*)^:y 1x'  NB. linear congruence generator

test_lcg=: 3 : 0
 t=: x:^:_1 lcg 1+i.1597
 assert. ( 5{.t) -: 16807 282475249 1622650073 984943658 1144108930
 assert. (_5{.t) -: 1476003502 1607251617 2028614953 1481135299 1958017916
 1
)

test_lcg 1


NB. Model of dx-1597-4d -------------------------------------------------

B=: 1073741362x
M=: <._1+2^31

r1597=: lcg 1+i.1597

dx=: 3 : 0
 z=. M|B*+/r1597{~532*i.4
 r1597=: (}.r1597),z
 z
)

dx1=: 3 : 0
 z=. <. M|29746*M|36097*+/r1597{~532*i.4  
 r1597=: (}.r1597),z
 z
)

test_dx=: 3 : 0
 r1597=: lcg 1+i.1597
 t=: dx^:(1+i.5000) 0
 assert. ((  i.5){t) -:  221240004 2109349384 527768079 238300266 1495348915 
 assert. ((5+i.5){t) -: 1589596592 1437773979 813027151 401290350 1732813760
 r1597=: x:^:_1 lcg 1+i.1597
 x=: dx1^:(1+i.5000) 0
 assert. t -: x
 if. -.IF64 do.
  i=: 9!:42 ''
  9!:43 ]3
  9!:1 ]1
  x=: 128!:4 ]5000
  assert. t -: x
  9!:43 ]i
 end.
 1
)

test_dx 1


4!:55 ;:'a B b bigdeal c d deal deal1 dx dx1'
4!:55 ;:'gb_A gb_flip_cycle gb_i gb_init_rand gb_init_rand1 gb_next_rand'
4!:55 ;:'i lcg M m mean mod_diff n qrl r r1597 rix roll roll0 seed'
4!:55 ;:'t test test_dx test_lcg test1 testmean tick var x zz'


