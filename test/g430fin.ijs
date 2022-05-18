prolog './g430fin.ijs'
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
+  test - 1000 ?@$ 2e9
+  test (1000 ?@$ 2e9)* _1^1000 ?@$ 2

sum=: 3 : '+/y'

12 (sum\ -: +/\) _  (?100)} ?100  $1e5
12 (sum\ -: +/\) _  (?100)} ?100 3$1e5
12 (sum\ -: +/\) __ (?100)} ?100  $1e5
12 (sum\ -: +/\) __ (?100)} ?100 3$1e5

3 (sum\ -: +/\) 675365485 435227401 325776289 990265772 435227401 852965599

4 = type 12 +/\ ?100$1e5

(,1) -: 1 +/\ 1
(,1) -: 1 ^/\ 1
(,1) -: 1 undefname/\ 1

NB. Verify proper operation of condrange
cr =: 4 : 0
min =. <./ y , 0 { x
max =. >./ y , 1 { x
if. (max-min) >: 2 { x do.
 0 0
else.
 min,1+max-min
end.
)
(imax,imin,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,_500 + ?~ 1000   NB.  starting min, starting max, max range
(0,0,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,_500 + ?~ 1000
(_10000 _10000,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,_500 + ?~ 1000
(10000 10000,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,_500 + ?~ 1000
(imax,imin,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,5 * _500 + ?~ 1000
(imax,imin,imax)&(cr -: 13!:80)\. yy
(imax,imin,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,5 * _5000 + ?~ 1000
(imax,imin,imax)&(cr -: 13!:80)\. yy
(imax,imin,imax)&(cr -: 13!:80)\ yy =: (i. 64) , 2000 _2000,5 * 5000 + ?~ 1000
(imax,imin,imax)&(cr -: 13!:80)\. yy
((imax,imin),"1 0 ] 3990 +i. 20) (cr -: 13!:80)"1 yy
_3000 3000 10000 (cr -: 13!:80) yy
_3000 3000 10000 (cr -: 13!:80) }. yy

NB. Early exit
yy1 =: (12#00) , 2000 , (1e7$00)
yy2 =:  (12#00) , 2000 0 0 0 0 ,~ (1e7$00)
yy3 =:   (12#00) , (2000 , 64#00) ,~ (1e7$00)
(10 (6!:2) '(imax,imin,1000) 13!:80 yy3') > 5 * (10 (6!:2) '(imax,imin,1000) 13!:80 yy1')
3 : 0 ''
if. -. 'avx2' +./@:E. 9!:14'' do. 1 return. end.
(10 (6!:2) '(imax,imin,1000) 13!:80 yy3') > 5 * (10 (6!:2) '(imax,imin,1000) 13!:80 yy2')
)

(16b7fffffff,0,imax)&((cr 3&u:) -: 13!:81)\ yy =: 10 u: (i. 64) , 2000 12000, ?~ 100
(16b7fffffff,0,imax)&((cr 3&u:) -: 13!:81)\. yy
((16b7fffffff,0),"1 0 ] 3990 +i. 20) ((cr 3&u:) -: 13!:81)"1 yy
_3000 3000 10000 ((cr 3&u:) -: 13!:81) yy
_3000 3000 10000 ((cr 3&u:) -: 13!:81) }. yy

(16b7fffffff,0,imax)&((cr 3&u:) -: 13!:82)\ yy =: 4 u: (i. 64) , 2000 12000,?~ 100
(16b7fffffff,0,imax)&((cr 3&u:) -: 13!:82)\. yy
((16b7fffffff,0),"1 0 ] 3990 +i. 20) ((cr 3&u:) -: 13!:82)"1 yy
_3000 3000 10000 ((cr 3&u:) -: 13!:82) yy
_3000 3000 10000 ((cr 3&u:) -: 13!:82) }. yy

4!:55 ;:'cr f g mm sum test yy yy1 yy2 yy3'



epilog''

