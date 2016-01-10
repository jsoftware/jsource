NB. q: ------------------------------------------------------------------

f =: = */
g =: (,. -: q:)@~.@]

'' -: q: 1   NB. Jeffrey Shallit
(i.($x),0) -: q: x=:$0
(i.($x),0) -: q: x=:i.?~5

x (f *. g) q: x=:2
x (f *. g) q: x=:*/2#2 3 5 7 11 13
x (f *. g) q: x=:2^30

*./ x (f *. g)&> q:&.> x=:2+i.1000
*./ x (f *. g)&> q:&.> x=:2+?40$<._1+2^31
*./ x (f *. g)&> q:&.> x=:y^<.2.147e9^.~ y=:p:i.15
*./ x (f *. g)&> q:&.> x=:*/\p: i.9
*./ x (f *. g)&> q:&.> x=:(<._1+2^31)-i.40
*./ x (f *. g)&> q:&.> x=:p: 1e6*>:i.105

x -: */"1 [ 1>.q: x=:1+?4 5$1e7
x -: */"1 [ 1>.q:"0 x
(q: x) -: >q:&.> x

n = */ q: n=: */ 3,29$2

'domain error' -: q: etx 0
'domain error' -: q: etx '1234'
'domain error' -: q: etx _19
'domain error' -: q: etx 3.4
'domain error' -: q: etx 3j4
'domain error' -: q: etx <12345


NB. q: model ------------------------------------------------------------

max   =: 1&>. @ (>./) @ ,
pn    =: <. @ (1.3&*) @ (% 1&>.@^.)
primes=: p: @ i. @ pn @ %: @ max
pa    =: [ #~ 0: = |

qa    =: 4 : 0
 z=.p=.x pa r=.y
 while. #p do. z=.z,p=.p pa r=.<.r%*/p end.
 /:~z,r-.1
)

qco   =: primes qa"1 0 ]

(q: -: qco) 1
(q: -: qco) >:i.10 5
(q: -: qco) x=:1+?20$2e9


NB. q: dyad -------------------------------------------------------------

2 0 2 1    -: _ q: 700
2 0 2 1 0  -: 5 q: 700

''   -: _ q: 1
''   -: 0 q: 1
(,0) -: 1 q: 1
0 0  -: 2 q: 1

(-: [&.(40&q:))"0 x=:1+?2 10$100

x -: (p:i.{:$y) */ .^"1 y=:_  q: x=:>:i.10 10
x -: (p:i.{:$y) */ .^"1 y=:50 q: x
x -: (p:i.{:$y) */ .^"1 y=:_  q: x=:>:?10 2 7$10000
x -: (p:i.{:$y) */ .^"1 y=:(1+p:^:_1 >./x) q: x=:>:?200$10000

(_ q: x) (<\@[ -: >:@i.@#@[ q:&.> ]) x=:*/>:?4$215
(_ q: x) (<\@[ -: >:@i.@#@[ q:&.> ]) x=:*/>:?4$215

64 -: 3!:0 q: !100x
64 -: 3!:0 ] _ q: !100x

300 3 -: 2 q: 27*2^300x
(i.0) -: 0 q: 1+2^1000x


NB. q: ------------------------------------------------------------------

phi =: * -.@%@~.&.q:    NB. Euler's totient function

1 = phi 1
(p-1) -: phi"0 p=:p: i.20
x=:b#x [ y=:b#y [ b=:1=x+.y=:?100$2e4 [ x=:>:?100$1e5 
(phi"0 x*y) -: x *&phi"0 y

f =: 1: #. 1: = (+.i.)
*./ (f -: phi)"0 x=:>:i.200
*./ (f -: phi)"0 x=:>:?10$1e4

odometer =: #: i.@(*/)
divisors =: /:~ @ (~. */ .^"1 odometer@:>:@(#/.~)) @ q:

t2 =: [ -: +/@phi@divisors
*./ t2"0 x=:>:i.200
*./ t2"0 x=:>:?10$2e9

d0=: [: +/ 0: = >:@i. | ]   NB. number of divisors of n
d1=: >:@#/.~ &. q: 
d2=: >:@#;.1 @ ~: &. q:  
d3=: */ @: >: @: (_&q:)

(d0 -: d1)"0 >:i.10 20
(d0 -: d1)"0 x=:>:?10 10$1e4
(d1 -: d2)"0 >:i.10 20
(d1 -: d2)"0 x=:>:?10 10$1e6
(d2 -: d3)"0 >:i.10 20
(d2 -: d3)"0 x=:>:?10 10$1e6

39001250856960000x -: d3 !100x

'domain error' -: 'a' q: etx 123
'domain error' -: 3j5 q: etx 123
'domain error' -: (<3)q: etx 123

'domain error' -: 1   q: etx 'a'
'domain error' -: 1   q: etx 0
'domain error' -: 1   q: etx _35
'domain error' -: 1   q: etx 3.5
'domain error' -: 1   q: etx 3j5
'domain error' -: 1   q: etx <35


NB. q: on large integers ------------------------------------------------

f=: 3 : 0
 v=: q: y
 assert. y=*/v
 assert. v -: /:~ v
 assert. (, -: q:)"0 v
 1
)

f x=: */ x: p: 10^i.5
f x=: */ x: p: 10^i.6
f x=: */ x: p: 10^i.7
f x=: */ x: p: 10^i.8
f x=: */ x: p: 1e7,10 ?@$ 1228
f x=: */ x: p: 2 ?@$ 1e8
f x=: 10 #. 18 ?@$ 10x
f x=: */ p: 1e8+0 1 2x

f x=: 1.3e13
f x=: 252097800629


4!:55 ;:'b d0 d1 d2 d3 divisors f g max odometer p pa phi pn primes'
4!:55 ;:'qa qco t2 x y '


