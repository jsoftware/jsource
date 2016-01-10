NB. +.y  ----------------------------------------------------------------

f =. 9 11&o."0"_

(f -: +.) r.?10 20$1000
(f -: +.) 0.1*_500+?200$1000

a -: [&.+. a=.0.1*_50j_50+j.&?~100$100

0 0   -: +. 0
1 0   -: +. 1
3 0   -: +. 3
3.4 0 -: +. 3.4
3 4   -: +. 3j4

8 -: 3!:0 +. r.?10$1000

'domain error' -: +. etx 'abc'
'domain error' -: +. etx <'abc'


NB. x+.y  ---------------------------------------------------------------

0 1 1 1 -: 0 0 1 1 +. 0 1 0 1
(12$6 1 2 3 2 1) -: 6 +. i.12
1j1 -: 5j11 +. 3j7

x=: j./ _50   + 2 10000 ?@$ 100
y=: j./ _50   + 2 10000 ?@$ 100
(x+.y) -: y+.x
x=: j./ _5000 + 2 10000 ?@$ 10000
y=: j./ _5000 + 2 10000 ?@$ 10000
(x+.y) -: y+.x

'domain error' -: 'abc' +. etx 4
'domain error' -: 'abc' +.~etx 4
'domain error' -: 4     +. etx <'abc'
'domain error' -: 4     +.~etx <'abc'
'domain error' -: 'j'   +. etx <'abc'
'domain error' -: 'j'   +.~etx <'abc'

'length error' -: 0 1   +. etx 0 1 0
'length error' -: 0 1   +.~etx 5 6 7
'length error' -: 0 1   +. etx i.5 6
'length error' -: 0 1   +.~etx i.5 6
'length error' -: 0 1 1 +. etx ?4 3$2
'length error' -: 3 4 5 +.~etx ?4 3$2


NB. x+.y, GCD as a linear combination of x and y ------------------------

gcd =: 3 : 0    NB. (x+.y)=+/(x,y)*x gcd y
   :
 m=.x,1 0
 n=.y,0 1
 while. {.m do. n=.t [ m=.n-m*<.n %&{. t=.m end.
 }.n
)

init =. , ,. =@i.@2:
iter =. {: ,: {. - {: * <.@%&{./
gcd1 =. (}.@{.) @ (iter^:(*@{.@{:)^:_) @ init

(+./ -: [ +/ .* gcd /)"1 x=.?3 10 2$100
(+./ -: [ +/ .* gcd1/)"1 x

4!:55 ;:'a f gcd gcd1 init iter x y'


