prolog './g102i.ijs'
NB. +:/ B ---------------------------------------------------------------

1 0 0 0 -: +:/ 0 0 1 1 ,: 0 1 0 1

nor=: 4 : 'x+:y'

(+:/"1 -: nor/"1) x=:?3 5 17$2
(+:/"2 -: nor/"2) x
(+:/"3 -: nor/"3) x

(+:/"1 -: nor/"1) x=:?3 5 32$2
(+:/"2 -: nor/"2) x
(+:/"3 -: nor/"3) x

(+:/"1 -: nor/"1) x=:?3 8 32$2
(+:/"2 -: nor/"2) x
(+:/"3 -: nor/"3) x

f=: 3 : '(+:/ -: nor/) y ?@$ 2'
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x




epilog''

