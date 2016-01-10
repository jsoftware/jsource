NB. x ({.,#)/. y and x (#,{.)/.y ----------------------------------------

f=: ~:@[ # ]
g=: #/.

test=: 3 : 0
 y testa 0 1
 y testa      i.900
 y testa _450+i.900
 y testa o.   i.900
 y testa x:   i.900
)

testa=: 4 : 0
 yy=:y{~?1000$#y
 assert. xx (({.,#)/. -: (f,.g)) yy [ xx=:x{~?1000$#x
 assert. xx ((#,{.)/. -: (g,.f)) yy
 if. 1:@:>: :: 0: x do.
  assert. xx (({.,#)/. -: (f,.g)) xx
  assert. xx ((#,{.)/. -: (g,.f)) xx
 end.
 assert. xx (({.,#)/. -: (f,.g)) yy [ xx=:x{~ 1000$0
 assert. xx ((#,{.)/. -: (g,.f)) yy
 if. 1:@:>: :: 0: x do.
  assert. xx (({.,#)/. -: (f,.g)) xx
  assert. xx ((#,{.)/. -: (g,.f)) xx
  assert. xx (({.,#)/. -: (f,.g)) xx=: ''($,)x
  assert. xx ((#,{.)/. -: (g,.f)) xx
 end.
 1
)

test 0 1
test 1 0
test a.
test      i.900
test _450+i.900
test  1e6+i.900
test _1e6+i.900
test u: 100 ?@$ 65536
test o.i.900
test o._450+i.900
test <"0 ?40$100


'domain error' -: 1 2 3 ({.,#)/. etx 'abc' 
'domain error' -: 1 2 3 ({.,#)/. etx ;:'foo upon thee' 


4!:55 ;:'f g test testa xx yy'


