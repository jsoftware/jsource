NB. x,y Append in place -------------------------------------------------

f=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. z,i
 end.
)

(i. -: f)"0 ?2 10$2e3

f1=: 3 : 0
 z=. i.0 0
 for_i. i.y do.
  z=. z,i. i
 end.
)

(i."0@i. -: f1)"0 ?2 10$100

f2=: 3 : 0
 z=. ''
 for_x. y do.
  z=. z,x
 end.
)

(] -: f2) 'only Hugh can prevent florist friars'
(] -: f2) ?1000$1e6

global=: i.7
f3=: 3 : 'global=. global,y'
((i.7),999 888) -: f3 999 888
NB. global -: i.7

f4=: 3 : 0
 a =: 4 27$'a'
 a1=: 4 27$'a'
 b =: 3 17$(10$'b'),'1234567'
 a =: a,b
 assert. a -: a1,b
 1
)

f4 ''

global=: ;: 'zero one two three four'

f5=: 3 : 0
 txt=. 1 2 { global
 txt=. txt ,each 'x'
)

('onex';'twox') -: f5 ''
global -: ;: 'zero one two three four'

f6=: 3 : 0
 g=. ;: 'zero one two three four'
 t=. 1 2 { g
 t=. t ,each 'x'
 assert. t -: ;:'onex twox'
 assert. g -: ;: 'zero one two three four'
 1
)

f6 ''

f7=: 3 : 0
 g=. ;: 'zero one two three four'
 t=. 1 2 { g
 t=. t ,&.> 'x'
 assert. t -: ;:'onex twox'
 assert. g -: ;: 'zero one two three four'
 1
)

f7 ''

global=: '01234'

f8=: 3 : 0
 a=. y
 a=. a,'x'
)

'01234x' -: f8 global
global -: '01234'

'01234x' -: f8 '01234'

f9=: 3 : 0
 a=. '01234'
 a=. a,'x'
)

'01234x' -: f9 ''

test=: 4 : 0
 q=. x, y
 x=. x, y
 assert. q -: x
 1
)

5 6     test 2 3 4
5 6     test 2 3 4
(i.4 3) test 10 20 30
5 6 7 8 test 0 1 0
5 6 7.8 test 0 1 0
5 6 7.8 test 0 3 4
5 6 7.8 test 0 3.4
5 6 7j8 test 0 1 0
5 6 7j8 test 0 3 4
5 6 7j8 test 0 3.4
5 6 7j8 test 0 3j4

(<5 6 ) test&>~ (i.10 25)$&.> <2 3 4
(<'12') test&>~ (i.10 25)$&.> <'abc'

testa=: 4 : 0
 (a.{~x ?@$ 256) test a.{~y ?@$ 256
 (    x ?@$ 2e9) test     y ?@$ 2e9
 (    x ?@$ 0  ) test     y ?@$ 0
)

67       testa&> '';,&.> 3 5 7 11 13 67 71
(<4 5  ) testa&> '';3;5;7
(<6 4 5) testa&> '';3;5;7; 4 5;2 3;4 3;2 5; 2 3 4;2 4 5;2 7


4!:55 ;:'a a1 b f f1 f2 f3 f4 f5 f6 f7 f8 f9 global local test testa'


