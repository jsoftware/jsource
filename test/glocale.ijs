NB. locatives -----------------------------------------------------------

ab__=: x=: ?20$1e9
x -: ab__
x -: ab_base_
(<'base') e. 4!:1 [6
-.a:      e. 4!:1 [6
(,<,'z') -: 18!:2 <'base'

not_a_locative=: x=: ?4 5$1e9
x -: not_a_locative

'ill-formed name' -: ex 'abc_'

'ill-formed name' -: ex 'abc__d_e_=: 5'
'ill-formed name' -: ex 'abc_34e_ =: 5'
'ill-formed name' -: ex 'abc___d  =: 5'
'ill-formed name' -: ex 'abc_012_ =: 5'


NB. indirect reference --------------------------------------------------

ab_xyz_ =: x =: ?20$1e9
x -: ab_xyz_
indirect=: <'xyz'
x -: ab__indirect
a__indirect=: y=: ?20$1e6
y -: a_xyz_

c=: <'charlie'
b_charlie_=: <'baker'
a__b__c =: y=: ?20$1e6
y -: a__b__c
y -: a_baker_
d=: <'base'
y -: a__b__c__d
f=: 3 : 'a__b__c__local [ local=.<''base'''
y -: f 0

a_z_=: x=: ?20$1e6
x -: a__k [ k=: <'huh'

'value error'     -: ex 'ab__huh'
'value error'     -: ab__k etx [ k=: <'huh'

'rank error'      -: ex 'ab__k' [ k=: 'xyz'
'rank error'      -: ex 'ab__k' [ k=: 3$<'abc'
'rank error'      -: ex 'ab__k' [ k=: 3 4$<'abc'

'length error'    -: ex 'ab__k' [ k=: <''
'length error'    -: ex 'ab__k' [ k=: <$0

'domain error'    -: ex 'ab__k' [ k=: 0 
'domain error'    -: ex 'ab__k' [ k=: 'x'
'domain error'    -: ex 'ab__k' [ k=: 5
'domain error'    -: ex 'ab__k' [ k=: 5.4
'domain error'    -: ex 'ab__k' [ k=: 5j4
'domain error'    -: ex 'ab__k' [ k=: 5x
'domain error'    -: ex 'ab__k' [ k=: 5r4

'domain error'    -: ex 'ab__k' [ k=: <0 1 0
'domain error'    -: ex 'ab__k' [ k=: <i.5
'domain error'    -: ex 'ab__k' [ k=: <3.4 5
'domain error'    -: ex 'ab__k' [ k=: <3j4 5
'domain error'    -: ex 'ab__k' [ k=: <3 4 5x
'domain error'    -: ex 'ab__k' [ k=: <3r4 5
'domain error'    -: ex 'ab__k' [ k=: <<'abc'

'rank error'      -: ex 'ab__k' [ k=: <2 3$'x'

'ill-formed name' -: ex 'ab___'
'ill-formed name' -: ex 'ab__4'
'ill-formed name' -: ex 'ab__4=: 8'
'ill-formed name' -: ex '9+ab__4'
'ill-formed name' -: ex 'ab__4xy'
'ill-formed name' -: ex 'ab__a__5'
'ill-formed name' -: ex 'ab__a___b'
'ill-formed name' -: ex 'ab___cde'

'ill-formed name' -: ex 'ab__k' [ k=: <'*(&!'
'ill-formed name' -: ex 'ab__k' [ k=: <'ab_xyz'
'ill-formed name' -: ex 'ab__k' [ k=: <'ab_xyz_'
'ill-formed name' -: ex 'ab__k' [ k=: <'ab__xyz'

18!:55 ;:'baker charlie huh xyz'


NB. 18!:0 ---------------------------------------------------------------

lnc     =: 18!:0
lcreate =: 18!:3
ldestroy=: 18!:55

0     -: lnc <'base'
0 0   -: lnc <;._1 ' base z'
0 0 1 -: lnc x=:(;:'base z'),lcreate ''
_1 _1 -: lnc 'nonsuch123';'99999999'
_2 _2 -: lnc '!!#*@';'01abc'

ldestroy {:x

'domain error'  -: lnc etx 0 1 0
'domain error'  -: lnc etx 'abc'
'domain error'  -: lnc etx '123'
'domain error'  -: lnc etx 1 2 3
'domain error'  -: lnc etx 1 2.3
'domain error'  -: lnc etx 1 2j3
'domain error'  -: lnc etx 1 2 3x
'domain error'  -: lnc etx 1 2r3

'domain error'  -: lnc etx 'ab';0 1 0
'domain error'  -: lnc etx 'ab';1 2 3
'domain error'  -: lnc etx 'ab';1 2.3
'domain error'  -: lnc etx 'ab';1 2j3
'domain error'  -: lnc etx 'ab';1 2 3x
'domain error'  -: lnc etx 'ab';1 2r3

'domain error'  -: 2 lnc etx <'base'

'length error'  -: lnc etx 'ab';''
'length error'  -: lnc etx 'ab';$0


NB. 18!:1 ---------------------------------------------------------------

lnc =: 18!:0
lnl =: 18!:1

x=:lnl 0 1
1  -: #$x
32 -: type x
x -: /~x
(;:'base z') e. x
2 = type&>x
1 = #@$ &>x

0 = lnc lnl 0
1 = lnc lnl 1

(lnl 0) -: lnl -~2
(lnl 0) -: lnl -~2.5
(lnl 0) -: lnl -~2j5
(lnl 0) -: lnl -~2x
(lnl 0) -: lnl -~2r5

(lnl $0) -: lnl ''
(lnl $0) -: lnl 0$<5

x=: lnl 0 1
(a lnl 0 1) -: (a e.~ {.&>x)#x [ a=: 'j'
(a lnl 0 1) -: (a e.~ {.&>x)#x [ a=: 'j0'
(a lnl 0 1) -: (a e.~ {.&>x)#x [ a=: 'zb'

'domain error'  -: lnl etx 'abc'
'domain error'  -: lnl etx 1 2.3
'domain error'  -: lnl etx 1 2j3
'domain error'  -: lnl etx 1 2 3x
'domain error'  -: lnl etx 1 2r3
'domain error'  -: lnl etx 0;1

'domain error'  -: 0 1 0   lnl etx 0
'domain error'  -: 2 3 4   lnl etx 0
'domain error'  -: 2 3.4   lnl etx 0
'domain error'  -: 2 3j4   lnl etx 0
'domain error'  -: 2 3 4x  lnl etx 0
'domain error'  -: 2 3r4   lnl etx 0
'domain error'  -: (<'abc')lnl etx 0


NB. 18!:2 ---------------------------------------------------------------

lpath=: 18!:2

y=: (+%)/\20$1r1
a_new_ =: y
y -: a_new_
(<;._1 ' first new') lpath <'cool'
y -: a_cool_
a_first_=: x=:'kakistocracy kerygma'
x -: a_cool_

4!:55 ;:'a_new_ a_first_'

(<'a')         lpath <'cool'
('a';'bc';'d') lpath <'new' 
(,<,'a')       -: lpath <'cool'
(;:'a bc d')   -: lpath <'new'

(,<,'z') -: lpath <'NonExistentLocale'
(<'NonExistent') lpath <'abc'
(,<'NonExistent') -: lpath <'abc'
'' -: lpath <'z'

(i.0 0) -: ''    lpath <'asdf'
(i.0 0) -: (i.0) lpath <'asdf'
(i.0 0) -: (0$a:)lpath <'asdf'

18!:55 ;:'a abc asdf bc cool d first new NonExistent NonExistentLocale'

'domain error'    -: lpath etx 0 1 0
'domain error'    -: lpath etx 'abc'
'domain error'    -: lpath etx 2 3 4
'domain error'    -: lpath etx 2 3.4
'domain error'    -: lpath etx 2 3j4
'domain error'    -: lpath etx 2 3x
'domain error'    -: lpath etx 2 3r4
'domain error'    -: lpath etx 2;3 4

'domain error'    -: lpath etx <0 1 0
'domain error'    -: lpath etx <2 3 4
'domain error'    -: lpath etx <2 3.4
'domain error'    -: lpath etx <2 3j4
'domain error'    -: lpath etx <2 3 4x
'domain error'    -: lpath etx <2 3r4
'domain error'    -: lpath etx <<'abc'
'domain error'    -: lpath etx <<'234'

'ill-formed name' -: lpath etx <'!!#+'
'ill-formed name' -: lpath etx <'abc_ju'
'ill-formed name' -: lpath etx <'abc_junk_'
'ill-formed name' -: lpath etx <'abc__j'

'rank error'      -: lpath etx <3 4$'abc'

'length error'    -: lpath etx <''
'length error'    -: lpath etx <$0

'domain error'    -: 0 1 0            lpath etx <'z'
'domain error'    -: 'abc'            lpath etx <'z'
'domain error'    -: 1 2 3            lpath etx <'z'
'domain error'    -: 1 2.3            lpath etx <'z'
'domain error'    -: 1 2j3            lpath etx <'z'
'domain error'    -: 1 2 3x           lpath etx <'z'
'domain error'    -: 1 2r3            lpath etx <'z'

'domain error'    -: ('base';0 1 0)   lpath etx <'z'
'domain error'    -: ('base';1 2 3)   lpath etx <'z'
'domain error'    -: ('base';1 2.3)   lpath etx <'z'
'domain error'    -: ('base';1 2j3)   lpath etx <'z'
'domain error'    -: ('base';1 2x )   lpath etx <'z'
'domain error'    -: ('base';1 2r3)   lpath etx <'z'

'length error'    -: ('base';''   )   lpath etx <'z'
'length error'    -: ('base';$0   )   lpath etx <'z'

'rank error'      -: ('base';2 3$'ab')lpath etx <'z'

'ill-formed name' -: ('base';'!!@$')  lpath etx <'z'
'ill-formed name' -: ('base';'z   ')  lpath etx <'z'
'ill-formed name' -: ('base';'ab_c')  lpath etx <'z'


NB. 18!:3 ---------------------------------------------------------------

lcreate =: 18!:3
ldestroy=: 18!:55
spnow   =: 7!:0

x=: 12345
y=: spnow ''
y=: spnow ''

t=: lcreate ''
t e. 18!:1 [1
0  -: #$t
32 -: type t
2  -: type x=:>t
1  -: #$x
*./ x e. '0123456789'
asdf__t=: i.1e4
18!:55 t,;:'t x'

x=: 12345
x=: spnow ''
(200*1+IF64) > x-y

(<'asdf') -: 8 lcreate <'asdf'
(<'asdf') -: 4 lcreate <'asdf'
x_asdf_=: i.1e4
'locale error'  -: 5 lcreate etx <'asdf'

18!:55 <'asdf'

'domain error'  -: lcreate etx 0 1
'domain error'  -: lcreate etx 234
'domain error'  -: lcreate etx 2.4
'domain error'  -: lcreate etx 2j4
'domain error'  -: lcreate etx 2r4
'domain error'  -: lcreate etx 23x

'domain error'  -: lcreate etx <0 1
'domain error'  -: lcreate etx <234
'domain error'  -: lcreate etx <2.4
'domain error'  -: lcreate etx <2j4
'domain error'  -: lcreate etx <2r4
'domain error'  -: lcreate etx <23x

'domain error'  -: _34 lcreate etx <'asdf'
'domain error'  -: 3.4 lcreate etx <'asdf'
'domain error'  -: 3j4 lcreate etx <'asdf'
'domain error'  -: 3r4 lcreate etx <'asdf'
'domain error'  -: '4' lcreate etx <'asdf'
'domain error'  -: (<4)lcreate etx <'asdf'

'limit error'   -: 256 lcreate etx <'asdf'


NB. 18!:4 ---------------------------------------------------------------

lswitch=: 18!:4

f_a_ =: 3 : 0
 p=. 18!:5 ''
 q=. f1_b_ 0
 r=. 18!:5 ''
 p,q,r
)

f1_b_ =: 3 : 0
 p=. 18!:5 ''
 lswitch_base_ <'asdf'
 q=. 18!:5 ''
 p,q
)

(<;._1 ' a b asdf a') -: x=: f_a_ 0

f_a_ =: 3 : 0
 p=. 18!:5 ''
 q=. f1_b_ 0
 r=. 18!:5 ''
 p,q,r
)

f1_b_ =: 3 : 0
 p=. 18!:5 ''
 q=. f2_c_ 0
 r=. 18!:5 ''
 p,q,r
)

f2_c_ =: 3 : 0
 p=. 18!:5 ''
 18!:4 <'asdf'
 q=. 18!:5 ''
 p,q
)

(<;._1 ' a b c asdf asdf a') -: x=: f_a_ 0

18!:4 <'base'
(<'base') -: 18!:5 ''

lswitch x=:<'NonExistent2'
x_base_ -: 18!:5 ''
x_base_ e. 18!:1 [0
lswitch_base_ <'base'
(<'base') -: 18!:5 ''

lswitch <'base'
plus_a_=: +
4 plus_a_ _3
(<'base') -: 18!:5 ''

18!:55 ;:'a b c asdf NonExistent2'

'domain error'    -: lswitch etx 0
'domain error'    -: lswitch etx 'a'
'domain error'    -: lswitch etx 2
'domain error'    -: lswitch etx 2.3
'domain error'    -: lswitch etx 2j3
'domain error'    -: lswitch etx 2x
'domain error'    -: lswitch etx 2r3

'domain error'    -: lswitch etx <0 1 0
'domain error'    -: lswitch etx <2 3 4
'domain error'    -: lswitch etx <2 3.4
'domain error'    -: lswitch etx <2 3j4
'domain error'    -: lswitch etx <2 3x
'domain error'    -: lswitch etx <2 3r4
'domain error'    -: lswitch etx <<'abc'

'domain error'    -: (<'j') lswitch etx <'abc'

'rank error'      -: lswitch etx <3 4$'a'

'length error'    -: lswitch etx <''
'length error'    -: lswitch etx <$0

'ill-formed name' -: lswitch etx <'!!#+'
'ill-formed name' -: lswitch etx <'abc_ju'
'ill-formed name' -: lswitch etx <'abc_junk_'
'ill-formed name' -: lswitch etx <'abc__j'


NB. 18!:5 ---------------------------------------------------------------

lname=: 18!:5

(<'base') -: lname ''

'rank error'   -: lname etx 0
'rank error'   -: lname etx i.2 3

'length error' -: lname etx 2 3 4
'length error' -: lname etx 'abc'


NB. 18!:55 --------------------------------------------------------------

lcreate =: 18!:3
lname   =: 18!:5
ldestroy=: 18!:55
spnow   =: 7!:0

x=: y=: spnow ''
x=: spnow ''
-.(<'ex1') e. 18!:1 [0
a_ex1_=: i.1e5
extract_ex1_=: 1e4$'pericope'
(<'ex1') e. 18!:1 [0
ldestroy <'ex1'
-.(<'ex1') e. 18!:1 [0
y=: spnow ''
(200*1+IF64) > |x-y

x=: y=: spnow ''
x=: spnow ''
k=: lcreate ''
k e. 18!:1 [1
a__k=: i.1e5
extract__k=: 1e4$'pericope'
ldestroy k
-. k e. 18!:1 [1
'locale error' -: ex '#a__k'
'locale error' -: ex 'a__k=: i.12'
4!:55 <'k'
y=: spnow ''
(200*1+IF64) > |x-y

x=: spnow ''
k=: lcreate"1 i.20 0
ldestroy k
4!:55 <'k'
y=: spnow ''
(200*1+IF64) > |x-y

1 -: ldestroy <'NoNoSuchLocale'

k=: lcreate ''
a__k=: i.12
ldestroy k
'locale error' -: ex 'a__k=: i.9'

'' -: ldestroy ''
'' -: ldestroy i.0
'' -: ldestroy 0$<''

g_a_=: 3 : 0
 z=.   (<,'a') e. 18!:1 [0
 ldestroy_base_ <'a'
 z=. z,(<,'a') e. 18!:1 [0
)

g_a_ 0
-. (<,'a') e. 18!:1 [0

g_b_=: 3 : 0
 z=. ''
 asdf_b_=: x=. ?.1000$1000
 ldestroy_base_ <'b'
 z=.   (<,'b') -: 18!:5 ''
 z=. z,x -: ?.1000$1000
 z=. z,0 -: 4!:0 <'asdf'
 z=. z,0 -: 4!:0 <'asdf_b_'
)

h_b_ =: 3 : 0
 top_b_=: 'cacophemistic'
 z=.  3 0 -: 4!:0 ;:'g_b_ top_b_'
 z=.z,(<,'b') e. 18!:1 [0
 g_b_ 0
 z=.z,3 0 -: 4!:0 ;:'g_b_ top_b_'
 z=.z,(<,'b') e. 18!:1 [0
)

h_b_ 0
-. (<,'b') e. 18!:1 [0
_1 -: 18!:0 <'b'
_1 _1 _1 -: 4!:0  ;:'g_b_ top_b_ h_b_'

f0_loca_ =: 18!:5@i.@0: @ (18!:55)
(;:'base loca') -: (18!:5 ''),f0_loca_ <'loca'
_1 -: 18!:0 <'loca'

f1_locb_ =: 18!:1@i.@2: @ (18!:55)
(<'locb') e. f1_locb_ <'locb'
_1 -: 18!:0 <'locb'

18!:55 ;:'a b loca locb'

'domain error'    -: ldestroy etx 0 1 0
'domain error'    -: ldestroy etx 'abc'
'domain error'    -: ldestroy etx 2 3 4
'domain error'    -: ldestroy etx 2 3.4
'domain error'    -: ldestroy etx 2 3j4
'domain error'    -: ldestroy etx 2 3x
'domain error'    -: ldestroy etx 2 3r4
'domain error'    -: ldestroy etx 2;3 4

'domain error'    -: ldestroy etx <0 1 0
'domain error'    -: ldestroy etx <2 3 4
'domain error'    -: ldestroy etx <2 3.4
'domain error'    -: ldestroy etx <2 3j4
'domain error'    -: ldestroy etx <2 3 4x
'domain error'    -: ldestroy etx <2 3r4
'domain error'    -: ldestroy etx <<'abc'
'domain error'    -: ldestroy etx <<'234'

'ill-formed name' -: ldestroy etx <'!!#+'
'ill-formed name' -: ldestroy etx <'abc_ju'
'ill-formed name' -: ldestroy etx <'abc_junk_'
'ill-formed name' -: ldestroy etx <'abc__j'

'rank error'      -: ldestroy etx <3 4$'abc'

'length error'    -: ldestroy etx <''
'length error'    -: ldestroy etx <$0

'domain error'    -: 3 ldestroy etx <'abc'


NB. locatives and 4!:5 --------------------------------------------------

1 [ 4!:5 [1
a_baker_ =: i.12
xy_z_ =: 99
k=: 18!:3 ''
sum__k=: +/
x=: 4!:5 [1
4!:5 [0
18!:55 k,<'baker'
x -: /:~ ('sum_',(":>k),'_');;:'a_baker_ k_base_ xy_z_'


4!:55 ;:'a a_z_ ab c d f '
4!:55 ;:'indirect k lcreate ldestroy lname lnc lnl lpath lswitch '
4!:55 ;:'not_a_locative spnow t test x xy_z_ y '


