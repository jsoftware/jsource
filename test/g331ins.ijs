NB. f/;.n ---------------------------------------------------------------

sp =: 7!:2 
spa=: 3 : '7!:5 <''y'''

test=: 1 : 0
 f=: u/
 xx=: ,: 'b=: ?(#yy)$2 [ yy=: y{~?233  $#y'
 xx=: xx,'b=: ?(#yy)$2 [ yy=: y{~?233 5$#y'
 xx=: xx,'b=:  (#yy)$1 [ yy=: y{~?233  $#y'
 xx=: xx,'b=:  (#yy)$1 [ yy=: y{~?233 5$#y'
 for_exp. xx do.
  ".exp
  assert. (b f;. 1     yy) -: b u/;. 1     yy
  assert. (b f;._1 etx yy) -: b u/;._1 etx yy
  assert. (b f;. 2     yy) -: b u/;. 2     yy
  assert. (b f;._2 etx yy) -: b u/;._2 etx yy
 end.
 if. (1=3!:0 y) >: (<5!:5 <'u') e. ;:'= < <: > >: +. * *. ~:' do.
  yy=: y{~?10000$#y
  b=: ?(#yy)$2
  p=: sp  'b u/;.1 yy'
  q=: spa  b u/;.1 yy
  assert. p <: q*IF64{1.5 2 
 end.
 1
)

=  test 0 1
<  test 0 1
<. test 0 1
<: test 0 1
>  test 0 1
>. test 0 1
>: test 0 1
=  test 0 1
+  test 0 1
+. test 0 1
+: test 0 1
-  test 0 1
*  test 0 1
*. test 0 1
*: test 0 1
~: test 0 1

=  test i.2000
<  test i.2000
<. test i.2000
<: test i.2000
>  test i.2000
>. test i.2000
>: test i.2000
=  test i.2000
+  test i.2000
+. test i.2000
-  test i.2000
*  test i.2000
*. test i.2000
~: test i.2000

=  test o.i.2000
<  test o.i.2000
<. test o.i.2000
<: test o.i.2000
>  test o.i.2000
>. test o.i.2000
>: test o.i.2000
=  test o.i.2000
+  test o.i.2000
-  test o.i.2000
*  test o.i.2000
~: test o.i.2000

testb=: 3 : 0
 yy=: y{~?233$#y
 b=: ?(#yy)$2
 c=:  (#yy)$1
 for_i. 16+i.16 do.
  j=: i
  assert. (> i b./&.> b <;.1 yy) -: b i b./;.1 yy
  assert. (> i b./&.> c <;.1 yy) -: c i b./;.1 yy
  t=: y{~?10000$#y
  a=: ?(#t)$2
  p=: sp  'a i b./;.1 t'
  q=: spa  a i b./;.1 t
  assert. p <: 1.5*q 
 end.
 1
)

testb i.2000

1 0 -: $ 1 0 +/;.1 i.2 0


4!:55 ;:'a b c f j p q sp spa t test testb xx yy'


