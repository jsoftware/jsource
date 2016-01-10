NB. prehashed i. family of functions ------------------------------------

g=: 4 : 0
 xx=: y{~(1e4,x) ?@$ #y 
 yy=: y{~(1e3,x) ?@$ #y
 ss=: y{~(    x) ?@$ #y
 fidot=: xx&i.
 fico =: xx&i:
 fedot=: e.&xx
 assert. (fidot yy) -: xx i. yy
 assert. (fico  yy) -: xx i: yy
 assert. (fedot yy) -: yy e. xx
 assert. (fidot ss) -: xx i. ss
 assert. (fico  ss) -: xx i: ss
 assert. (fedot ss) -: ss e. xx
 1 
)

h=: 4 : 0
 xx=: y{~(1e4,x) ?@$ #y 
 yy=: y{~(1e3,x) ?@$ #y
 ss=: y{~(    x) ?@$ #y
 fidot=: xx&(i.!.0)
 fico =: xx&(i:!.0)
 fedot=: e.!.0&xx
 assert. (fidot yy) -: xx i. yy
 assert. (fico  yy) -: xx i: yy
 assert. (fedot yy) -: yy e. xx
 assert. (fidot ss) -: xx i. ss
 assert. (fico  ss) -: xx i: ss
 assert. (fedot ss) -: ss e. xx
 1 
)

''  g 0 1
2 3 g 0 1
31  g 0 1

''  g a.
2 3 g a.
37  g a.

''  g u: 1000 ?@# 256
2 3 g u: 1000 ?@# 256
13  g u: 1000 ?@# 256
''  g u: 1000 ?@# 65536
2 3 g u: 1000 ?@# 65536
13  g u: 1000 ?@# 65536

''  g 1000 ?@$ 0
2 3 g 1000 ?@$ 0

''  h 1000 ?@$ 0
2 3 h 1000 ?@$ 0

''  g j./ _1e4+2 1000 ?@$ 2e4
2 3 g j./ _1e4+2 1000 ?@$ 2e4

''  h j./ _1e4+2 1000 ?@$ 2e4
2 3 h j./ _1e4+2 1000 ?@$ 2e4

''  g x: 1000 ?@$ 3e3
2 3 g x: 1000 ?@$ 3e3

''  g %/ x: 0 1 + 2 1000 ?@$ 3e3
2 3 g %/ x: 0 1 + 2 1000 ?@$ 3e3

''  g ;:'Cogito, ergo sum. 4 20 and 10 years ago'
1 2 g ;:'Cogito, ergo sum. 4 20 and 10 years ago'

''  g s: ' Cogito ergo sum 4 20 and 10 years ago kakistocracy'
2 3 g s: ":&.> 20 ?@$1000

''  g      1000 ?@$ m=: 1e1
''  g m -~ 1000 ?@$ 2*m
''  g      1000 ?@$ m=: 1e3
''  g m -~ 1000 ?@$ 2*m
''  g      1000 ?@$ m=: 1e5
''  g m -~ 1000 ?@$ 2*m
''  g      1000 ?@$ m=: 1e7
''  g m -~ 1000 ?@$ 2*m
''  g      1000 ?@$ m=: 1e9
''  g m -~ 1000 ?@$ 2*m

2 3 g      1000 ?@$ m=: 1e1
2 3 g m -~ 1000 ?@$ 2*m
2 3 g      1000 ?@$ m=: 1e3
2 3 g m -~ 1000 ?@$ 2*m
2 3 g      1000 ?@$ m=: 1e5
2 3 g m -~ 1000 ?@$ 2*m
2 3 g      1000 ?@$ m=: 1e7
2 3 g m -~ 1000 ?@$ 2*m
2 3 g      1000 ?@$ m=: 1e9
2 3 g m -~ 1000 ?@$ 2*m

x=: 10 ?@$2
f=: x&i.
(i.~x) -: f x+0
(i.~x) -: f x+-~0.1
(i.~x) -: f x+-~0j1
(i.~x) -: f x+-~1x
(i.~x) -: f x+-~1r2

((i.7 2) -.   2 3) -: -.&   2 3  i. 7 2
((i.7 2) -. ,:2 3) -: -.&(,:2 3) i. 7 2

'rank error'   -: (i.3 4)&i. etx 7

'length error' -: (i.3 4)&i. etx i.5
'length error' -: (i.3 4)&i. etx i.2 5


4!:55 ;:'f fedot fico fidot g h m ss x xx yy'


 