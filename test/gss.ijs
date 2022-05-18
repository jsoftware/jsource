prolog './gss.ijs'
NB. Speedy Singletons ------------------------------------------------------------------

randuni''

NB. Tests of local-name lookup

NB. Local name assigned to global name
tolower =: 3 : 'x =. y'
3 : 0 ''
filecase =: tolower
1
)
filecase ''

NB. Verify singleton not changed when in use by fork
v =: 3 : 0
a =. 0 + 1
b =. 0 + 1
a =. a (+ , +) b
)
2 2 -: v''

NB. Verify that singleton gets same result as 1 item of an array, and also in "0.  Also verify inplacing gives the same value
compss =: 1 : 0 (&>)
yy =: y
assert. (val =. u f. etx y) -: (({.@:(u f.&(2 $ ,:))) etx y)
assert. (u f."+&:(5&$) y) -: 5&$@:(u f.) y
assert. (val =. u f. etx y) -:&(3!:0) (({.@:(u f.&(2 $ ,:))) etx y)
if. 2 ~: 3!:0 val do.
assert. val -: u f. etx 0 + y
end.
1
:
xx =: x
yy =: y
assert. (val =. x u f. etx y) -: (x ({.@:(u f.&(2 $ ,:))) etx y)
assert. (val =. x u f. etx y) -:&(3!:0) (x ({.@:(u f.&(2 $ ,:))) etx y)
assert. (x (u f."+&:(5&$) etx) y) -: x 5&$@:(u f.) etx y
if. 2 ~: 3!:0 val do.
assert. val -: x u 0 + y
assert. val -: x u 0 + y
assert. val -: (0 + x) u 0 + y
end.
1
)

NB. values to use
NB. Use smaller arrays if doing full memory audit
arglen =. ((QKTEST{100 10),2) {~ 9!:57 (0) [ 9!:57 (1)
NB. v =: 0;1;imax;imin;(<"0 (2.0-2.0)+0 1,imax,imin),((<"0)2 - 1 2),(<"0 i:_20),((<"0) 100 ?@$ 1e6),((<"0) 100000 * 200 ?@$ 0)
v =: 0;1;imax;imin;(<"0 (2.0-2.0)+0 1,imax,imin),((<"0)2 - 1 2),(<"0 i:_20<.arglen),((<"0) arglen ?@$ 1e6),((<"0) 100000 * (20 <. arglen) ?@$ 0)
NB. vv0 =: ((<"0) sdot0{~ arglen ?@$ #sdot0)
NB. vv1 =: ((<"0) adot1{~ arglen ?@$ #adot1)
NB. vv2 =: ((<"0) adot2{~ arglen ?@$ #adot2)

NB.  *./,  faster display scrolling
*./,   = compss/~ v
*./,   < compss/~ v
*./,   <. compss/~ v
*./,   <: compss/~ v
*./,   > compss/~ v
*./,   >. compss/~ v
*./,   >: compss/~ v
*./,   + compss/~ v
*./,   +. compss/~ v
*./,   +: compss/~ v
*./,   * compss/~ v
*./,   *. compss/~ v
*./,   *: compss/~ v
*./,   - compss/~ v
*./,   % compss/~ v
*./,   ~: compss/~ v
*./,   ^ compss/~ v
*./,   16 b. compss/~ v
*./,   17 b. compss/~ v
*./,   18 b. compss/~ v
*./,   19 b. compss/~ v
*./,   20 b. compss/~ v
*./,   21 b. compss/~ v
*./,   22 b. compss/~ v
*./,   23 b. compss/~ v
*./,   24 b. compss/~ v
*./,   25 b. compss/~ v
*./,   26 b. compss/~ v
*./,   27 b. compss/~ v
*./,   28 b. compss/~ v
*./,   29 b. compss/~ v
*./,   30 b. compss/~ v
*./,   31 b. compss/~ v
*./,   32 b. compss/~ v
*./,   33 b. compss/~ v
*./,   34 b. compss/~ v
*./,   | compss/~ v

*./,   = compss/~ v
*./,   < compss/~ v
*./,   <. compss/~ v
*./,   <: compss/~ v
*./,   > compss/~ v
*./,   >. compss/~ v
*./,   >: compss/~ v
*./,   ~: compss/~ v

NB. Remove large values for trig
v =: (#~ (1000&> *. _1000&<)@>) v
ops =: 0 ; 1 ; (, 0.1&+&.>) <"0 i: 12
*./,   ops o. compss/ v


NB. *./,   = compss/~ vv1
NB. *./,   ~: compss/~ vv1
NB. *./,   = compss/~ vv2
NB. *./,   ~: compss/~ vv2

NB. ! is slower, especially for big values
compssp =: 1 : 0 (&>)
:
smoutput x,y
(x u f. etx y) -: (x ({.@:(u f.&(2 $ ,:))) etx y)
)
v =: 0;1;(<"0 (2.0-2.0)+0 1),((<"0)2 - 1 2),((<"0) 5 ?@$ 1e6),((<"0)100000 * 5 ?@$ 0)
! compss/~ v

NB. Verify that different ranks produce correct result rank
NB. Use random data and varying verb rank; only the rank of the result is at issue
compssn =: 2 : 0
:
'l r'=.n
(x u f."n etx y) -: ((<"l x) u f.&> etx (<"r y))
)

ops =: ((1 $~ [) $ ])&.>/"1 b =: (;"0/~ i. 6) ([ ,"0 ($~ $)~) v
*./,   + compss/"1 ops
*./,   = compssn _5&>/"1 ops
*./,   < compssn _4&>/"1 ops
*./,   > compssn _3&>/"1 ops
*./,   <: compssn _2&>/"1 ops
*./,   >: compssn _1&>/"1 ops
*./,   +. compssn 0 _2&>/"1 ops
*./,   +. compssn 0 _1&>/"1 ops
*./,   +. compssn 0 0&>/"1 ops
*./,   +. compssn 0 1&>/"1 ops
*./,   +. compssn 0 2&>/"1 ops
*./,   +: compssn 1 _2&>/"1 ops
*./,   +: compssn 1 _1&>/"1 ops
*./,   +: compssn 1 0&>/"1 ops
*./,   +: compssn 1 1&>/"1 ops
*./,   +: compssn 1 2&>/"1 ops
*./,   *. compssn 2 _2&>/"1 ops
*./,   *. compssn 2 _1&>/"1 ops
*./,   *. compssn 2 0&>/"1 ops
*./,   *. compssn 2 1&>/"1 ops
*./,   *. compssn 2 2&>/"1 ops
*./,   *: compssn 3&>/"1 ops
*./,   ~: compssn 4&>/"1 ops
*./,   % compssn 5&>/"1 ops

NB. Monads
*./,   <. compss v
*./,   >. compss v
*./,   * compss v
*./,   + compss v
*./,   | compss v
*./,   %: compss v
*./,   ^ compss v
*./,   ^. compss v
*./,   ! compss v
*./,   o. compss v

NB. Verify each verb goes through a faster path
compst =: 1 : 0
:
timesing =. (QKTEST{2000 200) (6!:2) 'u f.&>~ y'
timearray =. (QKTEST{2000 200) (6!:2) 'u f.&>~ x'
timesing,timearray return.
u x  NB. to force dyad
)
vv0 =. <"0 (1000 $ 1 - 1)
vv1 =. <"1 (1000 2 $ 1 - 1)
THRESHOLD +. 0.75 > %/ vv1 + compst vv0
THRESHOLD +. 0.75 > %/ vv1 = compst vv0
THRESHOLD +. 0.75 > %/ vv1 < compst vv0
THRESHOLD +. 0.75 > %/ vv1 <. compst vv0
THRESHOLD +. 0.75 > %/ vv1 <: compst vv0
THRESHOLD +. 0.75 > %/ vv1 > compst vv0
THRESHOLD +. 0.75 > %/ vv1 >. compst vv0
THRESHOLD +. 0.75 > %/ vv1 >: compst vv0
THRESHOLD +. 0.75 > %/ vv1 + compst vv0
THRESHOLD +. 0.75 > %/ vv1 +. compst vv0
THRESHOLD +. 0.75 > %/ vv1 +: compst vv0
THRESHOLD +. 0.75 > %/ vv1 * compst vv0
THRESHOLD +. 0.75 > %/ vv1 *. compst vv0
THRESHOLD +. 0.75 > %/ vv1 *: compst vv0
THRESHOLD +. 0.75 > %/ vv1 - compst vv0
THRESHOLD +. 0.75 > %/ vv1 % compst vv0
THRESHOLD +. 0.75 > %/ vv1 ~: compst vv0
THRESHOLD +. 0.85 > %/ vv1 ^ compst vv0
THRESHOLD +. 0.75 > %/ vv1 16 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 17 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 18 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 19 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 20 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 21 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 22 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 23 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 24 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 25 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 26 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 27 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 28 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 29 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 30 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 31 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 | compst vv0
NB. no ss 0.75 > %/ vv1 32 b. compst vv0
NB. no ss 0.75 > %/ vv1 33 b. compst vv0
NB. no ss 0.75 > %/ vv1 34 b. compst vv0
THRESHOLD +. 0.75 > %/ vv1 = compst vv0
THRESHOLD +. 0.75 > %/ vv1 < compst vv0
THRESHOLD +. 0.75 > %/ vv1 <. compst vv0
THRESHOLD +. 0.75 > %/ vv1 <: compst vv0
THRESHOLD +. 0.75 > %/ vv1 > compst vv0
THRESHOLD +. 0.75 > %/ vv1 >. compst vv0
THRESHOLD +. 0.75 > %/ vv1 >: compst vv0
THRESHOLD +. 0.75 > %/ vv1 ~: compst vv0
THRESHOLD +. 0.75 > %/ vv1 0&o. compst vv0
THRESHOLD +. 0.75 > %/ vv1 1&o. compst vv0
THRESHOLD +. 0.75 > %/ vv1 _1&o. compst vv0
NB. Repeat for monad
compst =: 1 : 0
:
timesing =. (QKTEST{2000 200) (6!:2) 'u f.&> y'
timearray =. (QKTEST{2000 200) (6!:2) 'u f.&> x'
timesing,timearray return.
u x
)
NB. 0.75 > %/ vv1 <. compst vv0   <. >. on INT/BOOL is just a nop
NB. 0.75 > %/ vv1 >. compst vv0
NB. 0.75 > %/ vv1 + compst vv0   + on reals is faster than the ss code; we assume it won't be used much
THRESHOLD +. 0.9 > %/ vv1 * compst vv0
THRESHOLD +. 0.9 > %/ vv1 ^ compst vv0
THRESHOLD +. 0.9 > %/ vv1 | compst vv0
THRESHOLD +. 0.9 > %/ vv1 ! compst vv0
THRESHOLD +. 0.9 > %/ vv1 %: compst vv0
THRESHOLD +. 0.9 > %/ vv1 ^. compst vv0
THRESHOLD +. 0.9 > %/ vv1 o. compst vv0

NB. Verify that operations are performed in-place where possible
iptime =: 6!:2 '4 : ''for. i. y do. y =.y-1 [ t=.x end.''~ 100000'
niptime =: 6!:2 '4 : ''for. i. y do. y =.y-1 [ t=.y end.''~ 100000'
THRESHOLD+. niptime > 1.1 * iptime   NB. ~25% improvement normally

iptime1 =: 6!:2 '3 : ''for. i. y do. 1+1+1+1+1+1+1+1 end.'' 100000'
iptime2 =: 6!:2 '3 : ''for. i. y do. 1]1]1]1]1]1]1]1 end.'' 100000'
THRESHOLD+. iptime1 < 1.6 * iptime2  NB. Both are inplace; verify + not too slow

iptime1 =: 6!:2 '3 : ''for. i. y do. ************1 end.'' 100000'
iptime2 =: 6!:2 '3 : ''for. i. y do. ++++++++++++1 end.'' 100000'  NB. Not ssing
iptime3 =: 6!:2 '3 : ''for. i. y do. ]]]]]]]]]]]]1 end.'' 100000'  NB. inplace
THRESHOLD+. iptime1 < 0.85 * iptime2 NB. Faster than non-ssing
THRESHOLD+. iptime1 < 1.45 * iptime3 NB. Both are inplace; verify * not too slow


NB. Same for inside fork
iptime1 =: 6!:2 '3 : ''for. i. y do. (** ** ** ** ** ***)*1 end.'' 100000'
iptime2 =: 6!:2 '3 : ''for. i. y do. (++ ++ ++ ++ ++ +++) +1 end.'' 100000'  NB. Not ssing
iptime3 =: 6!:2 '3 : ''for. i. y do. (]] ]] ]] ]] ]] ]]]) ]1 end.'' 100000'  NB. inplace
THRESHOLD+. iptime1 < 0.85 * iptime2 NB. Faster than non-ssing
THRESHOLD+. iptime1 < 1.45 * iptime3 NB. Both are inplace; verify * not too slow

NB. If ^ needs to produce complex, it fails over to the normal code.  Verify no corruption of the in-place block
_0.707106781186547j0.707106781186548 = ({._1)^({.0.75)
_0.707106781186547j0.707106781186548 = ({._1)^(0.75)
_0.707106781186547j0.707106781186548 = (_1)^({.0.75)

NB. Same for %:y and ^.y
0j1.4142135623730951 = %: - 2  NB. In place
0j1p1 = ^. - 1   NB. In place

NB. Verify that the contents of a non-inplacable box are not inplaced
(2 2 1 $ 0 _2 2 0) -: -&>/~ ((<"_1) ,._1 1)

NB. verify inplaceable in assignment
vv0 =. 5 + 2
xx =: 15!:14<'vv0'
vv0 =. * vv0
xx = 15!:14<'vv0'  NB. verify inplace
vv0 =. 7 + vv0
xx = 15!:14<'vv0'  NB. verify inplace
vv0 =. vv0 + 6
xx = 15!:14<'vv0'  NB. verify inplace
vv1 =. vv0  NB. alias the value
vv0 =. * vv0
xx ~: yy =: 15!:14<'vv0'  NB. verify inplace
vv1 =. vv0  NB. alias the value
vv0 =. 7 + vv0
yy ~: xx =: 15!:14<'vv0'  NB. verify inplace
vv1 =. vv0  NB. alias the value
vv0 =. vv0 + 6
xx ~: yy =: 15!:14<'vv0'  NB. verify inplace




4!:55 ;:'adot1 adot2 sdot0 b compss compssn compssp compst compt dou filecase iptime iptime1 iptime2 iptime3 niptime ops tolower v v1 v2 v3 vv0 vv1 vv2 xx yy'
randfini''

epilog''

