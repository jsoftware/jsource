prolog './g101s.ijs'
NB. +./\. B -------------------------------------------------------------

(0 1 1 1,:0 1 0 1) -: +./\. 0 0 1 1 ,: 0 1 0 1

or=: 4 : 'x+.y'

f=: 3 : '(+./\. -: or/\.) y ?@$ 2'
f"0 x=:>:i.2 10
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4+255
,f"1 |."1 x


NB. +./\. I -------------------------------------------------------------

or=: 4 : 'x+.y'

(+./\. -: or/\.) x=:1 2 3 1e2 2e2
(+./\. -: or/\.) |.x
(+./\. -: or/\.) x=:1 2 3 1e9 2e9
(+./\. -: or/\.) |.x

(+./\.   -: or/\.  ) x=:_1e4+?    23$2e4
(+./\.   -: or/\.  ) x=:_1e4+?4   23$2e4
(+./\."1 -: or/\."1) x
(+./\.   -: or/\.  ) x=:_1e4+?7 5 23$2e4
(+./\."1 -: or/\."1) x
(+./\."2 -: or/\."2) x

(+./\.   -: or/\.  ) x=:_1e2+?    23$2e2
(+./\.   -: or/\.  ) x=:_1e2+?4   23$2e2
(+./\."1 -: or/\."1) x
(+./\.   -: or/\.  ) x=:_1e2+?7 5 23$2e2
(+./\."1 -: or/\."1) x
(+./\."2 -: or/\."2) x

'domain error' -: +./\. etx 'deipnosophist'
'domain error' -: +./\. etx ;:'professors in New England'
'domain error' -: +./\. etx u:'deipnosophist'
'domain error' -: +./\. etx u:&.> ;:'professors in New England'
'domain error' -: +./\. etx 10&u:'deipnosophist'
'domain error' -: +./\. etx 10&u:&.> ;:'professors in New England'
'domain error' -: +./\. etx s:@<"0 'deipnosophist'
'domain error' -: +./\. etx s:@<"0&.> ;:'professors in New England'
'domain error' -: +./\. etx <"0@s: ;:'professors in New England'




epilog''

