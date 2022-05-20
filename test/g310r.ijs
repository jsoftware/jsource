prolog './g310r.ijs'
NB. : representations of explicit operators -----------------------------

nln=: 10{a.
nn=: <@((,'0')&;)

F=: 2 : 0
u v y
:
x u v y
)


F1=: 2 : 'u v y'

F2=: 2 : 0
:
x u v y
)

f =: + F  *
f1=: + F1 *
f2=: + F2 *

(5!:1 <'F' ) -: <(,':');<(nn 2),nn ];._1 '/u v y/:/x u v y'
(5!:1 <'F1') -: <(,':');<(nn 2),nn ];._1 '/u v y'
(5!:1 <'F2') -: <(,':');<(nn 2),nn ];._1 '/:/x u v y'
(5!:1 <'f' ) -: <(5!:1 <'F' ),<,&.>'+*'
(5!:1 <'f1') -: <(5!:1 <'F1'),<,&.>'+*'
(5!:1 <'f2') -: <(5!:1 <'F2'),<,&.>'+*'

(5!:2 <'F' ) -: 2;(,':');];._1 '/u v y/:/x u v y'
(5!:2 <'F1') -: 2;(,':');];._1 '/u v y'
(5!:2 <'F2') -: 2;(,':');];._1 '/:/x u v y'
(5!:2 <'f' ) -: ,&.> '+';(5!:2 <'F' );'*'
(5!:2 <'f1') -: ,&.> '+';(5!:2 <'F1');'*'
(5!:2 <'f2') -: ,&.> '+';(5!:2 <'F2');'*'

(5!:5 <'F' ) -: '2 : 0',nln,'u v y',nln,':',nln,'x u v y',nln,')'
(5!:5 <'F1') -: '2 : ''u v y'''
(5!:5 <'F2') -: '2 : ('':''; ''x u v y'')'
(5!:5 <'f' ) -: '+ (2 : 0) *',(i.&nln }. ]) 5!:5 <'F'
(5!:5 <'f1') -: '+ (',(5!:5 <'F1'),') *'
(5!:5 <'f2') -: '+ (',(5!:5 <'F2'),') *'

f =: ".@'abc'
(5!:5<'f') -: '".@''abc'''

G=: 1 : 0
u y
:
x u y
)

G1=: 1 : 'u y'

G2=: 1 : 0
:
x u y
)
 
g =: +/ G
g1=: +/ G1
g2=: +/ G2

sum=: +/

(5!:1 <'G' ) -: <(,':');<(nn 1),nn ];._1 '/u y/:/x u y'
(5!:1 <'G1') -: <(,':');<(nn 1),nn ];._1 '/u y'
(5!:1 <'G2') -: <(,':');<(nn 1),nn ];._1 '/:/x u y'
(5!:1 <'g' ) -: <(5!:1 <'G'),<,5!:1 <'sum'
(5!:1 <'g1') -: <(5!:1 <'G1'),<,5!:1 <'sum'
(5!:1 <'g2') -: <(5!:1 <'G2'),<,5!:1 <'sum'

(5!:2 <'G' ) -: 1;(,':');];._1 '/u y/:/x u y'
(5!:2 <'G1') -: 1;(,':');];._1 '/u y'
(5!:2 <'G2') -: 1;(,':');];._1 '/:/x u y'
(5!:2 <'g' ) -: (,&.>'+/');<5!:2 <'G'
(5!:2 <'g1') -: (,&.>'+/');<5!:2 <'G1'
(5!:2 <'g2') -: (,&.>'+/');<5!:2 <'G2'

(5!:5 <'G' ) -: '1 : 0',nln,'u y',nln,':',nln,'x u y',nln,')'
(5!:5 <'G1') -: '1 : ''u y'''
(5!:5 <'G2') -: '1 : ('':''; ''x u y'')'
(5!:5 <'g' ) -: '+/ (1 : 0)',(i.&nln }. ]) 5!:5 <'G'
(5!:5 <'g1') -: '+/ (',(5!:5 <'G1'),')'
(5!:5 <'g2') -: '+/ (',(5!:5 <'G2'),')'

NB. Going through locatives leaves global path unchanged
a_z_ =: a =: 1 : 0
xxx =. u. f.
1
)
+ a
f =: ((coname '') -: coname)
f a
g =: a_z_
+ g
f g
3 : 0 ''
assert. + a
ff =. ((coname '') -: coname)
assert. f a
assert. ff a
assert. ((coname '') -: coname) g
assert. f g
assert. ff g
1
)
4!:55 ;:'a a_z_'  NB. names used below
 
NB. Local name passed to another modifier
'`v1 v2 v3' =: -`*:`-:   NB. these are the values found by dou
dou =: 1 : '". (5!:5<''u'') , ''  :: ]'' , ": y return. u'  NB. This processes by name
v =: 3 : 0
r =. ''
v1 =. v4 =. v5 =. v6 =. v7 =. v8 =. +
r =. r , v1 dou 5
v2 =. %
r =. r , v2 dou 5
v3 =. +:
r =. r , v3 dou 6
r
)
_5 25 3 -: v ''
dou =: 1 : 'u.  :: ] y'  NB. This used the implicit locative
5 0.2 12 -: v ''

4!:55 ;:'v1 v2 v3'  NB. names used above

NB. processing on u./v. in native locales
a_z_ =: 1 : 0
s =. 6
cocurrent 'yyy'
r =. u. y
assert. (<'yyy') -: coname''
assert. s = 6
r
)
f =: 3 : 0
s =. 7
t =. ".
j=: 8
j_xxx_=. 5
cocurrent 'xxx'
(t a y) , (t a 'j')
)
7 5 = f 's'

d_yyy_=: 9:
d_xxx_=:7:
c_z_ =: 2 : 0
s =. 15
cocurrent 'base'
r =. ((d+u.) a x),(v. a y)  NB. d and u run in different locales (d in yyy, u in calling locale base/xxx)
NB. execution of a changes the locale to yyy because the operator is anonymous
assert. s = 15
r
)

f =: 4 : 0
s =. 17
t =. ".
r =. *:
j=: 8
j_xxx_=. 5
cocurrent 'xxx'
r =. (x r c t y) , (x r c t 'j')  NB. locale changes between executions because the operator is anonymous
r
)
153 17 153 5 -: 12 f 's'

a =: 1 : 'u.'
20 = +: a 10

aa =: 1 : 'u. a'
20 = +: aa 10

NB. References passed through
a =: 1 : 0
1+u.
)

aa =: 1 : 0
(*:@u) a
)

26 = (<: aa) 6

NB. References assigned
aa =: 1 : 0
s =. 9:
p =. 10
t =. *:@u.
q =: *:@u. f.  NB. verifies fix picks up from caller
(u. 'p'),(t 'p'),(q 'p'),((*:@u.) a 'p'),(t a 'p'),(q a 'p')
)

f =: 3 : 0
s =. ".
p =. 15
s aa
)
15 225 100 226 226 101 -: f''  NB. The returns from a all get fixed, but only at lowest level

NB. Returns
5 -: 10 + (2 : 'u. v.') - 5
_5 -: 10 + (2 : '(u.~ v.)~') - 5
NB. v. not fixed (bug)  24 = +: 2 : 'u.^:(1:`([v.))' - 12
+: 2 :' v."_`u.@.(*@#@])' *:
'noun result was required' -: 3 : 0 etx ''
nm =. + + +
 + 2 : 'v."_`' nm
)
nm =. +:
2 -: + 2 : 0 - (2)
nm =. +
%: 2 : 'u. v.' nm   NB. return contains nm, executed in caller
)
6 = 1 (+ 2 : 'x undefname`u.`v.@.[ y' -) 5
_3 = 2 (+ 2 : 'x undefname`u.`v.@.[ y' -) 5
'value error' -: 0 (+ 2 : 'x undefname`u.`v.@.[ y' -) etx 5

1: 0 : 0
a =: 1 : 0
u. aa y
<@i."0 i. 30
u =. %:
g y
()
aa =: 1 : 0
u. aaa y
u. y
()
aaa =: 1 : 0
g =: u.
u. y
()
)

NB. Inverse lookups stop fixing at inverse
f =: 3 : 0  NB. Run in a definition to avoid setting bstkreqd for all subsequent tests
4!:55 ;:'a c'
cocurrent 'xxx'
nameinxxx =: ]
dec=: {{ nameinxxx y
:
nameinxxx y
}}
enc=: {{
>:@nameinxxx y
:
>:@nameinxxx y
}}
cocurrent 'z'
a =: enc_xxx_ :. dec_xxx_
c=: a_z_^:_1
cocurrent 'base'
assert. 1 -: ]&.a 0
assert. 1 -: ]&.c 0
assert. 101 -: 100 [&.a 0
assert. 101 -: 100 [&.c 0
1
)
f ''

4!:55 ;:'a aa q a_z_ c_z_ d_yyy_ d_xxx_ j j_xxx_ dou F f F1 f1 F2 f2 G g G1 g1 G2 g2 nameinxxx_xxx_ nln nm nn sum v'
18!:55 ;:'xxx yyy'



epilog''

