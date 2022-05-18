prolog './g421d.ijs'
NB. x ({.,#)/. y and x (#,{.)/.y ----------------------------------------

randuni''

f=: ~:@[ # ]
g=: #/.

NB. Verify special forms detected
(7!:2 'xx ({.,#"#)/. yy') > 10000 + (7!:2 'xx ({.,#)/. yy') [ xx =: i. 10000 [ yy =: i. 10000
(7!:2 'xx (#"#,{.)/. yy') > 10000 + (7!:2 'xx (#,{.)/. yy')
(7!:2 '((#,{.)/. i.@#@]) yy') > 50000 + (7!:2 '((#,{.)/. i.@#) yy') [ yy =: 10000 $ a.

test=: 3 : 0
 y testa 0 1
 y testa      i.900
 y testa _450+i.900
 y testa o.   i.900
 y testa x:   i.900
 xx=:y{~?1000$#y
 assert. ((({.,#)/. i.@#) -: ((f,.g) i.@#)) xx
 assert. (((#,{.)/. i.@#) -: ((g,.f) i.@#)) xx
 xx=:1 $ xx
 assert. ((({.,#)/. i.@#) -: ((f,.g) i.@#)) xx
 assert. (((#,{.)/. i.@#) -: ((g,.f) i.@#)) xx
 xx=:'' $ xx
 assert. ((({.,#)/. i.@#) -: ((f,.g) i.@#)) xx
 assert. (((#,{.)/. i.@#) -: ((g,.f) i.@#)) xx
 xx=:0 $ xx
 assert. ((({.,#)/. i.@#) -: ((f,.g) i.@#)) xx
 assert. (((#,{.)/. i.@#) -: ((g,.f) i.@#)) xx
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
test adot1
test adot2
test sdot0
test      i.900
test _450+i.900
test  1e6+i.900
test _1e6+i.900
test u: 100 ?@$ 65536
test 10&u: RAND32 100 ?@$ C4MAX
test o.i.900
test o._450+i.900
test <"0 ?40$100


'domain error' -: 1 2 3 ({.,#)/. etx 'abc' 
'domain error' -: 1 2 3 ({.,#)/. etx ;:'foo upon thee' 
'domain error' -: 1 2 3 ({.,#)/. etx u:'abc' 
'domain error' -: 1 2 3 ({.,#)/. etx u:&.> ;:'foo upon thee' 
'domain error' -: 1 2 3 ({.,#)/. etx 10&u:'abc' 
'domain error' -: 1 2 3 ({.,#)/. etx 10&u:&.> ;:'foo upon thee' 
'domain error' -: 1 2 3 ({.,#)/. etx s:@<"0 'abc' 
'domain error' -: 1 2 3 ({.,#)/. etx s:@<"0&.> ;:'foo upon thee' 
'domain error' -: 1 2 3 ({.,#)/. etx <"0@s: ;:'foo upon thee' 


4!:55 ;:'adot1 adot2 sdot0 f g test testa xx yy'
randfini''


epilog''

