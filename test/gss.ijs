NB. Speedy Singletons ------------------------------------------------------------------

randuni''

NB. Tests of local-name lookup

NB. Local name passed to another modifier
'`v1 v2 v3' =: -`*:`-:   NB. these are the values found by dou
dou =: 1 : 'u  :: ] y'
 _5 25 3 -: 3 : 0 ''
r =. ''
v1 =. v4 =. v5 =. v6 =. v7 =. v8 =. +
r =. r , v1 dou 5
v2 =. %:
r =. r , v2 dou 5
v3 =. +:
r =. r , v3 dou 6
r
)


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

NB. Verify that singleton gets same result as 1 item of an array
compss =: 1 : 0 (&>)
(u etx y) -: (({.@:(u&(2 $ ,:))) etx y)
:
(x u etx y) -: (x ({.@:(u&(2 $ ,:))) etx y)
)

NB. values to use
NB. v =: 0;1;imax;imin;(<"0 (2.0-2.0)+0 1,imax,imin),((<"0)2 - 1 2),(<"0 i:_20),((<"0) 100 ?@$ 1e6),((<"0) 100000 * 200 ?@$ 0)
v =: 0;1;imax;imin;(<"0 (2.0-2.0)+0 1,imax,imin),((<"0)2 - 1 2),(<"0 i:_20),((<"0) 100 ?@$ 1e6),((<"0) 100000 * 20 ?@$ 0)
vv0 =: ((<"0) sdot0{~ 100 ?@$ #sdot0)
vv1 =: ((<"0) adot1{~ 100 ?@$ #adot1)
vv2 =: ((<"0) adot2{~ 100 ?@$ #adot2)

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

*./,   = compss/~ vv0
*./,   < compss/~ vv0
*./,   <. compss/~ vv0
*./,   <: compss/~ vv0
*./,   > compss/~ vv0
*./,   >. compss/~ vv0
*./,   >: compss/~ vv0
*./,   ~: compss/~ vv0

*./,   = compss/~ vv1
*./,   ~: compss/~ vv1

*./,   = compss/~ vv2
*./,   ~: compss/~ vv2

NB. ! is slower, especially for big values
compssp =: 1 : 0 (&>)
:
smoutput x,y
(x u etx y) -: (x ({.@:(u&(2 $ ,:))) etx y)
)
v =: 0;1;(<"0 (2.0-2.0)+0 1),((<"0)2 - 1 2),((<"0) 5 ?@$ 1e6),((<"0)100000 * 5 ?@$ 0)
! compss/~ v

NB. Verify that different ranks produce correct result rank
NB. Use random data and varying verb rank; only the rank of the result is at issue
compssn =: 2 : 0
:
'l r'=.n
(x u"n etx y) -: ((<"l x) u&> etx (<"r y))
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
*./,   | compss v
*./,   %: compss v
*./,   ^ compss v
*./,   ^. compss v
*./,   ! compss v
*./,   o. compss v

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

4!:55 ;:'adot1 adot2 sdot0 b compss compssn compssp dou filecase iptime iptime1 iptime2 iptime3 niptime ops tolower v v1 v2 v3 vv0 vv1 vv2'

