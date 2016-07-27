NB. Speedy Singletons ------------------------------------------------------------------

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


NB. Verify that singleton gets same result as 1 item of an array
compss =: 1 : 0 (&>)
:
(x u etx y) -: (x ({.@:(u&(2 $ ,:))) etx y)
)

NB. values to use
v =: 0;1;imax;imin;(<"0 (2.0-2.0)+0 1,imax,imin),((<"0)2 - 1 2),(<"0 i:_20),((<"0) 100 ?@$ 1e6),((<"0) 100000 * 200 ?@$ 0)

= compss/~ v
< compss/~ v
<. compss/~ v
<: compss/~ v
>: compss/~ v
+ compss/~ v
+. compss/~ v
+: compss/~ v
* compss/~ v
*. compss/~ v
*: compss/~ v
- compss/~ v
% compss/~ v
~: compss/~ v

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
+ compss/"1 ops
= compssn _5&>/"1 ops
< compssn _4&>/"1 ops
> compssn _3&>/"1 ops
<: compssn _2&>/"1 ops
>: compssn _1&>/"1 ops
+. compssn 0 _2&>/"1 ops
+. compssn 0 _1&>/"1 ops
+. compssn 0 0&>/"1 ops
+. compssn 0 1&>/"1 ops
+. compssn 0 2&>/"1 ops
+: compssn 1 _2&>/"1 ops
+: compssn 1 _1&>/"1 ops
+: compssn 1 0&>/"1 ops
+: compssn 1 1&>/"1 ops
+: compssn 1 2&>/"1 ops
*. compssn 2 _2&>/"1 ops
*. compssn 2 _1&>/"1 ops
*. compssn 2 0&>/"1 ops
*. compssn 2 1&>/"1 ops
*. compssn 2 2&>/"1 ops
*: compssn 3&>/"1 ops
~: compssn 4&>/"1 ops
% compssn 5&>/"1 ops

NB. Verify that operations are performed in-place where possible
iptime =: 6!:2 '4 : ''for. i. y do. y =.y-1 [ t=.x end.''~ 1000000'
niptime =: 6!:2 '4 : ''for. i. y do. y =.y-1 [ t=.y end.''~ 1000000'
niptime > 1.1 * iptime   NB. ~25% improvement normally

iptime1 =: 6!:2 '3 : ''for. i. y do. 1+1+1+1+1+1+1+1 end.'' 1000000'
iptime2 =: 6!:2 '3 : ''for. i. y do. 1]1]1]1]1]1]1]1 end.'' 1000000'
iptime1 < 1.40 * iptime2  NB. Both are inplace; verify + not too slow

4!:55 ;:'compss compssn compssp iptime niptime v'


