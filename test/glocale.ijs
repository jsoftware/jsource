prolog './glocale.ijs'

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

'domain error'    -: ex 'ab__k' [ k=: 'x'
'locale error'    -: ex 'ab__k' [ k=: 1e6
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

dd =: > d =: 0&".&.> c =: 18!:3 ''
a__c =: 5
5 -: a__d  NB. boxed atomic integer and string access the same locale
5 -: a__dd  NB. atomic integer and string access the same locale
ee =: > e =: 0&".&.> f =: 18!:3 ''
c__c =: d__c =: e
dd__c =: ee
a__e =: 7
7 -: a__c__c
7 -: a__c__d
7 -: a__d__c
7 -: a__d__d
a__ee =: 11
11 -: a__c__c
11 -: a__c__dd
11 -: a__dd__c
11 -: a__dd__dd

18!:55 d,f

NB. 18!:0 ---------------------------------------------------------------

lnc     =: 18!:0
lcreate =: 18!:3
ldestroy=: 18!:55

0     -: lnc <'base'
0 0   -: lnc <;._1 ' base z'
0 0 1 1 -: lnc x=:(;:'base z'),(, 0&".&.>) lcreate ''
_1 _1 -: lnc 'nonsuch123';'99999999'
_2 _2 -: lnc '!!#*@';'01abc'

ldestroy {:x

'domain error'  -: lnc etx 0 1 0
'domain error'  -: lnc etx 'abc'
'domain error'  -: lnc etx '123'
NB. OK 'domain error'  -: lnc etx 1 2 3
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

x=: lnl 0 [ y =: lnl 1
(a lnl 0 1) -: y,(a e.~ {.&>x)#x [ a=: 'j'
(a lnl 0 1) -: y,(a e.~ {.&>x)#x [ a=: 'j0'
(a lnl 0 1) -: y,(a e.~ {.&>x)#x [ a=: 'zb'

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

d =: 0&".&.> e =: 18!:3 ''
(<'a')         lpath e
(,<,'a')       -: lpath e
(,<,'a')       -: lpath d
('a';'bc';'d') lpath d 
(;:'a bc d')   -: lpath d
(;:'a bc d')   -: lpath e

NB. Verify numerics OK in path
f =: 0&".&.> y =: 18!:3 ''
('a';f,<'d') lpath d 
(,&.> 'a';y,<'d') -: lpath d
(,&.> 'a';y,<'d') -: lpath e

(,<,'z') -: lpath <'NonExistentLocale'
(<'NonExistent') lpath <'abc'
(,<'NonExistent') -: lpath <'abc'
'' -: lpath <'z'

(i.0 0) -: ''    lpath <'asdf'
(i.0 0) -: (i.0) lpath <'asdf'
(i.0 0) -: (0$a:)lpath <'asdf'

18!:55 ;:'a abc asdf bc cool d first new NonExistent NonExistentLocale'
18!:55 e
18!:55 >f

NB. Make sure path is freed when locale destroyed
before =. 00 + 00
after =. 00 + 00
before =. 7!:0 ''
1: 18!:3 <'a'
('a';'a';'a';'a';'a';'a';'a';'a';'a';'a';'a') 18!:2 <'a'
18!:55 <'a'
after  =. 7!:0 ''
64 >: after - before

before =. 00 + 00
after =. 00 + 00
before =. 7!:0 ''
1: l =. 18!:3 ''
(l,l,l,l,l,l,l,l,l,l,l,l,l) 18!:2 l
18!:55 l
4!:55 <'l'
after  =. 7!:0 ''
64 >: after - before

4!:55 ;:'after before'

'domain error'    -: lpath etx 'abc'
'domain error'    -: lpath etx 2 3.4
'domain error'    -: lpath etx 2 3j4
'domain error'    -: lpath etx 2 3x
'domain error'    -: lpath etx 2 3r4
'locale error'    -: lpath etx 200000;3 4
'domain error'    -: lpath etx 3 4;2

'domain error'    -: lpath etx <0 1 0
'domain error'    -: lpath etx <2 3 4
'domain error'    -: lpath etx <2 3.4
'domain error'    -: lpath etx <2 3j4
'domain error'    -: lpath etx <2 3 4x
'domain error'    -: lpath etx <2 3r4
'domain error'    -: lpath etx <<'abc'
'domain error'    -: lpath etx <<'234'
'locale error'    -: lpath etx 1e6


'ill-formed name' -: lpath etx <'!!#+'
'ill-formed name' -: lpath etx <'abc_ju'
'ill-formed name' -: lpath etx <'abc_junk_'
'ill-formed name' -: lpath etx <'abc__j'

'rank error'      -: lpath etx <3 4$'abc'

'length error'    -: lpath etx <''
'length error'    -: lpath etx <$0

'locale error'    -: 0 1 0            lpath etx <'z'
'locale error'    -: 'abc'            lpath etx <'z'
'locale error'    -: 100000 200000 300000   lpath etx <'z'
'locale error'    -: 1 2.3            lpath etx <'z'
'locale error'    -: 1 2j3            lpath etx <'z'
'locale error'    -: 1 2 3x           lpath etx <'z'
'locale error'    -: 1 2r3            lpath etx <'z'

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
'domain error'  -: lcreate etx < 1 + 0 ". > e =: lcreate ''  NB. Can't create numeric locale explicitly, even if number is high enough
'domain error'  -: lcreate etx < ": 1 + 0 ". > f =: lcreate ''  NB. Can't create numeric locale explicitly, even if number is high enough
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

18!:55 e,f


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

f =: {{
lswitch <'base'
assert. (<'base') -: 18!:5 ''

lswitch xx=:<'NonExxistent2'
assert. xx_base_ -: 18!:5 ''
assert. xx_base_ e. 18!:1 [0
lswitch_base_ <'base'
assert. (<'base') -: 18!:5 ''

lswitch <'base'
plus_a_=: +
4 plus_a_ _3
assert. (<'base') -: 18!:5 ''

4!:55 ;:'a a_z_'
d =: 0&".&.> e =: 18!:3 ''
lswitch e
a =: 5.4
lswitch__ <'base'
assert. _1 -: 4!:0 <'a'
lswitch d
assert. a -: 5.4
lswitch__ <'base'
assert. _1 -: 4!:0 <'a'
lswitch >d
assert. a -: 5.4
lswitch__ <'base'
assert. _1 -: 4!:0 <'a'

18!:55 ;:'a b c asdf NonExxistent2'
18!:55 e
0 = e e. 18!:1 (1)  NB. Deleted because not on stack
}}
f ''

'locale error'    -: lswitch etx 0
'domain error'    -: lswitch etx 'a'
'locale error'    -: lswitch etx 2
'locale error'    -: lswitch etx 15000
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
(257*1+IF64) > |x-y  NB. the numbered hashtable may grow; this will handle the first doubling

x=: spnow ''
k=: lcreate"1 i.20 0
ldestroy k
4!:55 <'k'
y=: spnow ''
(513*1+IF64) > |x-y  NB. the numbered hashtable may grow; this will handle the first doubling

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

ldestroy (0&".@>) (lcreate'') , (lcreate'')

18!:55 ;:'a b loca locb'

'domain error'    -: ldestroy etx 0 1 0
'domain error'    -: ldestroy etx 'abc'
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

NB. Numeric locale allocation.  Works only when not many locales have been allocated
NB. This is the version for locale reuse (not used)
f=: 3 : 0
if. 0 = 4!:0 <'HASRUNLOCNAME_z_' do. 1 return. end.
HASRUNLOCNAME_z_ =: 1
lastallo =. {: initallo =. allos =. , /:~ 0&".@> 18!:1 (1)   NB. locales as we see them
deldallo =. allos -.~ i. 16b1ff8   NB. locales in the order we delete them
NB. randomly allocate & delete locales, until we see an old number coming back
while. do.
  if. 0.5 < ? 0 do.
    NB. Allocate a new locale, append to list, exit loop if reuse has started
    allos =. allos , newallo =. 0&". > 18!:3 ''
    if. newallo < lastallo do. break. end.
    lastallo =. newallo
  else.
    NB. Delete a randomly-chosen locale if there is one
    if. #allos -. initallo do.
      delallo =. ({~  ?@#) allos -. initallo  NB. initallo may be on stack, so don't try to delete
      18!:55 delallo
      allos =. allos -. delallo
      deldallo =. deldallo , delallo
    end.
  end.
end.
NB. Verify our locale list matches the system
assert. allos -:&(/:~) 0&".@> fnm =. 18!:1 (1) [ 1
assert. lastallo > 5000   NB. at least 5000 allos before we recycle anything
NB. Now deldallo has the locales we have deleted, in the order we deleted them.  newallo is the locale we have just allocated
NB. It should not be among the last 5000 locales we deleted
assert. newallo -.@e. _5000 {. deldallo
NB. Go through 10000 more random alloc/delete, verifying that each allocated locale is not within the last 5000
for. i. 10000 do.
  if. 0.5 < ? 0 do.
    NB. Allocate a new locale, append to list
    allos =. allos , newallo =. 0&". > 18!:3 ''
    assert. newallo -.@e. _5000 {. deldallo
  else.
    NB. Delete a randomly-chosen locale if there is one
    if. #allos -. initallo do.
      delallo =. ({~  ?@#) allos -. initallo  NB. initallo may be on stack, so don't try to delete
      18!:55 delallo
      allos =. allos -. delallo
      deldallo =. deldallo , delallo
    end.
  end.
end.
NB. Even after all that, the largest locale number extant should be less than 16b1ff8
assert. allos -:&(/:~) 0&".@> fnm =. 18!:1 (1) [ 1
assert. 16b1ff8 > >./ allos
18!:55 allos -. initallo  NB. clean up
1
)

NB. This is the version for hashed allocation
f=: 3 : 0
cocurrent =. 18!:4
lastallo =. {: initallo =. allos =. , /:~ 0&".@> 18!:1 (1)   NB. locales as we see them
deldallo =. $0
NB. Allocate a starter set of locales
for. i. 100 do. allos =. allos , lastallo =. 0&".@> 18!:3'' end.
NB. Do random allocate/delete
for. i. 10000 do.
  if. 0.5 < ? 0 do.
    NB. Allocate a new locale, append to list, exit loop if reuse has started
    allos =. allos , newallo =. 0&". > 18!:3 ''
    assert. newallo = lastallo + 1
    lastallo =. newallo
  else.
    NB. Delete a randomly-chosen locale if there is one
    if. #allos -. initallo do.
      delallo =. ({~  ?@#) allos -. initallo  NB. initallo may be on stack, so don't try to delete
      assert. (<":delallo) e. 18!:1 (1)
      18!:55 delallo
      assert. (<":delallo) -.@e. 18!:1 (1)
      allos =. allos -. delallo
      deldallo =. deldallo , delallo
      NB. Verify we haven't cut off any of the locales in the hash chain
      for_loc. allos do. - each__loc 1 end.
    end.
  end.
end.
NB. Verify our locale list matches the system
assert. allos -:&(/:~) 0&".@> fnm =. 18!:1 (1) [ 1
18!:55 allos -. initallo  NB. clean up
1
)

f''

NB. Verify local name doesn't affect lookup
f =: {{
xy_z_  =: 1:
nonlocale =. 5
assert. xy_nonlocale_ ''

18!:55 ;:'nonlocale'
coclass 'nonlocale'
dhs2liso=: ]
f =: _1 (1 : '] dhs2liso_nonlocale_ ]')  NB. ref in non-verb messed up lookup
coclass 'base'

NB. ensure destroyed locale reset properly
coclass 'abcpristloc'  NB. absolutely new locale
('abcpristloc';'z') copath 'nonlocale'  NB. protect it from deletion
(<"1 (+&(i.26)&.(a.&i.) 'a') ,. 26$'0123456789') =: 0  NB. Fill up Bloom filter
a_abcpristloc_ =: 5
b_abcpristloc_ =: 6
c_abcpristloc_ =: 11
assert. c = a + b
assert. 5 = a_nonlocale_
coclass 'base'
18!:55 ;:'abcpristloc'  NB. now zombie  usecount 1
coclass 'abcpristloc'  NB. revive: check reinited  usecount 2
assert. _1 = 4!:0 ;:'a0 a b c a_nonlocale_'
coclass 'base'  NB. take abcpristloc out of execution
18!:55 ;:'abcpristloc'  NB. now zombie  NB. usecount 1
s1 =: 7!:0''
('base';'z') copath 'nonlocale'  NB. frees abcpristloc
assert. s1 > 7!:0''
}}
f ''




4!:55 ;:'a a_z_ ab c d dd dhs2liso dhs2liso_nonlocale_ e ee f '
4!:55 ;:'indirect k lcreate ldestroy lname lnc lnl lpath lswitch '
4!:55 ;:'not_a_locative s1 s2 s3 spnow t test x xx xy_z_ xy_nonlocale_ y '
18!:55 ;:'abcpristloc nonlocale'



epilog''

