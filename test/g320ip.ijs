prolog './g320ip.ijs'
NB. ,y ravel in place -----------------------------------------------

(i. 1000) -: , i. 1000
(i. 10000) -: , i. 10 1000
(i. 10 10000) -: ,"2 i. 10 100 100
(i. 10 24000) -: ,"3 i. 10 20 30 40
(i. 240000) -: , i. 10 20 30 40
(i. 240000) -: ,"4 i. 10 20 30 40
(,999) -: ,"0 (999)
(1 1$999) -: ,"0 , 999

IGNOREIFFVI 20000 > 7!:2 ', 10000$''a'''
IGNOREIFFVI 20000 > 7!:2 ', 100 100$''a'''
IGNOREIFFVI 20000 > 7!:2 ',"2 (10 10 100$''a'')'
IGNOREIFFVI 20000 > 7!:2 ',"3 (10 10 10 10$''a'')'
a =: 10000#'a'
IGNOREIFFVI 2000 > 7!:2 'a =: ,a'
a =: 10 1000#'a'
IGNOREIFFVI 2000 > 7!:2 'a =: ,a'
a =: 10 10 100#'a'
IGNOREIFFVI 2000 > 7!:2 'a =: ,"2 a'
a =: 10 10 10 10#'a'
IGNOREIFFVI 2000 > 7!:2 'a =: ,"3 a'

NB. x,y Append in place -------------------------------------------------

f=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. z,i
 end.
)

(i. -: f)"0 ?2 10$2e3

f=: 3 : 0
 z=. i.0
 for_i. i.y do.
  z=. i ,~ z
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

f1=: 3 : 0
 z=. i.0 0
 for_i. i.y do.
  z=. (i. i) ,~ z
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
(] -: f2) u:'only Hugh can prevent florist friars'
(] -: f2) 10&u:'only Hugh can prevent florist friars'
(] -: f2) s:@<"0 'only Hugh can prevent florist friars'
(] -: f2) ?1000$1e6
(] -: f2) 100 $ ;:'only Hugh can prevent florist friars'

f2=: 3 : 0
 z=. ''
 for_x. y do.
  z=. x ,~ z
 end.
)

(] -: f2) 'only Hugh can prevent florist friars'
(] -: f2) u:'only Hugh can prevent florist friars'
(] -: f2) 10&u:'only Hugh can prevent florist friars'
(] -: f2) s:@<"0 'only Hugh can prevent florist friars'
(] -: f2) ?1000$1e6
(] -: f2) 100 $ ;:'only Hugh can prevent florist friars'

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
 a =: 4 27$u:'a'
 a1=: 4 27$u:'a'
 b =: 3 17$(10$u:'b'),u:'1234567'
 a =: a,b
 assert. a -: a1,b
 a =: 4 27$10&u:'a'
 a1=: 4 27$10&u:'a'
 b =: 3 17$(10$10&u:'b'),10&u:'1234567'
 a =: a,b
 assert. a -: a1,b
 a =: 4 27$s:@<"0 'a'
 a1=: 4 27$s:@<"0 'a'
 b =: 3 17$(10$s:@<"0 'b'),s:@<"0 '1234567'
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

global=: (u:&.>) ;:'zero one two three four'

f5=: 3 : 0
 txt=. 1 2 { global
 txt=. txt ,each u:'x'
)

((u:'onex');u:'twox') -: f5 ''

global=: (10&u:&.>) ;:'zero one two three four'

f5=: 3 : 0
 txt=. 1 2 { global
 txt=. txt ,each 10&u:'x'
)

((10&u:'onex');10&u:'twox') -: f5 ''

global -: ;: 'zero one two three four'

f6=: 3 : 0
 g=. ;: 'zero one two three four'
 t=. 1 2 { g
 t=. t ,each 'x'
 assert. t -: ;:'onex twox'
 assert. g -: ;: 'zero one two three four'
 g=. (u:&.>) ;:'zero one two three four'
 t=. 1 2 { g
 t=. t ,each u:'x'
 assert. t -: (u:&.>) ;:'onex twox'
 assert. g -: (u:&.>) ;:'zero one two three four'
 g=. (10&u:&.>) ;:'zero one two three four'
 t=. 1 2 { g
 t=. t ,each 10&u:'x'
 assert. t -: (10&u:&.>) ;:'onex twox'
 assert. g -: (10&u:&.>) ;:'zero one two three four'
 1
)

f6 ''

f7=: 3 : 0
 g=. ;: 'zero one two three four'
 t=. 1 2 { g
 t=. t ,&.> 'x'
 assert. t -: ;:'onex twox'
 assert. g -: ;: 'zero one two three four'
 g=. (u:&.>) ;:'zero one two three four'
 t=. 1 2 { g
 t=. t ,&.> u:'x'
 assert. t -: (u:&.>) ;:'onex twox'
 assert. g -: (u:&.>) ;:'zero one two three four'
 g=. (10&u:&.>) ;:'zero one two three four'
 t=. 1 2 { g
 t=. t ,&.> 10&u:'x'
 assert. t -: (10&u:&.>) ;:'onex twox'
 assert. g -: (10&u:&.>) ;:'zero one two three four'
 g=. <"0 ;: 'zero one two three four'
 t=. 1 2 { g
 t=. t ,&.> <<,'x'
 assert. t -: _2 <\ ;:'one x two x'
 assert. g -: <"0 ;: 'zero one two three four'
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

global=: u:'01234'

f8=: 3 : 0
 a=. y
 a=. a,u:'x'
)

'01234x' -: f8 global
global -: '01234'

'01234x' -: f8 u:'01234'

global=: 10&u:'01234'

f8=: 3 : 0
 a=. y
 a=. a,10&u:'x'
)

'01234x' -: f8 global
global -: '01234'

'01234x' -: f8 10&u:'01234'

f9=: 3 : 0
 a=. '01234'
 a=. a,'x'
)

'01234x' -: f9 ''

f9=: 3 : 0
 a=. u:'01234'
 a=. a,u:'x'
)

'01234x' -: f9 ''

f9=: 3 : 0
 a=. 10&u:'01234'
 a=. a,10&u:'x'
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

NB. Verify that we avoid loop in box
a =: 10000 $ ;:'only Hugh can prevent florist friars'
b =: <i. 10000
IGNOREIFFVI 2000 > 7!:2 'a =: a , a:'
IGNOREIFFVI 2000 > 7!:2 'a =: a , b'
IGNOREIFFVI 2000 > 7!:2 'a =: a , <b'
IGNOREIFFVI 40000 < 7!:2 'a =: a , <a'  NB. Would loop, not inplace
IGNOREIFFVI 40000 < 7!:2 'a =: a , <<<<a'  NB. Would loop, not inplace

0 = 16777216 (17 b.) 1 { 13!:_4 (<"0 i. 2 5) , <memu 100   NB. scalar replication removes pristinity
0 = 16777216 (17 b.) 1 { 13!:_4 (<"0 i. 2 5) ,!.(<memu 100) <"0 i. 3  NB. installing user fill loses pristinity

NB. Verify that virtual extension inplaces
a =: i. 1e6
2000 > 7!:2 '_5 {. a , _1'
a -: i. 1e6
999996 999997 999998 999999 _1 -: _5 {. a , _1
NB. float
a =: 0.5 + i. 1e5
2000 > 7!:2 '_5 {. a , _1'
a -: 0.5 + i. 1e5
99996.5 99997.5 99998.5 99999.5 _1 -: _5 {. a , _1
NB. Not boxed
a =: <"0 i. 1e5
2000 < 7!:2 '_5 {. a , a:'
a -: <"0 i. 1e5
NB. Not extended
a =: i. 100000x
2000 < 7!:2 '_5 {. a , _1'
NB. Even if name is on the stack
a =: i. 1e6
f10=:{{ _5 {. a , 6 }}
999996 999997 999998 999999 6 -: f10 a
a =: i. 1e6
1e6 -: # ([ f10@5) a
20000 > 7!:2 'f10 5 [ a'
NB. asgn-in-place still allowed if name has been taken off the stack
a =: i. 1e6
f10=:{{ _5 {. a =: a , 6 }}
999996 999997 999998 999999 6 -: f10 a
a =: i. 1e6
1e6 -: # ([ f10@5) a
20000 > 7!:2 'f10 5 [ a'
NB. asgn-in-place not allowed if name is on the stack
a =: i. 1e6
f10=:{{ _5 {. a =: a , 6 }}
999996 999997 999998 999999 6 -: f10 a
a =: i. 1e6
1e6 -: # ([ f10@5) a
20000 < 7!:2 '([ f10@5) a'

NB. Verify forms for indexing
a =: i. 1e6
10000 > 7!:2 '3 : ''a =: }:@:(,&8) a'''  NB. must be in an explicit to inplace
NB. no virtual extension 5000 > 7!:2 '1 2 3 _1 { a , 7'
1 2 3 7 -: 1 2 3 _1 { a , 7

NB. Verify no local-to-global aliasing
f10 =: 3 : 'a =. a , 8'
f11 =: 3 : 'a =: a , 8'
f12 =: 3 : 'a =: a , a =. 2 3'
f13 =: 3 : 'a =. a , a =. 2 3'
a =: i. 4
0 1 2 3 8 -: f10''
0 1 2 3 -: a
0 1 2 3 8 -: f11''
0 1 2 3 8 -: a
'domain error' -: f12 etx ''
2 3 2 3 -: f13''

NB. Verify constants not corrupted in unassigned verbs
f =: 3 : 0"0 '123'
'abc' , (2#y) , 'd'
)
('abc11d' , 'abc22d' ,: 'abc33d') -: f

NB. Remember, 9!:53 has different effects on explicit sentences vs console!
NB. Create unsafe name
1: 4!:55 ;:'unsafename undefinedname undefinedname2'
unsafename =: undefinedname2  NB. first name-to-name ref is swallowed
undefinedname2 =: undefinedname
undefinedname =: ]

NB. Verify inplacing works in forks, including nvv forks
9!:53 (0)
IGNOREIFFVI 1120000 > 7!:2 '(i. , ]) 100000'
'lrlr' -: (1 {. 'l') (, , ,) (1 {. 'r')
(10000 # 'lrlr') -: (10000 # 'l') (, , ,) (10000 # 'r')
IGNOREIFFVI 24000 > 7!:2 '(10000 # ''l'') (, , ]) (1000 # ''r'')'
(10000 2000 # 'lr') -: (10000 # 'l') (, , ]) (1000 # 'r')
IGNOREIFFVI 24000 > 7!:2 '(10000#''l'') ([: ] ,) (1000#''r'')'
0 7 14 21 28 -: (({.7)"_ * ])"0 i. 5  NB. ensure constant function not overwritten
0 7 14 21 28 -: (({.7) * ])"0 i. 5  NB. ensure n in nvv not overwritten
b =: 7
0 7 14 21 28 -: (b * ])"0 i. 5  NB. ensure name in nvv not overwritten
b -: 7
9!:53 (0)
a =: 10000#'a'
IGNOREIFFVI 3000 > 7!:2 'a =: (''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace  even with 9!:53 (0)
IGNOREIFFVI 3000 > 7!:2 'a =: (({.''b'') ,~ [) a'  NB. Usecount -1 too  even with 9!:53 (0)
nb =: ('b' ,~ [)
(dbq'')+. IGNOREIFFVI 3000 > 7!:2 'a =: nb a'  NB. usecount=1 inside the name, still inplace even with 9!:53 (0)
a -: (10000#'a'),'bbb'
b =: 'c'
IGNOREIFFVI 3000 > 7!:2 'a =: (b ,~ ]) a'  NB. different name, still inplace assignment  even with 9!:53 (0)
b -: 'c'
a -: (10000#'a'),'bbbc'

a =: 10000#'a'   NB. Repeat as left tine of fork - now suppressed when 9!:53 (0)
IGNOREIFFVI 3000 < 7!:2 'a =: (] ] ''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0) or console
IGNOREIFFVI 3000 < 7!:2 'a =: ''b'' (] ] [ ,~ ]) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0) or console
IGNOREIFFVI 3000 < 7!:2 'a =: ''b'' ([ ] [ ,~ ]) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0) or console
IGNOREIFFVI 3000 < 7!:2 'a =: (] ] ({.''b'') ,~ [) a'  NB. Usecount -1 too  but not with 9!:53 (0) or console
nb =: (] ] 'b' ,~ [)
IGNOREIFFVI 3000 < 7!:2 'a =: nb a'  NB. usecount=1 inside the name, still inplace but not with 9!:53 (0) or console
a -: (10000#'a'),'bbbbb'
b =: 'c'
IGNOREIFFVI 3000 < 7!:2 'a =: (] ] b ,~ ]) a'  NB. different name, still inplace assignment but not with 9!:53 (0) or console
b -: 'c'
a -: (10000#'a'),'bbbbbc'

IGNOREIFFVI 3000 < 7!:2 'a =: (b ,~ unsafename) a'  NB. unsafe name, not inplace
b -: 'c'
a -: (10000#'a'),'bbbbbcc'
IGNOREIFFVI 3000 < 7!:2 'a =: (a ,~ ]) a'  NB. same name, not inplace
a -: ((10000#'a'),'bbbbbcc'),((10000#'a'),'bbbbbcc')

9!:53 (1)  NB. Now they should inplace
a =: 10000#'a'
IGNOREIFFVI 3000 > 7!:2 'a =: (''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace
NB. sporadic failures
IFUNIX+. IGNOREIFFVI 3000 < 7!:2 'a =: ''b'' (] ] [ ,~ ]) a'   NB. not with 9!:53 (0) or console
NB. sporadic failures
1 [ 3000 < 7!:2 'a =: ''b'' ([ ] [ ,~ ]) a'   NB. not with 9!:53 (0) or console
IGNOREIFFVI 3000 > 7!:2 'a =: (({.''b'') ,~ [) a'  NB. Usecount -1 too
nb =: ('b' ,~ [)
(dbq'')+. IGNOREIFFVI 3000 > 7!:2 'a =: nb a'  NB. usecount=1 inside the name, still inplace
a -: (10000#'a'),'bbbbb'
b =: 'c'
IGNOREIFFVI 3000 > 7!:2 'a =: (b ,~ ]) a'  NB. different name, still inplace assignment
b -: 'c'
a -: (10000#'a'),'bbbbbc'

a =: 10000#'a'   NB. Repeat as left tine of fork - still suppressed when 9!:53 (1) because comes from console
IGNOREIFFVI 3000 < 7!:2 'a =: (] ] ''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0)
IGNOREIFFVI 3000 < 7!:2 'a =: (] ] ({.''b'') ,~ [) a'  NB. Usecount -1 too  but not with 9!:53 (0)
nb =: (] ] 'b' ,~ [)
IGNOREIFFVI 3000 < 7!:2 'a =: nb a'  NB. usecount=1 inside the name, still inplace but not with 9!:53 (0)
a -: (10000#'a'),'bbb'
b =: 'c'
IGNOREIFFVI 3000 < 7!:2 'a =: (] ] b ,~ ]) a'  NB. different name, still inplace assignment but not with 9!:53 (0)
b -: 'c'
a -: (10000#'a'),'bbbc'

IGNOREIFFVI 3000 < 7!:2 'a =: (b ,~ unsafename) a'  NB. unsafe name, not inplace
b -: 'c'
a -: (10000#'a'),'bbbcc'
IGNOREIFFVI 3000 < 7!:2 'a =: (a ,~ ]) a'  NB. same name, not inplace
a -: ((10000#'a'),'bbbcc'),((10000#'a'),'bbbcc')
NB. Boxed values should cause no trouble, but we don't have anything that inplaces boxed arrays yet

NB. Repeat the tests in an explicit definition - twice, to ensure no corruption of stored sentences
ipexp =: 3 : 0
0 7 14 21 28 -: (({.7)"_ * ])"0 i. 5  NB. ensure constant function not overwritten
0 7 14 21 28 -: (({.7) * ])"0 i. 5  NB. ensure n in nvv not overwritten
b =. 7
0 7 14 21 28 -: (b * ])"0 i. 5  NB. ensure name in nvv not overwritten
b -: 7
9!:53 (0)
a =. 10000#'a'
IGNOREIFFVI 3000 < 7!:2 'a =. (''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0)
IGNOREIFFVI 3000 < 7!:2 'a =. ''b'' (] ] [ ,~ ]) a'   NB. not with 9!:53 (0) or console
IGNOREIFFVI 3000 < 7!:2 'a =. ''b'' ([ ] [ ,~ ]) a'   NB. not with 9!:53 (0) or console
IGNOREIFFVI 3000 < 7!:2 'a =. (({.''b'') ,~ [) a'  NB. Usecount -1 too  but not with 9!:53 (0)
nb =. ('b' ,~ [)
IGNOREIFFVI 3000 < 7!:2 'a =. nb a'  NB. usecount=1 inside the name, still inplace  but not with 9!:53 (0)
a -: (10000#'a'),'bbbbb'
b =. 'c'
IGNOREIFFVI 3000 < 7!:2 'a =. (b ,~ ]) a'  NB. different name, still inplace assignment  but not with 9!:53 (0)
b -: 'c'
a -: (10000#'a'),'bbbbbc'

a =. 10000#'a'   NB. Repeat as left tine of fork - not allowed when 9!:53 (0)
IGNOREIFFVI 3000 < 7!:2 'a =. (] ] ''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0)
IGNOREIFFVI 3000 < 7!:2 'a =. (] ] ({.''b'') ,~ [) a'  NB. Usecount -1 too  but not with 9!:53 (0)
nb =. (] ] 'b' ,~ [)
IGNOREIFFVI 3000 < 7!:2 'a =. nb a'  NB. usecount=1 inside the name, still inplace but not with 9!:53 (0)
a -: (10000#'a'),'bbbbb'
b =. 'c'
IGNOREIFFVI 3000 < 7!:2 'a =. (] ] b ,~ ]) a'  NB. different name, still inplace assignment but not with 9!:53 (0)
b -: 'c'
a -: (10000#'a'),'bbbbbc'

IGNOREIFFVI 3000 < 7!:2 'a =. (b ,~ unsafename) a'  NB. unsafe name, not inplace
b -: 'c'
a -: (10000#'a'),'bbbbbcc'
IGNOREIFFVI 3000 < 7!:2 'a =. (a ,~ ]) a'  NB. same name, not inplace
a -: ((10000#'a'),'bbbbbcc'),((10000#'a'),'bbbbbcc')

9!:53 (1)
a =. 10000#'a'
IGNOREIFFVI 3000 > 7!:2 'a =. (''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace
IGNOREIFFVI 3000 < 7!:2 'a =. ''b'' (] ] [ ,~ ]) a'   NB. Can't inplace h with f in use
IGNOREIFFVI 3000 > 7!:2 'a =. ''b'' ([ ] [ ,~ ]) a'   NB. Can inplace with 9!:53 and y free
IGNOREIFFVI 3000 > 7!:2 'a =. (({.''b'') ,~ [) a'  NB. Usecount -1 too
nb =. ('b' ,~ [)
IGNOREIFFVI 3000 > 7!:2 'a =. nb a'  NB. usecount=1 inside the name, still inplace
a -: (10000#'a'),'bbbbb'
b =. 'c'
IGNOREIFFVI 3000 > 7!:2 'a =. (b ,~ ]) a'  NB. different name, still inplace assignment
b -: 'c'
a -: (10000#'a'),'bbbbbc'

a =. 10000#'a'   NB. Repeat as left tine of fork - allowed when 9!:53 (1)
IGNOREIFFVI 3000 > 7!:2 'a =. (] ] ''b'' ,~ [) a'   NB. NVV with Usecount=1 still inplace  but not with 9!:53 (0)
IGNOREIFFVI 3000 > 7!:2 'a =. (] ] ({.''b'') ,~ [) a'  NB. Usecount -1 too  but not with 9!:53 (0)
nb =. (] ] 'b' ,~ [)
IGNOREIFFVI 3000 > 7!:2 'a =. nb a'  NB. usecount=1 inside the name, still inplace but not with 9!:53 (0)
a -: (10000#'a'),'bbb'
b =. 'c'
IGNOREIFFVI 3000 > 7!:2 'a =. (] ] b ,~ ]) a'  NB. different name, still inplace assignment but not with 9!:53 (0)
b -: 'c'
a -: (10000#'a'),'bbbc'

IGNOREIFFVI 3000 < 7!:2 'a =. (b ,~ unsafename) a'  NB. unsafe name, not inplace
b -: 'c'
a -: (10000#'a'),'bbbcc'
IGNOREIFFVI 3000 < 7!:2 'a =. (a ,~ ]) a'  NB. same name, not inplace
a -: ((10000#'a'),'bbbcc'),((10000#'a'),'bbbcc')
)
ipexp''
ipexp''
9!:53 (0)

a =: 'abc'
'abcbabcb' -: a (, , ,) 'b'
a =: a (, , ,) 'b'
'abcbabcb' -: a
'abcbabcb' -: ('a','bc') (, , ,) {.'b'
a =: 'abc'
a =: 'abc' (, , (, , ,)) 'b'
a -: 'abcbabcbabcb'
a =: 'abc'
a =: 'abc' (, , (, , [)) 'b'
a -: 'abcbabcbabc' 

9!:53 (1)
a =: >: i. 9  NB. >: so not AFRO
a =: (a ,~ 1 ,~ ]) a  NB. value in nvv must be protected
((>: i. 9) , 1 , (>: i. 9)) -: a

a =: >: i. 9  NB. >: so not AFRO
a =: ([: ,&a 1 ,~ ]) a  NB. value in nvv must be protected
((>: i. 9) , 1 , (>: i. 9)) -: a

a =: >: i. 1e6  NB. not AFRO
3000 > 7!:2 'a =: 5&+ a'

a =: 100 100 ?@$ 0
22000 > 7!:2 'a =: 99 I.@:(<&0.001)@,@]} a'

a =: 'ab'
b =: 1e5 $ 'abdab'
3000 > 7!:2 'b =: ([: ,&a ''z'' ,~ ]) b'
((1e5 $ 'abdab') , 'zab') -: b
'ab' -: a
a =: ([: ,&a 'z' ,~ ]) a
'abzab' -: a

NB. hook
9!:53 (0)
('c' ,~ 999 $ 'abc') -: (, {:) 999 $ 'abc'
IGNOREIFFVI 3000 > 7!:2 '(, {:) 888 $ ''abc'''
('c' ,~ 1000 # 'a') -: (1000 # 'a') (, ]) 'c'
IGNOREIFFVI 4000 > 7!:2 '(1000 # ''a'') (, ]) ''c'''
a =: 10000#'a'
IGNOREIFFVI 4000 > 7!:2 'a =: (, {.) a'
IGNOREIFFVI 4000 < 7!:2 'a =: (] ] (, {.)) a'
9!:53 (1)
IGNOREIFFVI 4000 > 7!:2 'a =: (, {.) a'
IGNOREIFFVI 4000 < 7!:2 'a =: (, {.@unsafename) a'
NB. singleton hook
a =: 10+1
aad =: 15!:14 <'a'
a =: a + 4  NB. inplaceable
aad2=:15!:14 <'a'
aad = aad2
a =: a (+ ]) 4  NB. inplaceable
aad=:15!:14 <'a'
aad = aad2
3 : 0 ''
a =. 10+1
aad =. 15!:14 <'a'
a =. a + 4
assert. aad = 15!:14 <'a' [ 1
a =. a (+ ]) 4
assert. aad = 15!:14 <'a' [ 2
1
)
9!:53 (0)


NB. u@v
'b' -: (10000#'a') {:@, 'b'
IGNOREIFFVI 20000 > 7!:2 '(10000#''a'') {:@, ''b'''         
9!:53 (0)
a =: 10000#'c'
IGNOREIFFVI 5000 < 7!:2 'a =: ]@(({.''b'') ,~ ]) a'  NB. name is virtually extended, then realized
a =: 10000#'c'
IGNOREIFFVI 5000 > 7!:2 'a =: {.@(({.''b'') ,~ ]) a'  NB. name is virtually extended, then truncated
a =: 10000#'c'
IGNOREIFFVI 3000 < 7!:2 'a =: a ]@, ''b'''

f =: 3 : 0
9!:53 (1)
a =: 10000#'c'
assert. IGNOREIFFVI 3000 > 7!:2 'a =: {.@(({.''b'') ,~ ]) a'
a =: 10000#'c'
assert. IGNOREIFFVI 3000 > 7!:2 'a =: a ]@, ''b'''
assert. IGNOREIFFVI 3000 < 7!:2 'a =: a unsafename@, ''b'''
1
)
f ''
9!:53 (0)

NB. u@:v
'b' -: (10000#'a') {:@:, 'b'
IGNOREIFFVI 20000 > 7!:2 '(10000#''a'') {:@:, ''b'''         
9!:53 (0)
a =: 10000#'c'
IGNOREIFFVI 5000 < 7!:2 'a =: ]@:(({.''b'') ,~ ]) a'
a =: 10000#'c'
IGNOREIFFVI 5000 > 7!:2 'a =: {.@:(({.''b'') ,~ ]) a'
a =: 10000#'c'
IGNOREIFFVI 3000 < 7!:2 'a =: a ]@:, ''b'''

f =: 3 : 0
9!:53 (1)
a =: 10000#'c'
assert. IGNOREIFFVI 3000 > 7!:2 'a =: {.@:(({.''b'') ,~ ]) a'
a =: 10000#'c'
assert. IGNOREIFFVI 3000 > 7!:2 'a =: a ]@:, ''b'''
assert. IGNOREIFFVI 3000 < 7!:2 'a =: a unsafename@:, ''b'''
1
)
f''
9!:53 (0)

NB. u&v
a =: i. 1000
0 1 2 3 4 -: ($0) {.&(5 ,~ ]) a
9!:53 (0)
a =: i. 1000
IGNOREIFFVI 3000 < 7!:2 'a =: ($0) {.&([: >@{.@< 5 ,~ ]) a'  NB. realize any virtual block

f =: 3 : 0
9!:53 (1)
a =: i. 1000
assert. IGNOREIFFVI 3000 > 7!:2 'a =: ($0) {.&([: >@{.@< 5 ,~ ]) a'
a =: i. 1000
assert. IGNOREIFFVI 3000 < 7!:2 'a =: ($0) {.&([: >@{.@< 5 ,~ unsafename) a'
1
)
f''
9!:53 (0)

NB. u&:v
a =: i. 1000
0 1 2 3 4 -: ($0) {.&:(5 ,~ ]) a
9!:53 (0)
a =: i. 1000
IGNOREIFFVI 3000 < 7!:2 'a =: ($0) {.&:([: >@{.@< 5 ,~ ]) a'
f =: 3 : 0
9!:53 (1)
a =: i. 1000
assert. IGNOREIFFVI 3000 > 7!:2 'a =: ($0) {.&:([: >@{.@< 5 ,~ ]) a'
a =: i. 1000
assert. IGNOREIFFVI 3000 < 7!:2 'a =: ($0) {.&:([: >@{.@< 5 ,~ unsafename) a'
1
)
f''
9!:53 (0)


NB. u&n
IGNOREIFFVI 20000 > 7!:2 ',&''a'' 10000#''b'''
a =: 10000#'a'
9!:53 (0)
a =: 10000#'a'
('c' ,~ 10000#'a') -: a =: ,&'c' a
IGNOREIFFVI 2000 > 7!:2 'a =: ,&({.''c'') a'

f =: 3 : 0
9!:53 (1)
a =: 10000#'a'
assert. ('c' ,~ 10000#'a') -: a =: ,&'c' a
assert. IGNOREIFFVI 2000 > 7!:2 'a =: ,&({.''c'') a'
1
)
f''
9!:53 (0)

NB. m&v
(5,.i.4) -: (6-1)&,"0 i. 4  NB. Verify constant not overwritten
a =: 5 6
15 17 -: 3 : 'a =: 5 (a&+)@:+ a' ''  NB. Verify not inplace
a =: i. 10000
40000 < 7!:2 '3 : ''a =: 5 (5&+)@:+ a'' 0'  NB. Not safe with usecount 1
40000 < 7!:2 '3 : ''a =: 5 ((15!:15 (5))&+)@:+ a'' 0'  NB. Safe with usecount _1
9!:53 (1)
12000 > 7!:2 '3 : ''a =: 5 ((15!:15 (5))&+)@:+ a'' 0'  NB. Safe with usecount _1

NB. u&.v
IGNOREIFFVI 20000 > 7!:2 ',&''b''&.] 10000#''a'''
9!:53 (0)
a =: 10000#5
(6 ,~ 10000#5) -: a =: ,&6&.] a
IGNOREIFFVI 3000 < 7!:2 'a =: ,&6&.] a'
IGNOREIFFVI 3000 < 7!:2 '{. a =: ,&6&.] a'  NB. Verify assignment need not be first word
a =: 10000#'a'
IGNOREIFFVI 17000 < 7!:2 'a =: ]&.(,&({.''b'')) a'
IGNOREIFFVI 17000 < 7!:2 'a =: ]&.(,&(''b'')) a'
f =: 3 : 0
9!:53 (1)
a =: 10000#5
assert. (6 ,~ 10000#5) -: a =: ,&((15!:15) 6)&.] a
assert. IGNOREIFFVI 3000 > 7!:2 'a =: ,&((15!:15) 6)&.] a'
assert. IGNOREIFFVI 3000 > 7!:2 '{. a =: ,&((15!:15) 6)&.] a'  NB. Verify assignment need not be first word
a =: 10000#'a'
assert. IGNOREIFFVI 20000 > 7!:2 'a =: ]&.(,&({.''b'')) a'
assert. IGNOREIFFVI 20000 > 7!:2 'a =: ]&.(,&(''b'')) a'
assert. IGNOREIFFVI 17000 < 7!:2 'a =: unsafename&.(,&(''b'')) a'
1
)
f''
9!:53 (0)

NB. u&.:v
IGNOREIFFVI 20000 > 7!:2 ',&''b''&.:] 10000#''a'''
9!:53 (0)
a =: 10000#5
(6 ,~ 10000#5) -: a =: ,&((15!:15) 6)&.:] a
IGNOREIFFVI 3000 < 7!:2 'a =: ,&((15!:15) 6)&.:] a'
IGNOREIFFVI 3000 < 7!:2 '{. a =: ,&((15!:15) 6)&.:] a'  NB. Verify assignment need not be first word
a =: 10000#'a'
IGNOREIFFVI 17000 < 7!:2 'a =: ]&.:(,&({.''b'')) a'
IGNOREIFFVI 17000 < 7!:2 'a =: ]&.:(,&(''b'')) a'

f =: 3 : 0
9!:53 (1)
a =: 10000#5
assert. (6 ,~ 10000#5) -: a =: ,&((15!:15) 6)&.:] a
assert. IGNOREIFFVI 3000 > 7!:2 'a =: ,&((15!:15) 6)&.:] a'
assert. IGNOREIFFVI 3000 > 7!:2 '{. a =: ,&((15!:15) 6)&.:] a'  NB. Verify assignment need not be first word
a =: 10000#'a'
assert. IGNOREIFFVI 20000 > 7!:2 'a =: ]&.:(,&({.''b'')) a'
assert. IGNOREIFFVI 20000 > 7!:2 'a =: ]&.:(,&(''b'')) a'
assert. IGNOREIFFVI 17000 < 7!:2 'a =: unsafename&.:(,&(''b'')) a'
1
)
f''
9!:53 (0)

NB. u^:v
a =: 10000#'a'
IGNOREIFFVI 3000 > 7!:2 ']^:0 a'
IGNOREIFFVI 3000 > 7!:2 ']^:1 a'
b =: 10000#a:
IGNOREIFFVI 3000 > 7!:2 ']^:0 b'
IGNOREIFFVI 3000 > 7!:2 ']^:1 b'
IGNOREIFFVI 23000 > 7!:2 '(10000#''a'') ,^:0: ''b'''
IGNOREIFFVI 23000 > 7!:2 '(10000#''a'') ,^:1: ''b'''
9!:53 (0)
i =: i. 5000
IGNOREIFFVI 3000 > 7!:2 'i =: 0 ,~^:0 i'
i -: (i. 5000) 
IGNOREIFFVI 3000 > 7!:2 'i =: 0 ,~^:1 i'   NB. u^:1 is equivalent to u
i -: (i. 5000) , 0
IGNOREIFFVI 3000 > 7!:2 'i =: 0 ,~^:[ i'
i -: (i. 5000) , 0
IGNOREIFFVI 3000 > 7!:2 'i =: 1 ,~^:[ i'
i -: (i. 5000) , 0 1
IGNOREIFFVI 3000 < 7!:2 'i =: 1 ,~^:([  0 ,~ ]) i'
i -: (i. 5000) , 0 1 1
f =: 3 : 0
9!:53 (1)
i =: i. 5000
assert. IGNOREIFFVI 3000 > 7!:2 'i =: 0 ,~^:0 i'
assert. i -: (i. 5000) 
assert. IGNOREIFFVI 3000 > 7!:2 'i =: 0 ,~^:1 i'
assert. i -: (i. 5000) , 0
assert. IGNOREIFFVI 3000 > 7!:2 'i =: 0 ,~^:[ i'
assert. i -: (i. 5000) , 0
assert. IGNOREIFFVI 3000 > 7!:2 'i =: 1 ,~^:[ i'
assert. i -: (i. 5000) , 0 1
assert. IGNOREIFFVI 3000 > 7!:2 'i =: i ,^:] 1'
assert. i -: (i. 5000) , 0 1 1
assert. IGNOREIFFVI 3000 < 7!:2 'i =: 1 ,~^:([  0 ,~ ]) i'
assert. i -: (i. 5000) , 0 1 1 1
assert. IGNOREIFFVI 3000 < 7!:2 'i =: i ,^:unsafename 1'
assert. i -: (i. 5000) , 0 1 1 1 1
1
)
f''
9!:53 (0)


NB. u@.v
i =: i. 5000
IGNOREIFFVI (9000 * IF64{4 8) > 7!:2 '(i. 5000) ,`]`>:@.] 0'
IGNOREIFFVI (9000 * IF64{4 8) > 7!:2 '(i. 5000) ,`]`[`>:@.] 1'
i =: i. 5000
IGNOREIFFVI (9000 * IF64{4 8) > 7!:2 '(i. 5000) ,`]`[`>:@.] 2'
IGNOREIFFVI (500 * IF64{4 8) > 7!:2 'i ,`]`[`>:@.] 1'
i =: i. 5000
IGNOREIFFVI (500 * IF64{4 8) > 7!:2 'i ,`]`[`>:@.] 2'
9!:53 (0)
IGNOREIFFVI 4000 > 7!:2 'i =: i ,`]`[`>:@.] 0'
IGNOREIFFVI 4000 > 7!:2 'i =: i ,`]`[`>:@.] 1'
i =: i. 5000
IGNOREIFFVI 4000 > 7!:2 'i =: i ,`]`[`>:@.] 2'
IGNOREIFFVI 3000 < 7!:2 'i =: i ,`]`[`>:@.] 3'
9!:53 (1)
IGNOREIFFVI 4000 > 7!:2 'i =: i ,`]`[`>:@.] 0'
IGNOREIFFVI 4000 > 7!:2 'i =: i ,`]`[`>:@.] 1'
i =: i. 5000
IGNOREIFFVI 4000 > 7!:2 'i =: i ,`]`[`>:@.] 2'
IGNOREIFFVI 3000 < 7!:2 'i =: i ,`unsafename`[`>:@.] 0'
IGNOREIFFVI 3000 < 7!:2 'i =: i ,`]`[`unsafename@.] 0'
IGNOREIFFVI 3000 < 7!:2 'i =: i ,`]`[`>:@.unsafename 0'
9!:53 (2)   NB. default

NB. sort on numeric lists
(50000 + 7!:2 'a =: /:~ a') > (7!:2 '/:~ a') [ a =: 10000 ?@$ 0  NB. Not inplaced; block has usecount 2
(50000 + 7!:2 'a =: /:~ a') < (7!:2 '/:~ a')
(160 + 7!:2 'a =: /:~ a') > (7!:2 '/:~ a') [ a =: 38 ?@$ 20000  NB. Not inplaced; block has usecount 2
(160 + 7!:2 'a =: /:~ a') < (7!:2 '/:~ a')

NB. The following failed when we didn't properly set the inplace flags for the selected verb
NB. The sentence aborted without performing the assignment
4!:55 ;:'a a'
a =: $$ a: (3 : ('y';':';'x'))`0:@.(0:) a:
(,0) -: a
4!:55 ;:'a'
a =: $$ (3 : ('y';':';'x'))`0:@.(0:) a:
(,0) -: a


NB. Don't inplace h in (f g h) if a==w, even if f is f@[
1 7 7 -: ($@] , ,)~ ,{.7

NB. Similarly in g}
1 0 -: (>:@]`(1{.[)`,)}~ i. 1
1 0 -: (>:@]`[`,)}~ i. 1

NB. Make sure zombieval is used only once
f =: 3 : 0
blk =. ([: <. >.&.(2&^.)@#) >{.y
blk =. ([: <. >.&.(2&^.)@#) >{.y
)
64 -: f 3 # <48#'x'

NB. Verify hook does not in-place g on repeated argument
f =: 3 : 0
(, ('c' ,~ ]))~ 2 # 'a'
)
'aaaac' -: f ''


NB. Verify assignment-in-place does not modify an aliased name
NB. Add a line here whenever new verbs are inplaced
3 : 0 ''
y =. i. 5 5
3 : 'y =. ,y' y
assert. y -: i. 5 5
3 : 'y =. y , i. 5' y
assert. y -: i. 5 5
1
)

NB. Verify fork does not execute f if there is an error in f
'domain error' -: (cocurrent [ 1&+) etx <'nonexlocale'
-. (<'nonexlocale') e. 18!:1]0 
'domain error' -: 1 (cocurrent@] [ +) etx <'nonexlocale'
-. (<'nonexlocale') e. 18!:1]0

NB. Test for intermediate result persisting when the name of which it is a part is reassigned
qd=: 3 : 'jdlast=: <,''a'''
t=: >@{:@qd
(t'')-:t''

NB. Verify that names defined up the stack prevent assignment through assignsym
t =: 5 6 7 8 9
5 6 7 8 9 -: ([ 3 : 't =: 4 (1}) t'@+:) t
5 4 7 8 9 -: ([ 3 : 'y =. 4 (1}) y'@:+:) t
5 4 7 8 9 -: ([ 3 : 't =: y =. 4 (1}) y'@:+:) t
10 4 14 16 18 -: ([ 3 : 'y =. 4 (1}) y'@}.) t
10 4 14 16 18 -: ([ 3 : 't =: y =. 4 (1}) y'@}.) t

t =: 5 6 7
5 6 7 -: ([ 3 : 't =: t , 0'@+:) t

NB. The following failed once on nonavx because of error pushing zombieval
'nmm min max'=. 0.1 _1.99977 1.99977
min=. nmm ([ * [: <. %~) 1.98
0.1 1.9 1.99977 -: nmm,min,max

10 -: ([ + 1 * -~)/ +: 2 5  NB. Failed on error in inplacing u~


NB. Verify inplaceable pscan, sscan
f =: 4 : 0&>
(1.2 * 7!:5 <'y') > 7!:2 x , ' 15!:15 y'
)
('<./\';'>./\';'<./\.';'>./\.') f/ ((i. 1e5);(0.5+i. 1e5))
('+/\';'-/\';'*/\';'+/\.';'-/\.';'*/\.') f/ (<(0.5+i. 1e5))

NB. Verify inplaceable #
l0 =: 1002 > i. 2000
l1 =: 999 > i. 2000
(9000 * >:IF64) > 7!:2 'l0 # i. 2000'
(9000 * >:IF64) < 7!:2 'l1 # i. 2000'

NB. Inplaceable monads
a =: (0.5 * 4.57e6) * i. 10000
60000 < '^ a' -&(7!:2) 'a =: ^ a'
(^ a) -:!.0 (^ 15!:15 a) [ a =: (* * ^@:|) i: 50

a =: (0.5 * 4.57e6) * i. 10000
60000 < '| a' -&(7!:2) 'a =: | a'
(| a) -:!.0 (| 15!:15 a) [ a =: (0.5+i:5) , (* * ^@:|) i: 50

a =: (0.5 * 4.57e6) * i. 10000
60000 < '%: a' -&(7!:2) 'a =: %: a'
(%: a) -:!.0 (%: 15!:15 a) [ a =: (0.5+i:5) , (* * ^@:|) i: 50

a =: (0.5 * 4.57e6) * i. 10000
b =: (0.5 * 4.57e6) * i. 10000
(-. IF64) +. 60000 < '<. b' -&(7!:2) 'a =: <. a'
(<. a) -:!.0 (<. 15!:15 a) [ a =: (0.5+i:5) , (* * ^@:|) i: 50

a =: (0.5 * 4.57e6) * i. 10000
b =: (0.5 * 4.57e6) * i. 10000
(-. IF64) +. 60000 < '>. b' -&(7!:2) 'a =: >. a'
(>. a) -:!.0 (>. 15!:15 a) [ a =: (0.5+i:5) , (* * ^@:|) i: 50

a =:  i: 5000
(-. IF64) +. 60000 < '* a' -&(7!:2) 'a =: * a'
(* a) -:!.0 (* 15!:15 a) [ a =: (i: 5) , imin,imax,(, 200&*) i: 50

a =:  i: 5000
60000 < '| a' -&(7!:2) 'a =: | a'
(| a) -:!.0 (| 15!:15 a) [ a =: (i: 5) , imin,imax,(, 200&*) i: 50

NB. Abandoned inputs become inplaceable in an explicit definition
f1 =: 3 : 'y =. b 5} y'
(dbq'') +. 140000 > 7!:2 'f1 131000 # a' [ a =: 'a' [ b =: 'b'   NB. debug doesn't inplace names

NB. They can be virtual too
(dbq'') +. 140000 > 7!:2 'f1 }. 131000 # a' [ a =: 'a' [ b =: 'b'

NB. They can be passed through but lose inplaceability at lower levels
f2 =: 3 : 'f1 y'
140000 < 7!:2 'f2 131000 # a' [ a =: 'a' [ b =: 'b'

NB. They are reset to inplace on exit
f3 =: 3 : '5'
(dbq'') +. 140000 > 7!:2 '(f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a)'
f3 =: 3 : 0
y =. b 5} y
5
)
(dbq'') +. 140000 > 7!:2 '(f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a)'
f3 =: 3 : 0
y =. 10
5
)
(dbq'') +. 140000 > 7!:2 '(f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a)'

NB. Assignment to self leaves inplaceable
f3 =: 3 : 0
y =. y
5
)
(dbq'') +. 140000 > 7!:2 '(f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a)'

NB. Assignment ending in ) is inplaceable
f3 =: 3 : 0
(y =. '2' 0} y)
y =. (y =. '2' 0} y) , '5'
y =. (y =. '2' 0} y) , '5'
y =. (y =. '2' 0} y) , '5'
(y =. 'b' ,~ y =. '3' 0} y)
(y =. (y =. '3' 0} y) ,  '5')
5
)
(dbq'') +. 140000 > 7!:2 '(f3 130000 # ''a'')'


NB. Inplaceable result of explicit definition stays inplaceable after exit
f3 =: 3 : '10000 # a'
(dbq'') +. 210000 > 7!:2 '(f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a)'
NB. Even if named
f3 =: 3 : 'p =. 10000 # a'
(dbq'') +. 210000 > 7!:2 '(f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a) [ (f3 131000 # a)'

a =.  'this';'is'  NB. Old bug when tried to zap the backer for a virtual block
(1;'this') -: ((}. 1: 1:);0&{) a_:
a =.  'this';'is'
(1;'this') -: ((1: 1: }.);0&{) a_:
a =.  'this';'is'
(1;'this') -: (([: 1: }.);0&{) a_:

NB. WILLOPEN through m&v u&n ~ hook

a =: i. 1e6
10000 >  7!:2 '((i. 5) ,:"0 (2e5)) 3&({. #@>)@:(<;.0) a'
10000 >  7!:2 '((i. 5) ,:"0 (2e5)) ({. #@>)~&3@:(<;.0) a'


NB. Inplacing forks
NB. The code to generate the testcases:
1: 0 : 0
testres =: 3 : 0&.>
r =. ". y
'(' , (5!:5 <'r') , ') -: ' , y 
()
NB. dyad forks
forms =: <@".;._2 ] 0 : 0
'1 $ 0'  NB. IP IP
'(1 $ ]) 0'  NB. NIP IP
'(1 $~ ]) 0'  NB. IP NIP
'1 (+ ] $) 0'  NB. NIP NIP
()

types =: , (<;._1 ' 0 9!:_3 [ ] 9!:_3@[ 9!:_3@]') ('(' , [ , ' (9!:_3) ' , ')' ,~ ])&.>/ (<;._1 ' 9!:_3 [ ] 9!:_3@[ 9!:_3@]') 

> testres , types (('$'&;)@[stringreplace ])&.>/ forms
NB. monad forks
types1 =: , (<;._1 ' 0 9!:_3 ]') ('(' , [ , ' (9!:_3) ' , ')' ,~ ])&.>/ (<;._1 ' 9!:_3 ]') 

forms1 =: <@".;._2 ] 0 : 0
'$ 0'  NB. IP
'(* ] $) 0'  NB. NIP
()
> testres , types1 (('$'&;)@[stringreplace ])&.>/ forms1
NB. hooks/@:/&:
types2 =: '(9!:_3)@:(9!:_3)';'(9!:_3)&:(9!:_3)';'((9!:_3) (9!:_3))'
forms2 =: <@".;._2 ] 0 : 0
'1 $ 0'  NB. IP IP
'(1 $ ]) 0'  NB. NIP IP
'(1 $~ ]) 0'  NB. IP NIP
'1 (+ ] $) 0'  NB. NIP NIP
'$ 0'  NB. IP
'(* ] $) 0'  NB. NIP
()
> testres , types2 (('$'&;)@[stringreplace ])&.>/ forms2

)

NB. The generated testcases:

NB. dyad forks
3 : 0 ''   NB. These values generated on 64-bit system
P =. <. 2 ^ IF64 { 30 62  NB. PERMANENT
I =. >: +: - P  NB. INPLACE
assert. ((<P;0),(<01),<(<I),(<P;1),(<3),<P;0) -: 1 (0 (9!:_3) 9!:_3) 0
assert. ((<P;0),(<01),<(<I),(<P;1),(<01),<P;0) -: (1 (0 (9!:_3) 9!:_3) ]) 0
assert. ((<P;0),(<01),<(<I),(<P;0),(<2),<P;1) -: (1 (0 (9!:_3) 9!:_3)~ ]) 0
assert. ((<P;0),(<01),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] (0 (9!:_3) 9!:_3)) 0
assert. ((<P;0),(<01),<P;1) -: 1 (0 (9!:_3) [) 0
assert. ((<P;0),(<00),<P;1) -: (1 (0 (9!:_3) [) ]) 0
assert. ((<P;0),(<01),<P;0) -: (1 (0 (9!:_3) [)~ ]) 0
assert. ((<P;0),(<00),<P;1) -: 1 (+ ] (0 (9!:_3) [)) 0
assert. ((<P;0),(<01),<P;0) -: 1 (0 (9!:_3) ]) 0
assert. ((<P;0),(<01),<P;0) -: (1 (0 (9!:_3) ]) ]) 0
assert. ((<P;0),(<00),<P;1) -: (1 (0 (9!:_3) ])~ ]) 0
assert. ((<P;0),(<00),<P;0) -: 1 (+ ] (0 (9!:_3) ])) 0
assert. ((<P;0),(<01),<(<I),(<01),<P;1) -: 1 (0 (9!:_3) 9!:_3@[) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;1) -: (1 (0 (9!:_3) 9!:_3@[) ]) 0
assert. ((<P;0),(<01),<(<I),(<01),<P;0) -: (1 (0 (9!:_3) 9!:_3@[)~ ]) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;1) -: 1 (+ ] (0 (9!:_3) 9!:_3@[)) 0
assert. ((<P;0),(<01),<(<I),(<01),<P;0) -: 1 (0 (9!:_3) 9!:_3@]) 0
assert. ((<P;0),(<01),<(<I),(<01),<P;0) -: (1 (0 (9!:_3) 9!:_3@]) ]) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;1) -: (1 (0 (9!:_3) 9!:_3@])~ ]) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;0) -: 1 (+ ] (0 (9!:_3) 9!:_3@])) 0
assert. ((<(<I),(<P;1),(<3),<P;0),(<3),<(<I),(<P;1),(<00),<P;0) -: 1 (9!:_3 (9!:_3) 9!:_3) 0
assert. ((<(<I),(<P;1),(<01),<P;0),(<3),<(<I),(<P;1),(<00),<P;0) -: (1 (9!:_3 (9!:_3) 9!:_3) ]) 0
assert. ((<(<I),(<P;0),(<2),<P;1),(<3),<(<I),(<P;0),(<00),<P;1) -: (1 (9!:_3 (9!:_3) 9!:_3)~ ]) 0
assert. ((<(<I),(<P;1),(<00),<P;0),(<3),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] (9!:_3 (9!:_3) 9!:_3)) 0
assert. ((<(<I),(<P;1),(<01),<P;0),(<3),<P;1) -: 1 (9!:_3 (9!:_3) [) 0
assert. ((<(<I),(<P;1),(<01),<P;0),(<2),<P;1) -: (1 (9!:_3 (9!:_3) [) ]) 0
assert. ((<(<I),(<P;0),(<00),<P;1),(<3),<P;0) -: (1 (9!:_3 (9!:_3) [)~ ]) 0
assert. ((<(<I),(<P;1),(<00),<P;0),(<2),<P;1) -: 1 (+ ] (9!:_3 (9!:_3) [)) 0
assert. ((<(<I),(<P;1),(<2),<P;0),(<3),<P;0) -: 1 (9!:_3 (9!:_3) ]) 0
assert. ((<(<I),(<P;1),(<00),<P;0),(<3),<P;0) -: (1 (9!:_3 (9!:_3) ]) ]) 0
assert. ((<(<I),(<P;0),(<2),<P;1),(<2),<P;1) -: (1 (9!:_3 (9!:_3) ])~ ]) 0
assert. ((<(<I),(<P;1),(<00),<P;0),(<2),<P;0) -: 1 (+ ] (9!:_3 (9!:_3) ])) 0
assert. ((<(<I),(<P;1),(<3),<P;0),(<3),<(<I),(<00),<P;1) -: 1 (9!:_3 (9!:_3) 9!:_3@[) 0
assert. ((<(<I),(<P;1),(<01),<P;0),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3 (9!:_3) 9!:_3@[) ]) 0
assert. ((<(<I),(<P;0),(<2),<P;1),(<3),<(<I),(<00),<P;0) -: (1 (9!:_3 (9!:_3) 9!:_3@[)~ ]) 0
assert. ((<(<I),(<P;1),(<00),<P;0),(<3),<(<I),(<00),<P;1) -: 1 (+ ] (9!:_3 (9!:_3) 9!:_3@[)) 0
assert. ((<(<I),(<P;1),(<3),<P;0),(<3),<(<I),(<00),<P;0) -: 1 (9!:_3 (9!:_3) 9!:_3@]) 0
assert. ((<(<I),(<P;1),(<01),<P;0),(<3),<(<I),(<00),<P;0) -: (1 (9!:_3 (9!:_3) 9!:_3@]) ]) 0
assert. ((<(<I),(<P;0),(<2),<P;1),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3 (9!:_3) 9!:_3@])~ ]) 0
assert. ((<(<I),(<P;1),(<00),<P;0),(<3),<(<I),(<00),<P;0) -: 1 (+ ] (9!:_3 (9!:_3) 9!:_3@])) 0
assert. ((<P;1),(<3),<(<I),(<P;1),(<01),<P;0) -: 1 ([ (9!:_3) 9!:_3) 0
assert. ((<P;1),(<01),<(<I),(<P;1),(<01),<P;0) -: (1 ([ (9!:_3) 9!:_3) ]) 0
assert. ((<P;0),(<3),<(<I),(<P;0),(<00),<P;1) -: (1 ([ (9!:_3) 9!:_3)~ ]) 0
assert. ((<P;1),(<01),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] ([ (9!:_3) 9!:_3)) 0
assert. ((<P;1),(<3),<P;1) -: 1 ([ (9!:_3) [) 0
assert. ((<P;1),(<00),<P;1) -: (1 ([ (9!:_3) [) ]) 0
assert. ((<P;0),(<3),<P;0) -: (1 ([ (9!:_3) [)~ ]) 0
assert. ((<P;1),(<00),<P;1) -: 1 (+ ] ([ (9!:_3) [)) 0
assert. ((<P;1),(<3),<P;0) -: 1 ([ (9!:_3) ]) 0
assert. ((<P;1),(<01),<P;0) -: (1 ([ (9!:_3) ]) ]) 0
assert. ((<P;0),(<2),<P;1) -: (1 ([ (9!:_3) ])~ ]) 0
assert. ((<P;1),(<00),<P;0) -: 1 (+ ] ([ (9!:_3) ])) 0
assert. ((<P;1),(<3),<(<I),(<00),<P;1) -: 1 ([ (9!:_3) 9!:_3@[) 0
assert. ((<P;1),(<01),<(<I),(<00),<P;1) -: (1 ([ (9!:_3) 9!:_3@[) ]) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;0) -: (1 ([ (9!:_3) 9!:_3@[)~ ]) 0
assert. ((<P;1),(<01),<(<I),(<00),<P;1) -: 1 (+ ] ([ (9!:_3) 9!:_3@[)) 0
assert. ((<P;1),(<3),<(<I),(<01),<P;0) -: 1 ([ (9!:_3) 9!:_3@]) 0
assert. ((<P;1),(<01),<(<I),(<01),<P;0) -: (1 ([ (9!:_3) 9!:_3@]) ]) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;1) -: (1 ([ (9!:_3) 9!:_3@])~ ]) 0
assert. ((<P;1),(<01),<(<I),(<00),<P;0) -: 1 (+ ] ([ (9!:_3) 9!:_3@])) 0
assert. ((<P;0),(<3),<(<I),(<P;1),(<2),<P;0) -: 1 (] (9!:_3) 9!:_3) 0
assert. ((<P;0),(<3),<(<I),(<P;1),(<00),<P;0) -: (1 (] (9!:_3) 9!:_3) ]) 0
assert. ((<P;1),(<01),<(<I),(<P;0),(<2),<P;1) -: (1 (] (9!:_3) 9!:_3)~ ]) 0
assert. ((<P;0),(<01),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] (] (9!:_3) 9!:_3)) 0
assert. ((<P;0),(<3),<P;1) -: 1 (] (9!:_3) [) 0
assert. ((<P;0),(<2),<P;1) -: (1 (] (9!:_3) [) ]) 0
assert. ((<P;1),(<01),<P;0) -: (1 (] (9!:_3) [)~ ]) 0
assert. ((<P;0),(<00),<P;1) -: 1 (+ ] (] (9!:_3) [)) 0
assert. ((<P;0),(<3),<P;0) -: 1 (] (9!:_3) ]) 0
assert. ((<P;0),(<3),<P;0) -: (1 (] (9!:_3) ]) ]) 0
assert. ((<P;1),(<00),<P;1) -: (1 (] (9!:_3) ])~ ]) 0
assert. ((<P;0),(<00),<P;0) -: 1 (+ ] (] (9!:_3) ])) 0
assert. ((<P;0),(<3),<(<I),(<01),<P;1) -: 1 (] (9!:_3) 9!:_3@[) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;1) -: (1 (] (9!:_3) 9!:_3@[) ]) 0
assert. ((<P;1),(<01),<(<I),(<01),<P;0) -: (1 (] (9!:_3) 9!:_3@[)~ ]) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;1) -: 1 (+ ] (] (9!:_3) 9!:_3@[)) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;0) -: 1 (] (9!:_3) 9!:_3@]) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;0) -: (1 (] (9!:_3) 9!:_3@]) ]) 0
assert. ((<P;1),(<01),<(<I),(<00),<P;1) -: (1 (] (9!:_3) 9!:_3@])~ ]) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;0) -: 1 (+ ] (] (9!:_3) 9!:_3@])) 0
assert. ((<(<I),(<01),<P;1),(<3),<(<I),(<P;1),(<01),<P;0) -: 1 (9!:_3@[ (9!:_3) 9!:_3) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<P;1),(<01),<P;0) -: (1 (9!:_3@[ (9!:_3) 9!:_3) ]) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<P;0),(<00),<P;1) -: (1 (9!:_3@[ (9!:_3) 9!:_3)~ ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] (9!:_3@[ (9!:_3) 9!:_3)) 0
assert. ((<(<I),(<00),<P;1),(<3),<P;1) -: 1 (9!:_3@[ (9!:_3) [) 0
assert. ((<(<I),(<00),<P;1),(<2),<P;1) -: (1 (9!:_3@[ (9!:_3) [) ]) 0
assert. ((<(<I),(<00),<P;0),(<3),<P;0) -: (1 (9!:_3@[ (9!:_3) [)~ ]) 0
assert. ((<(<I),(<00),<P;1),(<2),<P;1) -: 1 (+ ] (9!:_3@[ (9!:_3) [)) 0
assert. ((<(<I),(<01),<P;1),(<3),<P;0) -: 1 (9!:_3@[ (9!:_3) ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<P;0) -: (1 (9!:_3@[ (9!:_3) ]) ]) 0
assert. ((<(<I),(<01),<P;0),(<2),<P;1) -: (1 (9!:_3@[ (9!:_3) ])~ ]) 0
assert. ((<(<I),(<00),<P;1),(<2),<P;0) -: 1 (+ ] (9!:_3@[ (9!:_3) ])) 0
assert. ((<(<I),(<01),<P;1),(<3),<(<I),(<00),<P;1) -: 1 (9!:_3@[ (9!:_3) 9!:_3@[) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3@[ (9!:_3) 9!:_3@[) ]) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;0) -: (1 (9!:_3@[ (9!:_3) 9!:_3@[)~ ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<00),<P;1) -: 1 (+ ] (9!:_3@[ (9!:_3) 9!:_3@[)) 0
assert. ((<(<I),(<01),<P;1),(<3),<(<I),(<01),<P;0) -: 1 (9!:_3@[ (9!:_3) 9!:_3@]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<01),<P;0) -: (1 (9!:_3@[ (9!:_3) 9!:_3@]) ]) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3@[ (9!:_3) 9!:_3@])~ ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<00),<P;0) -: 1 (+ ] (9!:_3@[ (9!:_3) 9!:_3@])) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<P;1),(<2),<P;0) -: 1 (9!:_3@] (9!:_3) 9!:_3) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<P;1),(<00),<P;0) -: (1 (9!:_3@] (9!:_3) 9!:_3) ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<P;0),(<2),<P;1) -: (1 (9!:_3@] (9!:_3) 9!:_3)~ ]) 0
assert. ((<(<I),(<00),<P;0),(<3),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] (9!:_3@] (9!:_3) 9!:_3)) 0
assert. ((<(<I),(<01),<P;0),(<3),<P;1) -: 1 (9!:_3@] (9!:_3) [) 0
assert. ((<(<I),(<01),<P;0),(<2),<P;1) -: (1 (9!:_3@] (9!:_3) [) ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<P;0) -: (1 (9!:_3@] (9!:_3) [)~ ]) 0
assert. ((<(<I),(<00),<P;0),(<2),<P;1) -: 1 (+ ] (9!:_3@] (9!:_3) [)) 0
assert. ((<(<I),(<00),<P;0),(<3),<P;0) -: 1 (9!:_3@] (9!:_3) ]) 0
assert. ((<(<I),(<00),<P;0),(<3),<P;0) -: (1 (9!:_3@] (9!:_3) ]) ]) 0
assert. ((<(<I),(<00),<P;1),(<2),<P;1) -: (1 (9!:_3@] (9!:_3) ])~ ]) 0
assert. ((<(<I),(<00),<P;0),(<2),<P;0) -: 1 (+ ] (9!:_3@] (9!:_3) ])) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<01),<P;1) -: 1 (9!:_3@] (9!:_3) 9!:_3@[) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3@] (9!:_3) 9!:_3@[) ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<01),<P;0) -: (1 (9!:_3@] (9!:_3) 9!:_3@[)~ ]) 0
assert. ((<(<I),(<00),<P;0),(<3),<(<I),(<00),<P;1) -: 1 (+ ] (9!:_3@] (9!:_3) 9!:_3@[)) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;0) -: 1 (9!:_3@] (9!:_3) 9!:_3@]) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;0) -: (1 (9!:_3@] (9!:_3) 9!:_3@]) ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3@] (9!:_3) 9!:_3@])~ ]) 0
assert. ((<(<I),(<00),<P;0),(<3),<(<I),(<00),<P;0) -: 1 (+ ] (9!:_3@] (9!:_3) 9!:_3@])) 0

NB. monads
assert. ((<P;0),(<01),<(<I),(<01),<P;0) -: (0 (9!:_3) 9!:_3) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;0) -: (* ] (0 (9!:_3) 9!:_3)) 0
assert. ((<P;0),(<01),<P;0) -: (0 (9!:_3) ]) 0
assert. ((<P;0),(<00),<P;0) -: (* ] (0 (9!:_3) ])) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;0) -: (9!:_3 (9!:_3) 9!:_3) 0
assert. ((<(<I),(<00),<P;0),(<3),<(<I),(<00),<P;0) -: (* ] (9!:_3 (9!:_3) 9!:_3)) 0
assert. ((<(<I),(<00),<P;0),(<3),<P;0) -: (9!:_3 (9!:_3) ]) 0
assert. ((<(<I),(<00),<P;0),(<2),<P;0) -: (* ] (9!:_3 (9!:_3) ])) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;0) -: (] (9!:_3) 9!:_3) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;0) -: (* ] (] (9!:_3) 9!:_3)) 0
assert. ((<P;0),(<3),<P;0) -: (] (9!:_3) ]) 0
assert. ((<P;0),(<00),<P;0) -: (* ] (] (9!:_3) ])) 0

NB. hook/@:/&:
assert. ((<3),<(<I),(<P;1),(<3),<P;0) -: 1 (9!:_3)@:(9!:_3) 0
assert. ((<01),<(<I),(<P;1),(<01),<P;0) -: (1 (9!:_3)@:(9!:_3) ]) 0
assert. ((<3),<(<I),(<P;0),(<2),<P;1) -: (1 (9!:_3)@:(9!:_3)~ ]) 0
assert. ((<01),<(<I),(<P;1),(<00),<P;0) -: 1 (+ ] (9!:_3)@:(9!:_3)) 0
assert. ((<01),<(<I),(<01),<P;0) -: (9!:_3)@:(9!:_3) 0
assert. ((<01),<(<I),(<00),<P;0) -: (* ] (9!:_3)@:(9!:_3)) 0
assert. ((<(<I),(<01),<P;1),(<3),<(<I),(<01),<P;0) -: 1 (9!:_3)&:(9!:_3) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<01),<P;0) -: (1 (9!:_3)&:(9!:_3) ]) 0
assert. ((<(<I),(<01),<P;0),(<3),<(<I),(<00),<P;1) -: (1 (9!:_3)&:(9!:_3)~ ]) 0
assert. ((<(<I),(<00),<P;1),(<3),<(<I),(<00),<P;0) -: 1 (+ ] (9!:_3)&:(9!:_3)) 0
assert. ((<01),<(<I),(<01),<P;0) -: (9!:_3)&:(9!:_3) 0
assert. ((<01),<(<I),(<00),<P;0) -: (* ] (9!:_3)&:(9!:_3)) 0
assert. ((<P;1),(<3),<(<I),(<01),<P;0) -: 1 ((9!:_3) (9!:_3)) 0
assert. ((<P;1),(<01),<(<I),(<01),<P;0) -: (1 ((9!:_3) (9!:_3)) ]) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;1) -: (1 ((9!:_3) (9!:_3))~ ]) 0
assert. ((<P;1),(<01),<(<I),(<00),<P;0) -: 1 (+ ] ((9!:_3) (9!:_3))) 0
assert. ((<P;0),(<3),<(<I),(<00),<P;0) -: ((9!:_3) (9!:_3)) 0
assert. ((<P;0),(<01),<(<I),(<00),<P;0) -: (* ] ((9!:_3) (9!:_3))) 0
1
)



4!:55 ;:'a a1 aad aad2 b f f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 f13 global i ipexp jdlast l0 l1 local max min nmm nb qd t test testa unsafename undefinedname undefinedname2 x'



epilog''

