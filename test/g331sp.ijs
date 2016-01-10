NB. x f;.n y for sparse x, n e. _2 _1 1 2 -------------------------------

box=: 3 : '<y'

C=: 1 : 0
  :
 assert. x (u;. 1 -: $.@[ u;. 1 ]) y
 assert. x (u;._1 -: $.@[ u;._1 ]) y
 assert. x (u;. 2 -: $.@[ u;. 2 ]) y
 assert. x (u;._2 -: $.@[ u;._2 ]) y
 1
)

f=: 1 : 0
 n=. #y
 assert. b u C y [ b=. ?n$2
 assert. b u C y [ b=. 0=?n$10
 assert. b u C y [ b=. 1 ( 0)}b
 assert. b u C y [ b=. 1 (_1)}b
 assert. b u C y [ b=. n$1
 assert. b u C y [ b=. n$0
 assert. b u C y [ b=. (? = i.) n
 1
)

<    f x=: ?100$1e6
$    f x
[    f x
,    f x
<@}. f x
box  f x

<    f x=: a.{~ ?101 1 7$#a.
$    f x
[    f x
,    f x
<@}. f x
box  f x

test=: 4 : 0
 b=: $. (#y){.(i._2+#y) e. +/\2+?(#y)$10
 assert. (b #   ;. x y) -: b (3 : '#    y');.x y
 assert. (b $   ;. x y) -: b (3 : '$    y');.x y
 assert. (b {.  ;. x y) -: b (3 : '{.   y');.x y
 assert. (b {:  ;. x y) -: b (3 : '{:   y');.x y
 assert. (b ,   ;. x y) -: b (3 : ',    y');.x y
 assert. (b [   ;. x y) -: b (3 : '[    y');.x y
 assert. (b ]   ;. x y) -: b (3 : ']    y');.x y
 assert. (b <   ;. x y) -: b (3 : '<    y');.x y
 assert. (b <@}.;. x y) -: b (3 : '<@}. y');.x y
 assert. (b <@}:;. x y) -: b (3 : '<@}: y');.x y
 1
)

1  test t=: a.{~ ?300$#a.
_1 test t
2  test t
_2 test t

1  test t=: a.{~?101 2 3$#a.
_1 test t
2  test t
_2 test t

1  test t=: ?317$1e6
_1 test t
2  test t
_2 test t

1  test t=: ? 173 2 3$100
_1 test t
2  test t
_2 test t

'length error' -: ($. 1 0 1 0 0) <;.1 etx i.7

'index error'  -: ($. 1 1 1 0 0) {.;._1 etx i.5


4!:55 ;:'b box C f t test x'


