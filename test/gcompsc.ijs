prolog './gcompsc.ijs'
NB. fork special code for the following cases: --------------------------
NB.    comp i. 0:          i.&0@:comp
NB.    comp i. 1:          i.&1@:comp
NB.    comp i: 0:          i:&0@:comp
NB.    comp i: 1:          i:&1@:comp
NB.    [: + / comp         + /@:comp
NB.    [: +./ comp         +./@:comp
NB.    [: *./ comp         *./@:comp
NB.    [: I.  comp         I. @:comp
NB. where comp is one of the following:
NB.    = ~: < <: >: > E. e.   with optional tolerance

sp=: 7!:2

ftab=: 2 : 0   
 select. u 2 2 0 1 0 1    NB. detect what x is by applying it
  case. 2           do. v i. 0:
  case. 3           do. v i. 1:
  case. 4           do. v i: 0:
  case. 5           do. v i: 1:
  case. 6           do. [: + / v
  case. 1           do. [: +./ v
  case. 0           do. [: *./ v
  case. 0 0 1 1 3 5 do. [: I. v
 end.
)

data=: 3 : 0
 xb=:               10000?@$2   [ yb=:               10000?@$2
 xi=:        _5e5 + 10000?@$1e6 [ yi=:        _5e5 + 10000?@$1e6
 xd=: (*   [: >: 2e_13 * _0.5 + $ ?@$ 0:) 0.01 * _5e5 + 10000?@$1e6 [ yd=: (*   [: >: 2e_13 * _0.5 + $ ?@$ 0:) 0.01 * _5e5 + 10000?@$1e6  NB. Have some overlap if tolerant
 xs=: s: ' ',": 1000?@$4000    [ ys=: s: ' ',":1000?@$4000
 ai=:        _5e8 + ?1e9
 ad=: 0.01 * _5e8 + ?1e9
 as=: {. s: ":?4000
 zb=: 0$0
 zi=: 0$2
 zd=: 0$2.3
 zs=: 0$as
 xb1=: 1001?@$2 [ yb1=: 1001?@$2
 xb2=: 1002?@$2 [ yb2=: 1002?@$2
 xb3=: 1003?@$2 [ yb3=: 1003?@$2
 xb4=: 1004?@$2 [ yb4=: 1004?@$2
 xb5=: 1005?@$2 [ yb5=: 1005?@$2
 xb6=: 1006?@$2 [ yb6=: 1006?@$2
 xb7=: 1007?@$2 [ yb7=: 1007?@$2
 i.0 0
)

testvv=: 2 : 0  NB. vector vector
  :
 ri =: (210+i. 40) # 10
 for_c. i: 100 do.   assert. (u (5 c}ri) v (5 c}ri)) -: (5 c}ri) (u f.)@:(v f.) (5 c}ri) [ cc =: c end.
 assert. (u xb v yb) -: xb (u f.)@:(v f.) yb
 assert. (u xb v yi) -: xb (u f.)@:(v f.) yi
 assert. (u xb v yd) -: xb (u f.)@:(v f.) yd
 assert. (u xi v yb) -: xi (u f.)@:(v f.) yb
 assert. (u xi v yi) -: xi (u f.)@:(v f.) yi
 assert. (u xi v yd) -: xi (u f.)@:(v f.) yd
 assert. (u xd v yb) -: xd (u f.)@:(v f.) yb
 assert. (u xd v yi) -: xd (u f.)@:(v f.) yi
 assert. (u xd v yd) -: xd (u f.)@:(v f.) yd
 assert. (u xs v ys) -: xs (u f.)@:(v f.) ys
 f=: u ftab v
 assert. (u xb v yb) -: xb f      yb
 assert. (u xb v yi) -: xb f      yi
 assert. (u xb v yd) -: xb f      yd
 assert. (u xi v yb) -: xi f      yb
 assert. (u xi v yi) -: xi f      yi
 assert. (u xi v yd) -: xi f      yd
 assert. (u xd v yb) -: xd f      yb
 assert. (u xd v yi) -: xd f      yi
 assert. (u xd v yd) -: xd f      yd
 assert. (u xs v ys) -: xs f      ys
 1
)

testsv=: 2 : 0  NB. scalar vector
  :
 ri =: (210+i. 40) # 10
 for_c. i: 100 do.   assert. (u 5 v (5 c}ri)) -: 5 (u f.)@:(v f.) (5 c}ri) [ cc =: c end.
 assert. (u 0  v zb) -: 0  (u f.)@:(v f.) zb
 assert. (u 0  v zi) -: 0  (u f.)@:(v f.) zi
 assert. (u 0  v zd) -: 0  (u f.)@:(v f.) zd
 assert. (u 0  v yb) -: 0  (u f.)@:(v f.) yb
 assert. (u 0  v yi) -: 0  (u f.)@:(v f.) yi
 assert. (u 0  v yd) -: 0  (u f.)@:(v f.) yd
 assert. (u 1  v zb) -: 1  (u f.)@:(v f.) zb
 assert. (u 1  v zi) -: 1  (u f.)@:(v f.) zi
 assert. (u 1  v zd) -: 1  (u f.)@:(v f.) zd
 assert. (u 1  v yb) -: 1  (u f.)@:(v f.) yb
 assert. (u 1  v yi) -: 1  (u f.)@:(v f.) yi
 assert. (u 1  v yd) -: 1  (u f.)@:(v f.) yd
 assert. (u ai v yb) -: ai (u f.)@:(v f.) yb
 assert. (u ai v yi) -: ai (u f.)@:(v f.) yi
 assert. (u ai v yd) -: ai (u f.)@:(v f.) yd
 assert. (u ad v yb) -: ad (u f.)@:(v f.) yb
 assert. (u ad v yi) -: ad (u f.)@:(v f.) yi
 assert. (u ad v yd) -: ad (u f.)@:(v f.) yd
 assert. (u as v ys) -: as (u f.)@:(v f.) ys
 f=: u ftab v
 assert. (u 0  v yb) -: 0  f      yb
 assert. (u 0  v yi) -: 0  f      yi
 assert. (u 0  v yd) -: 0  f      yd
 assert. (u 1  v yb) -: 1  f      yb
 assert. (u 1  v yi) -: 1  f      yi
 assert. (u 1  v yd) -: 1  f      yd
 assert. (u ai v yb) -: ai f      yb
 assert. (u ai v yi) -: ai f      yi
 assert. (u ai v yd) -: ai f      yd
 assert. (u ad v yb) -: ad f      yb
 assert. (u ad v yi) -: ad f      yi
 assert. (u ad v yd) -: ad f      yd
 assert. (u as v ys) -: as f      ys
 1
)

testvs=: 2 : 0  NB. vector scalar
  :
 ri =: (210+i. 40) # 10
 for_c. i: 100 do.   assert. (u (5 c}ri) v 5) -: (5 c}ri) (u f.)@:(v f.) 5 [ cc =: c end.
 assert. (u xb v 0 ) -: xb (u f.)@:(v f.) 0
 assert. (u xb v 1 ) -: xb (u f.)@:(v f.) 1
 assert. (u xb v ai) -: xb (u f.)@:(v f.) ai
 assert. (u xb v ad) -: xb (u f.)@:(v f.) ad
 assert. (u xi v 0 ) -: xi (u f.)@:(v f.) 0
 assert. (u xi v 1 ) -: xi (u f.)@:(v f.) 1
 assert. (u xi v ai) -: xi (u f.)@:(v f.) ai
 assert. (u xi v ad) -: xi (u f.)@:(v f.) ad
 assert. (u xd v 0 ) -: xd (u f.)@:(v f.) 0
 assert. (u xd v 1 ) -: xd (u f.)@:(v f.) 1
 assert. (u xd v ai) -: xd (u f.)@:(v f.) ai
 assert. (u xd v ad) -: xd (u f.)@:(v f.) ad
 assert. (u xs v as) -: xs (u f.)@:(v f.) as
 f=: u ftab v
 assert. (u xb v 0 ) -: xb f      0
 assert. (u xb v 1 ) -: xb f      1
 assert. (u xb v ai) -: xb f      ai
 assert. (u xb v ad) -: xb f      ad
 assert. (u xi v 0 ) -: xi f      0
 assert. (u xi v 1 ) -: xi f      1
 assert. (u xi v ai) -: xi f      ai
 assert. (u xi v ad) -: xi f      ad
 assert. (u xd v 0 ) -: xd f      0
 assert. (u xd v 1 ) -: xd f      1
 assert. (u xd v ai) -: xd f      ai
 assert. (u xd v ad) -: xd f      ad
 assert. (u xs v as) -: xs f      as
 1
)

testss=: 2 : 0  NB. scalar scalar
  :
 assert. (u 0  v 0 ) -: 0  (u f.)@:(v f.) 0
 assert. (u 0  v 1 ) -: 0  (u f.)@:(v f.) 1
 assert. (u 0  v ai) -: 0  (u f.)@:(v f.) ai
 assert. (u 0  v ad) -: 0  (u f.)@:(v f.) ad
 assert. (u 1  v 0 ) -: 1  (u f.)@:(v f.) 0
 assert. (u 1  v 1 ) -: 1  (u f.)@:(v f.) 1
 assert. (u 1  v ai) -: 1  (u f.)@:(v f.) ai
 assert. (u 1  v ad) -: 1  (u f.)@:(v f.) ad
 assert. (u ai v 0 ) -: ai (u f.)@:(v f.) 0
 assert. (u ai v 1 ) -: ai (u f.)@:(v f.) 1
 assert. (u ai v ai) -: ai (u f.)@:(v f.) ai
 assert. (u ai v ad) -: ai (u f.)@:(v f.) ad
 assert. (u ad v 0 ) -: ad (u f.)@:(v f.) 0
 assert. (u ad v 1 ) -: ad (u f.)@:(v f.) 1
 assert. (u ad v ai) -: ad (u f.)@:(v f.) ai
 assert. (u ad v ad) -: ad (u f.)@:(v f.) ad
 assert. (u as v as) -: as (u f.)@:(v f.) as
 f=: u ftab v
 assert. (u 0  v 0 ) -: 0  f      0
 assert. (u 0  v 1 ) -: 0  f      1
 assert. (u 0  v ai) -: 0  f      ai
 assert. (u 0  v ad) -: 0  f      ad
 assert. (u 1  v 0 ) -: 1  f      0
 assert. (u 1  v 1 ) -: 1  f      1
 assert. (u 1  v ai) -: 1  f      ai
 assert. (u 1  v ad) -: 1  f      ad
 assert. (u ai v 0 ) -: ai f      0
 assert. (u ai v 1 ) -: ai f      1
 assert. (u ai v ai) -: ai f      ai
 assert. (u ai v ad) -: ai f      ad
 assert. (u ad v 0 ) -: ad f      0
 assert. (u ad v 1 ) -: ad f      1
 assert. (u ad v ai) -: ad f      ai
 assert. (u ad v ad) -: ad f      ad
 assert. (u as v as) -: as f      as
 1
)

testbvv=: 2 : 0  NB. boolean vector vector
  :
 assert. (u xb  v yb ) -: xb  (u f.)@:(v f.) yb
 assert. (u xb1 v yb1) -: xb1 (u f.)@:(v f.) yb1
 assert. (u xb2 v yb2) -: xb2 (u f.)@:(v f.) yb2
 assert. (u xb3 v yb3) -: xb3 (u f.)@:(v f.) yb3
 assert. (u xb4 v yb4) -: xb4 (u f.)@:(v f.) yb4
 assert. (u xb5 v yb5) -: xb5 (u f.)@:(v f.) yb5
 assert. (u xb6 v yb6) -: xb6 (u f.)@:(v f.) yb6
 assert. (u xb7 v yb7) -: xb7 (u f.)@:(v f.) yb7
 f=: u ftab v
 assert. (u xb  v yb ) -: xb  f      yb
 assert. (u xb1 v yb1) -: xb1 f      yb1
 assert. (u xb2 v yb2) -: xb2 f      yb2
 assert. (u xb3 v yb3) -: xb3 f      yb3
 assert. (u xb4 v yb4) -: xb4 f      yb4
 assert. (u xb5 v yb5) -: xb5 f      yb5
 assert. (u xb6 v yb6) -: xb6 f      yb6
 assert. (u xb7 v yb7) -: xb7 f      yb7
 1
)

testbsv=: 2 : 0  NB. boolean scalar vector
  :
 assert. (u 0 v zb  ) -: 0 (u f.)@:(v f.) zb   
 assert. (u 0 v yb  ) -: 0 (u f.)@:(v f.) yb   
 assert. (u 0 v yb1 ) -: 0 (u f.)@:(v f.) yb1
 assert. (u 0 v yb2 ) -: 0 (u f.)@:(v f.) yb2
 assert. (u 0 v yb3 ) -: 0 (u f.)@:(v f.) yb3
 assert. (u 0 v yb4 ) -: 0 (u f.)@:(v f.) yb4
 assert. (u 0 v yb5 ) -: 0 (u f.)@:(v f.) yb5
 assert. (u 0 v yb6 ) -: 0 (u f.)@:(v f.) yb6
 assert. (u 0 v yb7 ) -: 0 (u f.)@:(v f.) yb7
 assert. (u 1 v 0$yb) -: 1 (u f.)@:(v f.) 0$yb
 assert. (u 1 v yb  ) -: 1 (u f.)@:(v f.) yb
 assert. (u 1 v yb1 ) -: 1 (u f.)@:(v f.) yb1
 assert. (u 1 v yb2 ) -: 1 (u f.)@:(v f.) yb2
 assert. (u 1 v yb3 ) -: 1 (u f.)@:(v f.) yb3
 assert. (u 1 v yb4 ) -: 1 (u f.)@:(v f.) yb4
 assert. (u 1 v yb5 ) -: 1 (u f.)@:(v f.) yb5
 assert. (u 1 v yb6 ) -: 1 (u f.)@:(v f.) yb6
 assert. (u 1 v yb7 ) -: 1 (u f.)@:(v f.) yb7
 f=: u ftab v
 assert. (u 0 v zb   ) -: 0 f      zb   
 assert. (u 0 v yb   ) -: 0 f      yb   
 assert. (u 0 v yb1  ) -: 0 f      yb1
 assert. (u 0 v yb2  ) -: 0 f      yb2
 assert. (u 0 v yb3  ) -: 0 f      yb3
 assert. (u 0 v yb4  ) -: 0 f      yb4
 assert. (u 0 v yb5  ) -: 0 f      yb5
 assert. (u 0 v yb6  ) -: 0 f      yb6
 assert. (u 0 v yb7  ) -: 0 f      yb7
 assert. (u 1 v zb   ) -: 1 f      zb
 assert. (u 1 v yb   ) -: 1 f      yb
 assert. (u 1 v yb1  ) -: 1 f      yb1
 assert. (u 1 v yb2  ) -: 1 f      yb2
 assert. (u 1 v yb3  ) -: 1 f      yb3
 assert. (u 1 v yb4  ) -: 1 f      yb4
 assert. (u 1 v yb5  ) -: 1 f      yb5
 assert. (u 1 v yb6  ) -: 1 f      yb6
 assert. (u 1 v yb7  ) -: 1 f      yb7
 1
)

testbvs=: 2 : 0  NB. boolean vector scalar
  :
 assert. (u zb     v 0) -: zb     (u f.)@:(v f.) 0
 assert. (u xb     v 0) -: xb     (u f.)@:(v f.) 0
 assert. (u xb1    v 0) -: xb1    (u f.)@:(v f.) 0
 assert. (u xb2    v 0) -: xb2    (u f.)@:(v f.) 0
 assert. (u xb3    v 0) -: xb3    (u f.)@:(v f.) 0
 assert. (u xb4    v 0) -: xb4    (u f.)@:(v f.) 0
 assert. (u xb5    v 0) -: xb5    (u f.)@:(v f.) 0
 assert. (u xb6    v 0) -: xb6    (u f.)@:(v f.) 0
 assert. (u xb7    v 0) -: xb7    (u f.)@:(v f.) 0
 assert. (u xb     v 1) -: xb     (u f.)@:(v f.) 1
 assert. (u xb1    v 1) -: xb1    (u f.)@:(v f.) 1
 assert. (u xb2    v 1) -: xb2    (u f.)@:(v f.) 1
 assert. (u xb3    v 1) -: xb3    (u f.)@:(v f.) 1
 assert. (u xb4    v 1) -: xb4    (u f.)@:(v f.) 1
 assert. (u xb5    v 1) -: xb5    (u f.)@:(v f.) 1
 assert. (u xb6    v 1) -: xb6    (u f.)@:(v f.) 1
 assert. (u xb7    v 1) -: xb7    (u f.)@:(v f.) 1
 f=: u ftab v
 assert. (u zb     v 0) -: zb      f      0
 assert. (u xb1    v 0) -: xb1     f      0
 assert. (u xb2    v 0) -: xb2     f      0
 assert. (u xb3    v 0) -: xb3     f      0
 assert. (u xb4    v 0) -: xb4     f      0
 assert. (u xb5    v 0) -: xb5     f      0
 assert. (u xb6    v 0) -: xb6     f      0
 assert. (u xb7    v 0) -: xb7     f      0
 assert. (u xb     v 1) -: xb      f      1
 assert. (u xb1    v 1) -: xb1     f      1
 assert. (u xb2    v 1) -: xb2     f      1
 assert. (u xb3    v 1) -: xb3     f      1
 assert. (u xb4    v 1) -: xb4     f      1
 assert. (u xb5    v 1) -: xb5     f      1
 assert. (u xb6    v 1) -: xb6     f      1
 assert. (u xb7    v 1) -: xb7     f      1
 1
)

NB. test that special code is invoked by looking at space used
NB. Space may not match exactly because sc() sometimes consumes 0 bytes
SPTOLER =: (IGNOREIFFVI ((IF64 { 64 128)&>:))

testsp=: 2 : 0
 if. 'I.'-: 5!:5 <'u' do. 1 return. end.
 expression=:  4#,:'sp ''xx (u f.)@:(v f.) yy'' [ xx=. ,~xx [ yy=. ,~yy'
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xb [ yy=. yb
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xb [ yy=. yi
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xb [ yy=. yd
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xi [ yy=. yb
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xi [ yy=. yi
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xi [ yy=. yd
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xd [ yy=. yb
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xd [ yy=. yi
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xd [ yy=. yd
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xs [ yy=. ys
 f=: u ftab v
 expression=:  4#,:'sp ''xx f yy'' [ xx=. ,~xx [ yy=. ,~yy'
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xb [ yy=. yb
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xb [ yy=. yi
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xb [ yy=. yd
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xi [ yy=. yb
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xi [ yy=. yi
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xi [ yy=. yd
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xd [ yy=. yb
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xd [ yy=. yi
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xd [ yy=. yd
 assert. (SPTOLER >./ - <./) ".expression [ xx=. xs [ yy=. ys
 1
)

test=: 2 : 0
 u testvv  v
 u testsv  v
 u testvs  v
 u testss  v
 u testbvv v
 u testbsv v
 u testbvs v
 u testsp  v
 u testvv  (v f.!.0)
 u testsv  (v f.!.0)
 u testvs  (v f.!.0)
 u testss  (v f.!.0)
 u testbvv (v f.!.0)
 u testbsv (v f.!.0)
 u testbvs (v f.!.0)
 u testsp  (v f.!.0)
 1
)

data ''

i.&0 test <
i.&0 test <:
i.&0 test =
i.&0 test ~:
i.&0 test >:
i.&0 test >

i.&1 test <
i.&1 test <:
i.&1 test =
i.&1 test ~:
i.&1 test >:
i.&1 test >

i:&0 test <
i:&0 test <:
i:&0 test =
i:&0 test ~:
i:&0 test >:
i:&0 test >

i:&1 test <
i:&1 test <:
i:&1 test =
i:&1 test ~:
i:&1 test >:
i:&1 test >

+ /  test <
+ /  test <:
+ /  test =
+ /  test ~:
+ /  test >:
+ /  test >

+./  test <
+./  test <:
+./  test =
+./  test ~:
+./  test >:
+./  test >

*./  test <
*./  test <:
*./  test =
*./  test ~:
*./  test >:
*./  test >

I.   test <
I.   test <:
I.   test =
I.   test ~:
I.   test >:
I.   test >


testc=: 2 : 0  NB. character
 xx=: a.{~ ?400$#a.
 yy=: a.{~ ?400$#a.
 xa=: a.{~ ?     #a.
 ya=: a.{~ ?     #a.
 assert. (u xx v yy) -: xx (u f.)@:(v f.) yy
 assert. (u xx v ya) -: xx (u f.)@:(v f.) ya
 assert. (u xa v yy) -: xa (u f.)@:(v f.) yy
 assert. (u xa v ya) -: xa (u f.)@:(v f.) ya
 f=: u ftab v
 assert. (u xx v yy) -: xx f      yy
 assert. (u xx v ya) -: xx f      ya
 assert. (u xa v yy) -: xa f      yy
 assert. (u xa v ya) -: xa f      ya
 if. -.'I.'-: 5!:5 <'u' do.
  assert. (SPTOLER >./ - <./) t=: ".4#,:'sp ''xx (u f.)@:(v f.) yy'' [ xx=: ,~xx [ yy=: ,~yy' 
  assert. (SPTOLER >./ - <./) t=: ".4#,:'sp ''xx f yy''    [ xx=: ,~xx [ yy=: ,~yy'
 end.
 1
)

i.&0 testc =
i.&0 testc ~:
i.&1 testc =
i.&1 testc ~:
i:&0 testc =
i:&0 testc ~:
i:&1 testc =
i:&1 testc ~:
+ /  testc =
+ /  testc ~:
+./  testc =
+./  testc ~:
*./  testc =
*./  testc ~:
I.   testc =
I.   testc ~:

testE=: 2 : 0
 xb=: 7  ?@$2
 yb=: 1e5?@$2
 xi=: 4  ?@$5
 yi=: 1e5?@$5
 xj=: 4  ?@$1e6
 yj=: xj (<"0(?3$9e4)+/i.#xj)}1e5?@$1e6
 xc=: 'abcd'{~ 4  ?@$4
 yc=: 'abcd'{~ 1e5?@$4
 assert. (u xb v yb) -: xb (u f.)@:(v f.) yb
 assert. (u xc v yc) -: xc (u f.)@:(v f.) yc
 assert. (u xi v yi) -: xi (u f.)@:(v f.) yi
 assert. (u xj v yj) -: xj (u f.)@:(v f.) yj
 f=: u ftab v
 assert. (u xb v yb) -: xb f      yb
 assert. (u xc v yc) -: xc f      yc
 assert. (u xi v yi) -: xi f      yi
 assert. (u xj v yj) -: xj f      yj
 if. -.'I.'-: 5!:5 <'u' do.
  expression=: 4#,:'sp ''xx (u f.)@:(v f.) yy'' [ xx=: ,~xx [ yy=: ,~yy'
  assert. (SPTOLER >./ - <./) ". expression [ xx=: xb [ yy=: yb
  assert. (SPTOLER >./ - <./) ". expression [ xx=: xc [ yy=: yc
  assert. (SPTOLER >./ - <./) ". expression [ xx=: xi [ yy=: yi
  expression=: 4#,:'sp ''xx f yy''      [ xx=: ,~xx [ yy=: ,~yy'
  assert. (SPTOLER >./ - <./) ". expression [ xx=: xb [ yy=: yb
  assert. (SPTOLER >./ - <./) ". expression [ xx=: xc [ yy=: yc
  assert. (SPTOLER >./ - <./) ". expression [ xx=: xi [ yy=: yi
 end.
 1
)

+ /  testE E.
+./  testE E.
i.&1 testE E.
I.   testE E.

NB. Verify excluded-rank cases work
2 2 2 2 2 -: (i.4 5) +/@e. i. 10
10 -: (i. 10) i.&0@:= (i. 10 5)
10 -: (i. 1 10) i."_&0@:E. (i. 10 5)

NB. Verify reversion for other types
4 -: (i. 10x) +/@:> 5x
4 -: (i. 10x) +/@:> 11r2
4 -: 'abcdefegehie' +/@:= 'e'
'domain error' -: 'abcdefegehie' +/@:> etx 'e'

NB. Verify toler honored in compound
10 = 1 +/@:(=!.1e_12) 1 + 1e_14 * i. 10
1 = 1 +/@:(=!.1e_16) 1 + 1e_14 * i. 10
0 = # 1 -.!.1e_12~ 1 + 1e_14 * i. 10
9 = # 1 -.!.1e_16~ 1 + 1e_14 * i. 10
10 = # 1 ([ -.!.0 -.!.1e_12)~ 1 + 1e_14 * i. 10
1 = # 1 ([ -.!.0 -.!.1e_16)~ 1 + 1e_14 * i. 10

yy =: 1e7 $ 1 + 1e_14  NB. Must be big so that temp hashtable is released
(#yy) = yy +/@:(=!.1e_12) yy
10000 > 7!:2 'yy +/@:(=!.1e_12) yy'
f =: +/@:(e.!.1e_12)&yy
(#yy) = f yy
10000 > 7!:2 'f yy'
f =: -.!.1e_12&yy
0 = f yy
xx =: (#yy) $ 1.0
0 = # yy -.!.1e_12 xx
(#yy) = # yy -.!.1e_16 xx
0 = # -.!.1e_12&xx yy
(#yy) = # -.!.1e_16&xx yy
t =: 7!:2 'f yy'
(-.IF64) +. (7!:2 'yy  -.!.1e_12 yy') > 1.5 * t
(#yy) = # yy ([ -. -.!.1e_12) xx
0 = # yy ([ -.!.0 -.!.1e_16) xx
(#yy) = # ([ -.!.0 -.!.1e_12)&xx yy
0 = # ([ -.!.0 -.!.1e_16)&xx yy
(-.IF64) +. (7!:2 'yy ([ -. -.!.1e_16) yy') > 1.5 * t

NB. empty-arg detection
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) '' ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) (0 $ 0) ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) (0 $ 0) ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) (0 $ 00) ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) (0 $ 00) ; , 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) (0 $ 00) ; 01
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0 <"0@i.@#)) (0 $ 00) ; , 01
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) '' ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) (0 $ 0) ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) (0 $ 0) ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) (0 $ 00) ; 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) (0 $ 00) ; , 1
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) (0 $ 00) ; 01
((+/"]@:= S: 0 <"0@i.@#) -: (+/@:= S: 0~ <"0@i.@#)) (0 $ 00) ; , 01


4!:55 ;:'ad ai as cc data expression f ftab ri sp SPTOLER'
4!:55 ;:'t test testbsv testbvs testbvv testc testE testsp'
4!:55 ;:'testss testsv testvs testvv'
4!:55 ;:'xa xb xb1 xb2 xb3 xb4 xb5 xb6 xb7 xc xd xi xj xs xx'
4!:55 ;:'ya yb yb1 yb2 yb3 yb4 yb5 yb6 yb7 yc yd yi yj ys yy'
4!:55 ;:'zb zd zi zs'



epilog''

