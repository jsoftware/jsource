prolog './g111s.ijs'
NB. *./\. B -------------------------------------------------------------

(0 0 0 1,:0 1 0 1) -: *./\. 0 0 1 1 ,: 0 1 0 1

and=: 4 : 'x*.y'

f=: 3 : '(*./\. -: and/\.) y ?@$ 2'
f"0 x=:>:i.2 10
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x


NB. *./\. I -------------------------------------------------------------

and=: 4 : 'x*.y'

(*./\. -: and/\.) x=:1 2 3 1e2 2e2
(*./\. -: and/\.) |.x
(*./\. -: and/\.) x=:1 2 3 1e9 2e9
(*./\. -: and/\.) |.x

(*./\.   -: and/\.  ) x=:_1e4+?    23$2e4
(*./\.   -: and/\.  ) x=:_1e4+?4   23$2e4
(*./\."1 -: and/\."1) x
(*./\.   -: and/\.  ) x=:_1e4+?7 5 23$2e4
(*./\."1 -: and/\."1) x
(*./\."2 -: and/\."2) x

(*./\.   -: and/\.  ) x=:_1e2+?    23$2e2
(*./\.   -: and/\.  ) x=:_1e2+?4   23$2e2
(*./\."1 -: and/\."1) x
(*./\.   -: and/\.  ) x=:_1e2+?7 5 23$2e2
(*./\."1 -: and/\."1) x
(*./\."2 -: and/\."2) x

'domain error' -: *./\. etx 'deipnosophist'
'domain error' -: *./\. etx ;:'professors in New England'
'domain error' -: *./\. etx u:'deipnosophist'
'domain error' -: *./\. etx u:&.> ;:'professors in New England'
'domain error' -: *./\. etx 10&u:'deipnosophist'
'domain error' -: *./\. etx 10&u:&.> ;:'professors in New England'
'domain error' -: *./\. etx s:@<"0 'deipnosophist'
'domain error' -: *./\. etx s:@<"0&.> ;:'professors in New England'
'domain error' -: *./\. etx <"0@s: ;:'professors in New England'




epilog''

