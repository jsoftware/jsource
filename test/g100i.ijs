prolog './g100i.ijs'
NB. +/ B ----------------------------------------------------------------

f=: 3 : 'y -: +/y$1'
f"0 ?2 10$1e5
f"0 i.4 10
f"0 [_1 0 1+  255
f"0 [_1 0 1+4*255
f"0 [_1 0 1+8*255

f=: 3 : 'y -: +/(i.5e3) e. y?5e3'
f"0 ?2 10$4e3
f"0 i.4 10
f"0 [_1 0 1+  255
f"0 [_1 0 1+4*255
f"0 [_1 0 1+8*255

f=: 3 : 'n  -: +/(i.y) e. (n=.<.-:y)?y'
f"0 ?2 10$1e4
f"0 i.4 10
f"0 [_1 0 1+  255
f"0 [_1 0 1+4*255
f"0 [_1 0 1+8*255

f=: 3 : '($/|.y) -: +/y$1'
,f"1 ,."0 1~i.10
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+8*255
,f"1 |."1 x

f=: 3 : '200 -: +/+/(i.y) e. 200?*/y'
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+8*255
,f"1 |."1 x

plus=: 4 : 'x+y'

f=: 3 : '(+/ -: plus/) y ?@$ 2'
,f"1 x=:7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x
,f"1 x=:7 8 9,."0 1 [ _1 0 1+8*255
,f"1 |."1 x


NB. +/ I ----------------------------------------------------------------

plus=: 4 : 'x+y'

(+/ -: plus/) x=:1 2 3 1e9 2e9
(+/ -: plus/) |.x

(+/   -: plus/  ) x=:_1e4+?    23$2e4
(+/   -: plus/  ) x=:_1e4+?4   23$2e4
(+/"1 -: plus/"1) x
(+/   -: plus/  ) x=:_1e4+?7 5 23$2e4
(+/"1 -: plus/"1) x
(+/"2 -: plus/"2) x

(+/   -: plus/  ) x=:_1e9+?    23$2e9
(+/   -: plus/  ) x=:_1e9+?4   23$2e9
(+/"1 -: plus/"1) x
(+/   -: plus/  ) x=:_1e9+?7 5 23$2e9
(+/"1 -: plus/"1) x
(+/"2 -: plus/"2) x


NB. +/ D ----------------------------------------------------------------

plus=: 4 : 'x+y'

(+/   -:!.1e_11 plus/  ) x=:0.01*_1e9+?    11$2e9  NB. comparison not close because of different orders of operations
(+/   -:!.1e_11 plus/  ) x=:0.01*_1e9+?4   11$2e9
(+/"1 -:!.1e_11 plus/"1) x
(+/   -:!.1e_11 plus/  ) x=:0.01*_1e9+?7 5 11$2e9
(+/"1 -:!.1e_11 plus/"1) x
(+/"2 -:!.1e_11 plus/"2) x
(+/   -:!.1e_11 plus/  )@> x=:(0.01*_1e9+2e9 ?@$~ ])&.> 4 ,&.>/ >: i. 100
(+/   -:!.1e_11 plus/  )@> x=:(0.01*_1e9+2e9 ?@$~ ])&.> >: i. 100

NB. +/!.0 D ----------------------------------------------------------------

f =: 4 : 0"0 1  NB. y is shape of region, x is rank of sums
xasrank =. x (1 : ']"m')
big =. (, -@:(+/))@}:"xasrank y ?@$ 1e8   NB. large integers adding to 0
small =. y ?@$ 0   NB. floats in range 0-1
bigfloat =. big+small  NB. floats with large & small components
smalltrunc =. bigfloat-big  NB. the actual small parts of bigfloat
((+/"xasrank smalltrunc) + +/"xasrank big) ; (+/"xasrank bigfloat) ; (+/!.0"xasrank bigfloat)  NB. correct total, +/ version, +/!.0 version
)
0.05 > 1 ([: | [: -/ 0 2 {::"0 _ f)"0 x =: 1e7 + i. 17
0.05 > 1 ([: | [: -/ 0 2 {::"0 _ f)"0 x =: >: i. 32
0 -: +/!.0 i. 0
0.05 > _ ([: | [: -/ 0 2 {::"0 _ f)"0 1/ x =: (1e6 + i. 20) ,"0 ] 6
0.005 > _ ([: | [: -/ 0 2 {::"0 _ f)"0 1/ x =: (1e5) ,"0 ] >: i. 20
, 1 2 3 ([: *./@, 1e_6 > [: | [: -/ 0 2 {::"0 _ f)"0 1/ x =: > { 1 2 3 ; (>: i. 20) ; (>: i. 20)
, 1 2 3 4 ([: *./@, 1e_6 > [: | [: -/ 0 2 {::"0 _ f)"0 1/ x =: > { 1 2 3 ; (>: i. 20) ; (>: i. 20) ; (>: i. 20)

NB. +/ Z ----------------------------------------------------------------

plus=: 4 : 'x+y'

(+/   -:!.1e_11 plus/  ) x=:j./0.01*_1e9+?2     11$2e9
(+/   -:!.1e_11 plus/  ) x=:j./0.01*_1e9+?2 4   11$2e9
(+/"1 -:!.1e_11 plus/"1) x
(+/   -:!.1e_11 plus/  ) x=:j./0.01*_1e9+?2 7 5 11$2e9
(+/"1 -:!.1e_11 plus/"1) x
(+/"2 -:!.1e_11 plus/"2) x


NB. +/ X ----------------------------------------------------------------

(+/   -: +/  @:x:) x=:_1e5+?2 7 5 23$2e5
(+/"1 -: +/"1@:x:) x
(+/"2 -: +/"2@:x:) x
(+/"3 -: +/"3@:x:) x

'domain error' -: +/   etx 3 4$'abc'
'domain error' -: +/"1 etx 3 4$'abc'
'domain error' -: +/   etx ;:'modus operandi'
'domain error' -: +/"1 etx 3 4$;:'I think not'
'domain error' -: +/   etx 3 4$u:'abc'
'domain error' -: +/"1 etx 3 4$u:'abc'
'domain error' -: +/   etx u:&.> ;:'modus operandi'
'domain error' -: +/"1 etx 3 4$(u:&.>) ;:'I think not'
'domain error' -: +/   etx 3 4$10&u:'abc'
'domain error' -: +/"1 etx 3 4$10&u:'abc'
'domain error' -: +/   etx 10&u:&.> ;:'modus operandi'
'domain error' -: +/"1 etx 3 4$(10&u:&.>) ;:'I think not'
'domain error' -: +/   etx 3 4$s:@<"0 'abc'
'domain error' -: +/"1 etx 3 4$s:@<"0 'abc'
'domain error' -: +/   etx s:@<"0&.> ;:'modus operandi'
'domain error' -: +/   etx <"0@s: ;:'modus operandi'
'domain error' -: +/"1 etx 3 4$s:@<"0&.> ;:'I think not'
'domain error' -: +/"1 etx 3 4$<"0@s: ;:'I think not'


4!:55 ;:'f plus x'



epilog''

