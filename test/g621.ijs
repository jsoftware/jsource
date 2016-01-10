NB. @. ------------------------------------------------------------------

f=: 1:`(* $:@<:) @. *
(!i.10) -: f"0 i.10

do   =: 0!:100
ar   =: 5!:1
test =: 2 : 0
 do 'f9=.',x
 do 'g9=.',y
 =&ar/;:'f9 g9'
)

ifopen =: (-:>) @ {. @ [
from1  =: >@{ ` { @. (1&<@#@[)
from   =: ($:&.> <) ` from1 @. ifopen

g =: ;: '&+&.+&:^.@'
'(++^.)+^.' test '((1 3 5;3;5) from g)`:6'
'(++^.)+^.' test 'g@.(1 3 5;3;5)'

'index error' -: +`-`* @. ] etx 3
'index error' -: +`-`* @. ] etx _4

'rank error'  -: +`-`* @. ] etx i.3
'rank error'  -: +`-`* @. ] etx i.1 1

ack=: c1`c1`c2`c3 @. (#.@(,&*)) " 0
c1 =: >:@]                     NB. 1+y
c2 =: <:@[ ack 1:              NB. (x-1) ack 1
c3 =: <:@[ ack [ ack <:@]      NB. (x-1) ack x ack y-1

(0&ack -:  >:&.(3&+)) x=: ?10$20
(1&ack -: 2&+&.(3&+)) x=: ?10$20
(2&ack -: 2&*&.(3&+)) x=: ?10$20
(3&ack -: 2&^&.(3&+)) 3

(o.  x) -: o.`*`(*:@+:)@.0 x=: 1 2 3
(*   x) -: o.`*`(*:@+:)@.1 x=: 1 2 3
(*:+:x) -: o.`*`(*:@+:)@.2 x=: 1 2 3


NB. g @. v " 0 ----------------------------------------------------------

NB. Example: CDF of standard Normal N(0,1) by J.E.H. Shaw
NB.
NB. n01pdf v    probability density function for N(0,1)
NB. erf v       error function
NB. n01cdfh v   cumulative distribution function for N(0,1) using H.
NB. n01cdfr v   ratio used for CDF in tails of N(0,1)
NB. n01cdfn v   more accurate than n01cdfh in lower tail (say y. < _6)
NB. n01cdfp v   more accurate than n01cdfh in upper tail (say y. > 6)
NB. n01cdfa v   N(0,1) cdf using @.
NB. n01cdf v    N(0,1) cdf using agenda 

agenda=: 2 : 0
 j=. ~.k=. v y
 b=. k (('@:' <@(,&<) '<'&;)"0 j{m)/. y
 (/:/:k) { ; b /: j
)

n01pdf =: ([: ^ _0.5"_ * *:) % (%: 2p1)"_ 
erf    =: (*&(%:4p_1) % ^@:*:) * [: 1 H. 1.5 *:
n01cdfh=: [: -: 1: + [: erf %&(%:2) 
n01cdfr=: n01pdf % ] + 1: % ] + 2: % ] + 3: % ] + 4: % ] + 4.5&%
n01cdfn=: [: - n01cdfr
n01cdfp=: 1: - n01cdfr
n01cdfa=: n01cdfn`n01cdfh`n01cdfp @. (>&_6 + >&6) " 0
n01cdf =: n01cdfn`n01cdfh`n01cdfp agenda (>&_6 + >&6)

(n01cdf -: n01cdfa) x=: _10 + 0.01 * ? 10000 $ 1000

f0 =: -`*: @. (1: = 2&|) " 0
f1 =: 3 : ' (b**:y)+(-.b=.2|y)*-y'
(f0 -: f1) x=: ?13 17 19$1000

square=: 3 : '*: y'

f0=: 1: ` *:     @. (1: = 2&|) " 0
f1=: 1: ` square @. (1: = 2&|) " 0
(f0 -: f1) x=: ?13 17 19$1000

f0=: 1: ` - ` *:     @. (3&|) " 0
f1=: 1: ` - ` square @. (3&|) " 0
(f0 -: f1) x=: ?13 17 19$1000


NB. @. -- all size x partitions of y ------------------------------------

start =: +/@{. >:@i.@<.@%&>: {:@$
mask  =: start <:/ {."1 <. -.@(-/)@(_2&{.)"1
pfx   =: +/"1 # >:@i.@#
ind   =: , # */@$ $ i.@{:@$
decr  =: (>:@(-/)@(_1 0&{) _1} ])"1
form  =: pfx@[ decr@,. ind@[ { ]
recur =: (mask form ])@(part&<:)

test  =: 1&<@[ *. <
basis =: (0&<@] , [) $ (1&=@[ 1&>.@* ])
part  =: basis`recur@.test

f=: 4 : 0
 t=.x part y
 assert. x={:$t
 assert. (i.#t)-:/:t
 assert. (0=x)+.y=+/"1 t
 assert. (/:"1 t) *./ .=i.x
 1
)

f/+/\?5 10
0 f 0
0 f 5
5 f 5


4!:55 ;:'ack agenda ar c1 c2 c3 basis decr do erf f f0 f1 form from from1 '
4!:55 ;:'g ifopen ind mask '
4!:55 ;:'n01cdf n01cdfa n01cdfh n01cdfn n01cdfp n01cdfr n01pdf '
4!:55 ;:'part pfx recur square start t test x z '


