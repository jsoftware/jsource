NB. x (+/ % #)\ y -------------------------------------------------------

avg=: 3 : '(+/y)%#y'

test=: 3 : 0
 m=: 1+?50
 for_c. (i.0);3;2 3 do.
  d=. >c
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(500  ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(254  ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(255  ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(256  ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(257  ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(258  ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?(m    ,d)$#y){y
  assert. m  (avg\ -: (+/ % #)\) yy=: (?((m-1),d)$#y){y
  assert. 1  (avg\ -: (+/ % #)\) yy=: (?(20   ,d)$#y){y
  assert. 1  (avg\ -: (+/ % #)\) yy=: (?(1    ,d)$#y){y
  assert. 0  (avg\ -: (+/ % #)\) yy=: (?(20   ,d)$#y){y
  assert. 0  (avg\ -: (+/ % #)\) yy=: (i.0    ,d    ){y
  assert. _5 (avg\ -: (+/ % #)\) yy=: (?(25   ,d)$#y){y
  assert. _5 (avg\ -: (+/ % #)\) yy=: (?(23   ,d)$#y){y
  assert. _5 (avg\ -: (+/ % #)\) yy=: (?(3    ,d)$#y){y
  assert. _5 (avg\ -: (+/ % #)\) yy=: (i.0    ,d    ){y
  assert. _  (avg\ -: (+/ % #)\) yy=: (?(23   ,d)$#y){y
  assert. _  (avg\ -: (+/ % #)\) yy=: (i.0    ,d    ){y
  assert. __ (avg\ -: (+/ % #)\) yy=: (?(23   ,d)$#y){y
  assert. __ (avg\ -: (+/ % #)\) yy=: (i.0    ,d    ){y
 end.
 1
)

test 0 1                   NB. boolean
test ?$~1000               NB. integer
test 256%~?$~5000          NB. floating point
test 256%~j./?2 1000$5000  NB. complex

12 (avg\ -: (+/%#)\) _  (?100)} ?100  $1e5
12 (avg\ -: (+/%#)\) _  (?100)} ?100 3$1e5
12 (avg\ -: (+/%#)\) __ (?100)} ?100  $1e5
12 (avg\ -: (+/%#)\) __ (?100)} ?100 3$1e5


4!:55 ;:'avg m test yy'


