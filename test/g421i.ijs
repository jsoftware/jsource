prolog './g421i.ijs'
NB.  x f//. y special code for + +. *. >. <. = ~: -----------------------

randuni''
bigval =: imax-1000
test=: 1 : 0
 n=. 1000
 v=: 3 : ((5!:5 <'u'),'/y')
 yy=: y {~ n ?@$ #y
 assert. (xx=:      ? n   $2    ) (u//. -: v/.) yy
 assert. (xx=:      ?(n,2)$2    ) (u//. -: v/.) yy
 assert. (xx=:      ?(n,3)$2    ) (u//. -: v/.) yy
 assert. (xx=:  a.{~? n   $#a.  ) (u//. -: v/.) yy
 assert. (xx=:  a.{~?(n,2)$#a.  ) (u//. -: v/.) yy
 assert. (xx=:  a.{~?(n,3)$#a.  ) (u//. -: v/.) yy
 assert. (xx=:      ? n   $100  ) (u//. -: v/.) yy
 assert. (xx=: _50 +? n   $100  ) (u//. -: v/.) yy
 assert. (xx=: _5e9+? n   $100  ) (u//. -: v/.) yy
 assert. (xx=:      ? n   $bigval  ) (u//. -: v/.) yy
 assert. (xx=:      ?(n,2)$100  ) (u//. -: v/.) yy
 assert. (xx=:    u:? n   $65536) (u//. -: v/.) yy
 assert. (xx=: 10&u: RAND32 ? n $C4MAX) (u//. -: v/.) yy
 assert. (xx=: sdot0{~? n $#sdot0) (u//. -: v/.) yy
 1
)

+  test 0 1
+  test 0=?100$2
+  test 0<?100$2
+  test        ?100$1000
+  test   _500+?100$1000
+  test        ?100$bigval
+  test   _5e8+?100$bigval
+  test 1 _1 _2147483648 2147483647
+  test -:_5e8+?100$bigval

<. test 0 1
<. test 0=?100$2
<. test 0<?100$2
<. test        ?100$1000
<. test   _500+?100$1000
<. test        ?100$bigval
<. test   _5e8+?100$bigval
<. test 1 _1 _2147483648 2147483647
<. test -:_5e8+?100$bigval

>. test 0 1
>. test 0=?100$2
>. test 0<?100$2
>. test        ?100$1000
>. test   _500+?100$1000
>. test        ?100$bigval
>. test   _5e8+?100$bigval
>. test 1 _1 _2147483648 2147483647
>. test -:_5e8+?100$bigval

*. test 0 1
*. test 0=?100$2
*. test 0<?100$2
*  test 0 1
*  test 0=?100$2
*  test 0<?100$2

+. test 0 1
+. test 0=?100$2
+. test 0<?100$2

=  test 0 1
=  test 0=?100$2
=  test 0<?100$2

~: test 0 1
~: test 0=?100$2
~: test 0<?100$2

17 b. test        ?100$1000
17 b. test   _500+?100$1000
17 b. test        ?100$bigval
17 b. test   _5e8+?100$bigval
17 b. test 1 _1 _2147483648 2147483647
17 b. test -~2 2
17 b. test _1 _1

22 b. test        ?100$1000
22 b. test   _500+?100$1000
22 b. test        ?100$bigval
22 b. test   _5e8+?100$bigval
22 b. test 1 _1 _2147483648 2147483647
22 b. test -~2 2
22 b. test _1 _1

23 b. test        ?100$1000
23 b. test   _500+?100$1000
23 b. test        ?100$bigval
23 b. test   _5e8+?100$bigval
23 b. test 1 _1 _2147483648 2147483647
23 b. test -~2 2
23 b. test _1 _1

25 b. test        ?100$1000
25 b. test   _500+?100$1000
25 b. test        ?100$bigval
25 b. test   _5e8+?100$bigval
25 b. test 1 _1 _2147483648 2147483647
25 b. test -~2 2
25 b. test _1 _1

x=: 7183 2 ?@$ 61
x (+.//. -: 3 : '+./y'/.) y=:   ?($x)$2
x (+.//. -: 3 : '+./y'/.) y=: 0=?($x)$100
x (+.//. -: 3 : '+./y'/.) y=: 0<?($x)$100
x (+.//. -: 3 : '+./y'/.) y=:   ?($x)$1e6
x (+.//. -: 3 : '+./y'/.) y=: o.?($x)$1e6

x (*.//. -: 3 : '*./y'/.) y=:   ?($x)$2
x (*.//. -: 3 : '*./y'/.) y=: 0=?($x)$100
x (*.//. -: 3 : '*./y'/.) y=: 0<?($x)$100
x (*.//. -: 3 : '*./y'/.) y=:   ?($x)$1e6
x (*.//. -: 3 : '*./y'/.) y=: o.?($x)$1e6

x (<.//. -: 3 : '<./y'/.) y=:   ?($x)$2
x (<.//. -: 3 : '<./y'/.) y=: 0=?($x)$100
x (<.//. -: 3 : '<./y'/.) y=: 0<?($x)$100
x (<.//. -: 3 : '<./y'/.) y=:   ?($x)$1e6
x (<.//. -: 3 : '<./y'/.) y=: o.?($x)$1e6

x (>.//. -: 3 : '>./y'/.) y=:   ?($x)$2
x (>.//. -: 3 : '>./y'/.) y=: 0=?($x)$100
x (>.//. -: 3 : '>./y'/.) y=: 0<?($x)$100
x (>.//. -: 3 : '>./y'/.) y=:   ?($x)$1e6
x (>.//. -: 3 : '>./y'/.) y=: o.?($x)$1e6

x=: sdot0{~ 7183 2 ?@$ #sdot0
x (<.//. -: 3 : '<./y'/.) y=: sdot0{~  ?($x)$$sdot0

x (>.//. -: 3 : '>./y'/.) y=: sdot0{~  ?($x)$$sdot0

'domain error' -: x + //. etx (#x)$ 'abc' 
'domain error' -: x + //. etx (#x)$<'abc' 
'domain error' -: x +.//. etx (#x)$ 'abc' 
'domain error' -: x +.//. etx (#x)$<'abc' 
'domain error' -: x *.//. etx (#x)$ 'abc' 
'domain error' -: x *.//. etx (#x)$<'abc' 
'domain error' -: x >.//. etx (#x)$ 'abc' 
'domain error' -: x >.//. etx (#x)$<'abc' 
'domain error' -: x <.//. etx (#x)$ 'abc' 
'domain error' -: x <.//. etx (#x)$<'abc' 

'length error' -: 1 2 3 + //. etx 0 1
'length error' -: 1 2 3 + //. etx 4 5
'length error' -: 1 2 3 + //. etx 4 5.6
'length error' -: 1 2 3 +.//. etx 0 1
'length error' -: 1 2 3 +.//. etx 4 5
'length error' -: 1 2 3 +.//. etx 4 5.6
'length error' -: 1 2 3 *.//. etx 0 1
'length error' -: 1 2 3 *.//. etx 4 5
'length error' -: 1 2 3 *.//. etx 4 5.6
'length error' -: 1 2 3 <.//. etx 0 1
'length error' -: 1 2 3 <.//. etx 4 5
'length error' -: 1 2 3 <.//. etx 4 5.6
'length error' -: 1 2 3 >.//. etx 0 1
'length error' -: 1 2 3 >.//. etx 4 5
'length error' -: 1 2 3 >.//. etx 4 5.6


randfini''

epilog''

