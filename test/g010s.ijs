prolog './g010s.ijs'
NB. </\. B -------------------------------------------------------------

(0 1 0 0,:0 1 0 1) -: </\. 0 0 1 1 ,: 0 1 0 1

lt=: 4 : 'x<y'

f=: 3 : '(</\. -: lt/\.) ?y$2'
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x


NB. </\. I -------------------------------------------------------------

lt=: 4 : 'x<y'

(</\. -: lt/\.) x=:1 2 3 1e9 2e9
(</\. -: lt/\.) |.x

(</\.   -: lt/\.  ) x=:_1e4+?    23$2e4
(</\.   -: lt/\.  ) x=:_1e4+?4   23$2e4
(</\."1 -: lt/\."1) x
(</\.   -: lt/\.  ) x=:_1e4+?7 5 23$2e4
(</\."1 -: lt/\."1) x
(</\."2 -: lt/\."2) x

(</\.   -: lt/\.  ) x=:_1e9+?    23$2e9
(</\.   -: lt/\.  ) x=:_1e9+?4   23$2e9
(</\."1 -: lt/\."1) x
(</\.   -: lt/\.  ) x=:_1e9+?7 5 23$2e9
(</\."1 -: lt/\."1) x
(</\."2 -: lt/\."2) x


NB. </\. D -------------------------------------------------------------

lt=: 4 : 'x<y'

(</\.   -: lt/\.  ) x=:0.01*_1e4+?    23$2e4
(</\.   -: lt/\.  ) x=:0.01*_1e4+?4   23$2e4
(</\."1 -: lt/\."1) x
(</\.   -: lt/\.  ) x=:0.01*_1e4+?7 5 23$2e4
(</\."1 -: lt/\."1) x
(</\."2 -: lt/\."2) x


NB. </\. Z -------------------------------------------------------------

lt=: 4 : 'x<y'

(</\.   -: lt/\.  ) x=:[&.j. 0.1*_1e2+?2     23$2e2
(</\.   -: lt/\.  ) x=:[&.j. 0.1*_1e2+?2 4   23$2e2
(</\."1 -: lt/\."1) x
(</\.   -: lt/\.  ) x=:[&.j. 0.1*_1e2+?2 7 5 23$2e2
(</\."1 -: lt/\."1) x
(</\."2 -: lt/\."2) x

'domain error' -: </\. etx 3j4 5
'domain error' -: </\. etx 'deipnosophist'
'domain error' -: </\. etx ;:'professors in New England'
'domain error' -: </\. etx u:'deipnosophist'
'domain error' -: </\. etx u:&.> ;:'professors in New England'
'domain error' -: </\. etx 10&u:'deipnosophist'
'domain error' -: </\. etx 10&u:&.> ;:'professors in New England'
'domain error' -: </\. etx s:@<"0 'deipnosophist'
'domain error' -: </\. etx s:@<"0&.> ;:'professors in New England'
'domain error' -: </\. etx <"0@s: ;:'professors in New England'




epilog''

