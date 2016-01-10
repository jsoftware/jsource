NB. x +/\ y -------------------------------------------------------------

test=: 1 : 0
 f=: 3 : ((5!:5 <'u'),'/y')\
 g=: u f./\
 mm=: 1+?50
 for_c. (i.0);3;2 3 do.
  d=. >c
  assert. mm (f -: g) yy=: (?(500   ,d)$#y){y
  assert. mm (f -: g) yy=: (?(254   ,d)$#y){y
  assert. mm (f -: g) yy=: (?(255   ,d)$#y){y
  assert. mm (f -: g) yy=: (?(256   ,d)$#y){y
  assert. mm (f -: g) yy=: (?(257   ,d)$#y){y
  assert. mm (f -: g) yy=: (?(258   ,d)$#y){y
  assert. mm (f -: g) yy=: (?(mm    ,d)$#y){y
  assert. mm (f -: g) yy=: (?((mm-1),d)$#y){y
  assert. 2  (f -: g) yy=: (?(20    ,d)$#y){y
  assert. 2  (f -: g) yy=: (?(1     ,d)$#y){y
  assert. 1  (f -: g) yy=: (?(20    ,d)$#y){y
  assert. 1  (f -: g) yy=: (?(1     ,d)$#y){y
  assert. 0  (f -: g) yy=: (?(20    ,d)$#y){y
  assert. 0  (f -: g) yy=: (i.0     ,d    ){y
  assert. _5 (f -: g) yy=: (?(25    ,d)$#y){y
  assert. _5 (f -: g) yy=: (?(23    ,d)$#y){y
  assert. _5 (f -: g) yy=: (?(3     ,d)$#y){y
  assert. _5 (f -: g) yy=: (i.0     ,d    ){y
  assert. _  (f -: g) yy=: (?(23    ,d)$#y){y
  assert. _  (f -: g) yy=: (i.0     ,d    ){y
  assert. __ (f -: g) yy=: (?(23    ,d)$#y){y
  assert. __ (f -: g) yy=: (i.0     ,d    ){y
 end.
 1
)

+  test 0 1  NB. boolean
<. test 0 1
>. test 0 1
+. test 0 1
*. test 0 1
=  test 0 1
~: test 0 1

+  test ?1000$1e3  NB. small integer
<. test ?1000$1e3
>. test ?1000$1e3

+  test ?1000$2e8  NB. large integer
<. test ?1000$2e8
>. test ?1000$2e8

+  test 256%~?$~5000  NB. floating point
>. test 256%~?$~5000
<. test 256%~?$~5000

+  test 256%~j./?2 1000$5000  NB. complex

+  test 1000 ?@$ 1e9
+  test 1000 ?@$ 2e9
+  test (1000 ?@$ 2e9)* _1^1000 ?@$ 2

sum=: 3 : '+/y'

12 (sum\ -: +/\) _  (?100)} ?100  $1e5
12 (sum\ -: +/\) _  (?100)} ?100 3$1e5
12 (sum\ -: +/\) __ (?100)} ?100  $1e5
12 (sum\ -: +/\) __ (?100)} ?100 3$1e5

3 (sum\ -: +/\) 675365485 435227401 325776289 990265772 435227401 852965599

4 = type 12 +/\ ?100$1e5


4!:55 ;:'f g mm sum test yy'


