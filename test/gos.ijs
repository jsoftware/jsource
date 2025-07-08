prolog './gos.ijs'
NB. o./\. B -------------------------------------------------------------

cir=. 4 : 'x o. y'

(o./\.   -: cir/\.  ) x=: ?    23$2
(o./\.   -: cir/\.  ) x=: ?3   23$2
(o./\."1 -: cir/\."1) x
(o./\.   -: cir/\.  ) x=: ?7 5 23$2
(o./\."1 -: cir/\."1) x
(o./\."2 -: cir/\."2) x

(o./\.   -: cir/\.  ) x=: ?    24$2
(o./\.   -: cir/\.  ) x=: ?4   12$2
(o./\."1 -: cir/\."1) x
(o./\.   -: cir/\.  ) x=: ?4 2  8$2
(o./\."1 -: cir/\."1) x
(o./\."2 -: cir/\."2) x


NB. o./\. I -------------------------------------------------------------

cir=. 4 : 'x o. y'

(o./\.   -: cir/\.  ) x=: _12+?    2$25
(o./\.   -: cir/\.  ) x=: _12+?2   2$25
(o./\."1 -: cir/\."1) x
(o./\.   -: cir/\.  ) x=: _12+?2 2 2$25
(o./\."1 -: cir/\."1) x
(o./\."2 -: cir/\."2) x


NB. o./\. D -------------------------------------------------------------

cir=. 4 : 'x o. y'

(o./\.   -: cir/\.  ) x=: [&.o. _7+?    2$15
(o./\.   -: cir/\.  ) x=: [&.o. _7+?2   2$15
(o./\."1 -: cir/\."1) x
(o./\.   -: cir/\.  ) x=: [&.o. _7+?2 2 2$15
(o./\."1 -: cir/\."1) x
(o./\."2 -: cir/\."2) x


NB. o./\. Z -------------------------------------------------------------

cir=. 4 : 'x o. y'

(o./\.   -: cir/\.  ) x=:[&.j._7+?    2$15
(o./\.   -: cir/\.  ) x=:[&.j._7+?2   2$15
(o./\."1 -: cir/\."1) x
(o./\.   -: cir/\.  ) x=:[&.j._7+?2 2 2$15
(o./\."1 -: cir/\."1) x
(o./\."2 -: cir/\."2) x

(,'j')  -: o./\. 'j'
(,<123) -: o./\. <123

'domain error' -: o./\. etx 'eleemosynary'
'domain error' -: o./\. etx ;:'quod erat demonstratum'
'domain error' -: o./\. etx u:'eleemosynary'
'domain error' -: o./\. etx u:&.> ;:'quod erat demonstratum'
'domain error' -: o./\. etx 10&u:'eleemosynary'
'domain error' -: o./\. etx 10&u:&.> ;:'quod erat demonstratum'
'domain error' -: o./\. etx s:@<"0 'eleemosynary'
'domain error' -: o./\. etx s:@<"0&.> ;:'quod erat demonstratum'
'domain error' -: o./\. etx <"0@s: ;:'quod erat demonstratum'




epilog''

