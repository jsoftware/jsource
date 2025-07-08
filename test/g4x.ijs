prolog './g4x.ijs'
NB. 4!:0 ----------------------------------------------------------------

jnc   =: 4!:0
jnl   =: 4!:1
erase =: 4!:55
ttt   =: jnl i. 4
nounx =: ?3 4$1000
verbx =: +/ % #
advx  =: 1 : 'u^:_1'
conjx =: ^:

1 [ erase ;:'x y'

0  -: jnc <'nounx'
1  -: jnc <'advx'
2  -: jnc <'conjx'
3  -: jnc <'verbx'
_1 -: jnc <'nonexistent'
_2 -: jnc <'@#$!invalid'
0 1 2 3 _1 _2 -: jnc <;._1 ' nounx advx conjx verbx nonexistent @#$!invalid'

a     =:'abc'
plus  =: 3+4
f     =: 3 : ('a=.9'; 'plus=.+'; 'jnc ''a'';''plus''')
0 0   -: jnc 'a';'plus'
0 3   -: f 0
0 3 0 -: jnc 'a';'f';'a'
3 3   -: jnc 'jnl';'jnc'

nm =: ;:'x y'
_1  0 -:     (3 : 'jnc nm'      ) 8
0   0 -: 7   (3 : (':';'jnc nm')) 8
nm =: ;:'u v'
3  _1 -: +   (1 : 'jnc nm')
0  _1 -: 'a' (1 : 'jnc nm')
3  3  -: +   (2 : 'jnc nm') -
3  0  -: +   (2 : 'jnc nm') 'b'
0  3  -: 'b' (2 : 'jnc nm') -
0  0  -: 'a' (2 : 'jnc nm') 'b'

_2 _2 _2 -: jnc <;._1 ' 9 +-1aber *'
_1       -: jnc <'asflkjasasdf'
_2       -: jnc <'+*abc'
_2       -: jnc <'abc\def'

a =: 18!:3 ''
_1       -: jnc <'undefname__a'
'' [ 18!:55 a
_2       -: jnc <'undefname__a'

nounx =: 2
verbx =: +/ . *
advx  =: /
conjx =: .
0  3 1  2 -: jnc 'nounx';'verbx';'advx';'conjx'
erase 'verbx';'conjx'
0 _1 1 _1 -: jnc 'nounx';'verbx';'advx';'conjx'

a=:/
b=:0
c=:&
v=:+/
erase <'z'


NB. 4!:1 ----------------------------------------------------------------

jnc   =: 4!:0
jnl   =: 4!:1
erase =: 4!:55

nounx =: 2
verbx =: +/ . *
advx  =: /
conjx =: .
(<'nounx') e. jnl 0
(<'verbx') e. jnl 3
(<'advx' ) e. jnl 1
(<'conjx') e. jnl 2
(jnl 0) -: jnl (>:?20)$0
(jnl 1) -: jnl (>:?20)$1
(jnl 2) -: jnl (>:?20)$2
(jnl 3) -: jnl (>:?20)$3
(jnl i.4) -: jnl 3 2 1 0,?10$4
(;:'nounx verbx advx conjx') e. jnl i.4
(;:'nounx verbx advx conjx') e. a. jnl i.4
(;:'nounx verbx')    e. 'nv' jnl i.4
(;:'advx conjx' ) -.@e. 'nv' jnl i.4
erase 'verbx';'conjx'
(;:'verbx conjx') -.@e. jnl i.4
(;:'nounx advx' )    e. jnl i.4

(<,'y'  ) e.   (3 : 'jnl 0') 9
(;:'x y') e. 8 (3 : (':';'jnl 0')) 9

(;:'u'  ) e.   + (1 : 'jnl 3')
(;:'u m'  ) e. 'a' (1 : 'jnl 0')
(;:'u v') e.   + (2 : 'jnl 3') -
(;:'u m'  ) e. 'a' (2 : 'jnl 0') -
(;:'v n'  ) e.   + (2 : 'jnl 0') 'b'

t =: jnl i.4
1 = #$t
32 -: type <'t'
*./ 2  =  type&>t
*./ 1  =  #&$&>t
t  -: t/:>t
*./ (jnc t) e. i.4


NB. 4!:55 ---------------------------------------------------------------

jnc   =: 4!:0
jnl   =: 4!:1
erase =: 4!:55

nounx =: 2
verbx =: +/ . *
advx  =: /
conjx =: .
nm   =: ;:'nounx verbx advx conjx'
f    =: 3 : ('".y'; 'b=.jnc nm'; 'erase nm'; 'a=.jnc nm'; 'b,a')
 0  3  1  2 -: jnc nm
 3  3  3  3  0  3  1  2 -: f '(nounx=.verbx=.advx=.conjx=.*) 12'
 0  3  1  2 -: jnc nm
 0  0  3  2  0  3  1 _1 -: f '(advx=.*) nounx=.verbx=.12'
 0  3  1 _1 -: jnc nm
 0  0  1 _1  0  3 _1 _1 -: f 'nounx=.verbx=.12'
 0  3 _1 _1 -: jnc nm
 2  3 _1 _1  0 _1 _1 _1 -: f '3(nounx=. &)+ 4'
 0 _1 _1 _1 -: jnc nm
 0 _1 _1 _1 _1 _1 _1 _1 -: f '12'
_1 _1 _1 _1 -: jnc nm

erase ;:'x y'
g =: 3 : (t,':'; t =: 'yy=.y'; 'b=.jnc yy'; 'erase yy'; 'a=.jnc yy'; 'b,a')
 0 _1 -:   g <'y'
_1 _1 -:   g <'x'
 0 _1 -: 2 g <'y'
 0 _1 -: 2 g <'x'

nounx =: 2
verbx =: +/ . *
advx  =: /
conjx =: .
 0  3  1  2 _1 -: jnc ;:'nounx verbx advx conjx undef'
 1  1  1  1  1 -: t=:erase ;:'nounx verbx advx conjx undef'
_1 _1 _1 _1 _1 -: jnc ;:'nounx verbx advx conjx undef'

0 -: erase <'*&^invalid@'

a =: 9
1 1 0 0 0 -: t=:erase 'a'; 'undefined'; 'in+valid'; '8.'; 'a.'

alpha =: i.3 4
beta  =: ;:'Cogito, ergo sum.'
gamma =: !

1 1 1 1 0 0 -: t=:erase ;:'alpha beta boo hoo + +:'

NB. Verify stacked value not deleted
a =: 100
100 -: (([  [: >:@i.&.> i.)@[   [: 4!:55 (<'a')"_) a
f =: 3 : 0
a =. >:y  NB. need a new block to get usecount of 1
100 -: (([  [: >:@i.&.> i.)@[   [: 4!:55 (<'a')"_) a
)
f 99
NB. Verify space is freed
a =: i. 1e6
b =: 7!:0''
1: 4!:55 <'a'
b > 2e6+7!:0''
3 : 0 ''
a =. i. 1e6
b =. 7!:0''
1: 4!:55 <'a'
b > 2e6+7!:0''
)

{{ a =. b =. c =. d =. f =. <'abc'
nm =. ;:'nm a b c d e f'
4!:55 nm
}} ''   NB. 4!:55 protects its arg

'xyz' -: {{ a =. 'xyz' [ b =. c =. d =. f =. <'abc'
nm =. ;:'nm a b c d e f'
((4!:55 nm) ] ]) a
}} ''   NB. 4!:55 protects the stack

'4!:55 <''a''' -: {{
 a =. '4!:55 <''a'''
 ((". a) ] ]) a }} ''  NB. ". protects its arg

5 -: {{
 a =. 2+3 [ b =. 2 3
 ((". 'a =. b') ] ]) a }} ''  NB. ". protects the stack

5 -: {{
 a =. 2+3 [ b =. 2 3
 ((a =. b) ] ]) a }} ''  NB. =. protects the stack




NB. name_:
a =: i. 1e6
b =: 7!:0''
1e6 -: # a_:
b > 2e6+7!:0''
'value error' -: ". etx 'a + 5'
a =: i. 1e6
b =: 7!:0''
1e6 -: ". '# a_:'  NB. not deleted
b < 1e6+7!:0''
1e6 -: # a
a =: i. 1e6
b =: 7!:0''
1e6 1e6 -: (# a_:) , # a   NB. deleted, stack deferred
b > 2e6+7!:0''
'value error' -: ". etx 'a + 5'

1 -: 3 : 0 ''
a =. i. 1e6
b =. 7!:0''
assert. 1e6 -: # a_:
for. i. 6 do. b end.
assert. b > 2e6+7!:0''
assert. 'value error' -: ". etx 'a + 5'
a =. i. 1e6
b =. 7!:0''
assert. 1e6 -: ". '# a_:'  NB. not deleted inside ".
for. i. 6 do. b end.
assert. b < 1e6+7!:0''
assert. 1e6 -: # a
a =. i. 1e6
b =. 7!:0''
1e6 1e6 -: (# a_:) , # a
assert. 'value error' -: ". etx 'a + 5'
a =. i. 1e6
assert. 2e6 -: # a_: , a   NB. deleted, stack deferred
a =. i. 1e6
b =. 7!:0''
assert. 1e6 1e6 -: (# a_:) , , # a   NB. deleted, stack deferred
for. i. 6 do. b end.
assert. b > 2e6+7!:0''
assert. 'value error' -: ". etx 'a + 5'
a =. 1
assert. a_:
this1 =: 1
assert. this1_:
this1 =: 1
this1_:
)
20 -: ([ i."0@i.) {{ y_: }} 20
a =: 5
'|domain error: ex02|   a:    +a_:' -:&(_11&{.) ". eftx 'a: + a_:'
4!:55 <'a'
'|value error: ex02|   a:+    a_:' -:&(_11&{.) ". eftx 'a: + a_:'
'value error' -: ". eftxs 'a: + a_:'

a=:b=:0
4!:55 ;:'a undefname b'
_1 = 4!:0 ;:'a undefname b'
13!:0 (1)
a=.b=.0
4!:55 ;:'a undefname b'
_1 = 4!:0 ;:'a undefname b'
13!:0 (0)

NB. 4!: -----------------------------------------------------------------

jnc   =: 4!:0
jnl   =: 4!:1
scind =: 4!:4
erase =: 4!:55

_2 -: jnc    <''
0  -: erase <''

'domain error'    -: jnc etx 1
'domain error'    -: jnc etx 1 2
'domain error'    -: jnc etx 3 3.5
'domain error'    -: jnc etx 'abcvz*'
'domain error'    -: jnc etx 3j4
'domain error'    -: jnc etx 'a';1
'domain error'    -: jnc etx 'a';1 2
'domain error'    -: jnc etx 'a';3 3.5
'domain error'    -: jnc etx 'a';3j4
'domain error'    -: jnc etx <<'ab'
'domain error'    -: jnc etx i.3 4
'valence error'    -: 'abc' jnc etx 3  

'rank error'      -: jnc etx <3 4$'a'

'domain error'    -: jnl etx 'abc'    
'domain error'    -: jnl etx 2 3j4    
'domain error'    -: jnl etx 1 1.5    
'domain error'    -: jnl etx <1 2     
'domain error'    -: jnl etx 0 1 2 _1 3
'domain error'    -: jnl etx _2 _1
'domain error'    -: jnl etx 6 99999 0     

'domain error'    -: 2 3      jnl etx 3
'domain error'    -: 2.5      jnl etx 3
'domain error'    -: 3j54     jnl etx 3
'domain error'    -: (<'abc') jnl etx 3

'domain error'    -: erase etx 1      
'domain error'    -: erase etx 1 2    
'domain error'    -: erase etx 3 3.5  
'domain error'    -: erase etx 'abc'    
'domain error'    -: erase etx 3j4    
'domain error'    -: erase etx 'a';1      
'domain error'    -: erase etx 'a';1 2    
'domain error'    -: erase etx 'a';3 3.5  
'domain error'    -: erase etx 'a';3j4    
'domain error'    -: erase etx <<'ab'        

'rank error'      -: erase etx <3 4$'a'

'domain error'    -: scind etx 1 2 3
'domain error'    -: scind etx 'abc123'
'domain error'    -: scind etx 1 2 3;'abc'
'domain error'    -: scind etx <<'abc'

'ill-formed name' -: scind etx <'+*abc'
'ill-formed name' -: scind etx <'abc\def'
'rank error'    -: scind etx <2 0$''
_1    -: scind etx <''

'rank error'      -: scind etx <3 4$'xab'
'rank error'      -: scind etx <,:'xab'

 0 < x =: 3 : '4!:4 <y' 'y'

_1 = 4!:4 <'a__undefname'

NB. locales -------------------------------------------------------------

jnc    =: 4!:0
jnl    =: 4!:1
erase =: 4!:55

x =: ?13$100000
y =: 'supercalifragilisticespialidocious'
abcd =: x
abcd_locale_ =: y
abcd -: x
abcd_locale_ -: y
0 0 -: jnc ;:'abcd abcd_locale_'
(<'locale') e. jnl 6
(<'locale') e. 'l' jnl 6
-. (<'locale') e. 'abc' jnl 6
1=+/(jnl 0) e. <'abcd'
erase <'abcd_locale_'
abcd -: x
0 _1 -: jnc ;:'abcd abcd_locale_'

x =: 'sui generis'
ab_cd =. x
ab_cd_asdf_ =. +/ % #
ab_cd -: x
9.5 -: ab_cd_asdf_ i.20
0 3 -: jnc ;:'ab_cd ab_cd_asdf_'
(<'asdf') e. jnl 6
(<'asdf') e. 'abc' jnl 6
-. (<'asdf') e. 'xyz' jnl 6
1=+/(jnl 0) e. <'ab_cd'
erase <'ab_cd_asdf_'
ab_cd -: x
0 _1 -: jnc ;:'ab_cd ab_cd_asdf_'
1=+/(jnl 0 3) e. <'ab_cd'

exec_foo_ =: ".
1 [ exec_foo_ 'xy =: i.3 4'
xy_foo_ -: i.3 4
1 [ exec_foo_ 'ces__ =: ;:''Cogito, ergo sum.'''
ces -: ;:'Cogito, ergo sum.'
erase ;:'exec_foo_ xy_foo_ ces__'

'ill-formed name' -: ex 'a_      =: 12'
'ill-formed name' -: ex 'abcd_   =: 12'

'domain error'    -: ex '_  =: 12'
'domain error'    -: ex '__ =: 12'

NB. 'nonce error'     -: 3 : 'abc_d_ =. 17' etx 17

(271828) 18!:55 ;:'asdf foo locale'


NB. z locale ------------------------------------------------------------

erase ;:'abc abc_z_ sum sum_z_'
x=: o.?3 4$1000
abc__ =: x
abc__ -: x
abc   -: x
y =: j./?2 5 4$1000
abc_z_ =: y
abc_z_ -: y
abc_nonexist_ -: y
abc__ -: x
abc   -: x
sum_z_ =: +/
(+/y) -: sum y
(+/y) -: sum__ y
(+/y) -: sum_z_ y
(+/y) -: sum_nonexist_ y
ces_exist_ =: ;:'Cogito, ergo sum.'
3 3 3 3 -: jnc ;:'sum sum_exist_ sum_nonexist_ sum_z_'
-. (<'sum') e. jnl 3
jnc_z_ =: jnc f.
jnl_z_ =: jnl f.
3 -: jnc_z_ <'sum'
(<'sum') e. jnl_z_ 3
erase ;:'abc abc_z_ ces_exist_ sum_z_'

(271828) 18!:55 ;:'exist nonexist'

NB. Script lists --------------------------------------------------------

NB. obsolete (4!:3 =&# 4!:8) ''
this1=: 'this1'
si_g4x=: 4!:4 <'this1'
si_g4x e. i.#4!:3 ''
'g4x.ijs'-:tolower _7{.>si_g4x{4!:3 ''

empty 4!:7 si_sn [ this2=: 'this2' [ si_sn=: 4!:7 ] 4!:6 sn=. 'scriptname',":?1e6
sn-:>(4!:4 <'this2'){4!:3 ''
NB. obsolete si_g4x=(4!:4 <'this2'){4!:8 ''

NB. delete a name on same line as when it is reassigned
3 : 0 ''
bgn =. 7!:0''
for_i. i.4 do.
 dat=: 1e7$' '
 4!:55<'dat1' [ dat=. dat1 [ 4!:55<'dat' [ dat1=. dat
 4!:55<'dat'
end.
10000 > (7!:0'') - bgn
)



4!:55 ;:'abc_z_ jnc_z_ jnl_z_ sum_z_'



epilog''

