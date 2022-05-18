prolog './g330f.ijs'
NB. x ;@:{ y ------------------------------------------------------------

randuni''

f=: 4 : '; x { y'
m=: 179

data=: 4 : 0
 select. y
  case.  1 do.         ?&.> ((?m$11),&.>x{'';'';5;2 3) $&.> 2
  case.  2 do. {&a.&.> ?&.> ((?m$11),&.>x{'';'';5;2 3) $&.> #a.
  case.  131072 do. {&adot1&.> ?&.> ((?m$11),&.>x{(u:'');(u:'');5;2 3) $&.> #adot1
  case.  262144 do. {&adot2&.> ?&.> ((?m$11),&.>x{(10&u:'');(10&u:'');5;2 3) $&.> #adot2
  case.  65536  do. {&sdot0&.> ?&.> ((?m$11),&.>x{(s:'');(s:'');5;2 3) $&.> #sdot0
  case.  4 do.         ?&.> ((?m$11),&.>x{'';'';5;2 3) $&.> 500
  case.  8 do.   o.&.> ?&.> ((?m$11),&.>x{'';'';5;2 3) $&.> 500
  case. 16 do.   r.&.> ?&.> ((?m$11),&.>x{'';'';5;2 3) $&.> 500
 end.
)

test=: 3 : 0
 i=: ?2003$m
 b=: ?2003$2
 assert. i       (f -: ;@:{) yy=: 1 data y
 assert. (+:-:i) (f -: ;@:{) yy
 assert. b       (f -: ;@:{) yy
 assert. i       (f -: ;@:{) yy=: 2 data y
 assert. (+:-:i) (f -: ;@:{) yy
 assert. b       (f -: ;@:{) yy
 assert. i       (f -: ;@:{) yy=: 3 data y
 assert. (+:-:i) (f -: ;@:{) yy
 assert. b       (f -: ;@:{) yy
 1
)

test 1
test 2
test 131072
test 262144
test 65536
test 4
test 8
test 16

'domain error' -: (30$'a')  ;@:{ etx 2$<'xy'
'domain error' -: (30$<'a') ;@:{ etx 2$<'xy'
'domain error' -: (30$1.5)  ;@:{ etx 2$<'xy'
'domain error' -: (30$3j1)  ;@:{ etx 2$<'xy'
'domain error' -: (30$1 2)  ;@:{ etx 2 3;'abc';4 5 6 7
'domain error' -: (30$'a')  ;@:{ etx 2$<u:'xy'
'domain error' -: (30$<'a') ;@:{ etx 2$<u:'xy'
'domain error' -: (30$1.5)  ;@:{ etx 2$<u:'xy'
'domain error' -: (30$3j1)  ;@:{ etx 2$<u:'xy'
'domain error' -: (30$1 2)  ;@:{ etx 2 3;(u:'abc');4 5 6 7
'domain error' -: (30$'a')  ;@:{ etx 2$<10&u:'xy'
'domain error' -: (30$<'a') ;@:{ etx 2$<10&u:'xy'
'domain error' -: (30$1.5)  ;@:{ etx 2$<10&u:'xy'
'domain error' -: (30$3j1)  ;@:{ etx 2$<10&u:'xy'
'domain error' -: (30$1 2)  ;@:{ etx 2 3;(10&u:'abc');4 5 6 7
'domain error' -: (30$s:@<"0 'a')  ;@:{ etx 2$s:@<"0&.> <'xy'
'domain error' -: (30$s:@<"0 'a')  ;@:{ etx 2$<"0@s: <'xy'
'domain error' -: (30$s:@<"0&.> <'a') ;@:{ etx 2$s:@<"0&.> <'xy'
'domain error' -: (30$<"0@s: <'a') ;@:{ etx 2$<"0@s: <'xy'
'domain error' -: (30$1.5)  ;@:{ etx 2$s:@<"0&.> <'xy'
'domain error' -: (30$1.5)  ;@:{ etx 2$<"0@s: <'xy'
'domain error' -: (30$3j1)  ;@:{ etx 2$s:@<"0&.> <'xy'
'domain error' -: (30$3j1)  ;@:{ etx 2$<"0@s: <'xy'
'domain error' -: (30$1 2)  ;@:{ etx 2 3;(s:@<"0 'abc');4 5 6 7

'index error'  -: (30$3)    ;@:{ etx 2$<'xy'
'index error'  -: (71$3 _6) ;@:{ etx 5$<'xy'
'index error'  -: (30$3)    ;@:{ etx 2$<u:'xy'
'index error'  -: (71$3 _6) ;@:{ etx 5$<u:'xy'
'index error'  -: (30$3)    ;@:{ etx 2$<10&u:'xy'
'index error'  -: (71$3 _6) ;@:{ etx 5$<10&u:'xy'
'index error'  -: (30$3)    ;@:{ etx 2$s:@<"0&.> <'xy'
'index error'  -: (30$3)    ;@:{ etx 2$<"0@s: <'xy'
'index error'  -: (71$3 _6) ;@:{ etx 5$s:@<"0&.> <'xy'
'index error'  -: (71$3 _6) ;@:{ etx 5$<"0@s: <'xy'


4!:55 ;:'adot1 adot2 sdot0 b data f i m test yy'
randfini''


epilog''

