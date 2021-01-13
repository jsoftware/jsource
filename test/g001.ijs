1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g001.ijs'
NB. =. and =: -----------------------------------------------------------

erase  =: 4!:55
local  =: 4 : '(x)=.y'
global =: 4 : '(x)=:y'

a  =: 1 2 3
aa =: 'foo upon thee'
a  -: 1 2 3
aa -: 'foo upon thee'
erase <'a'
aa -: 'foo upon thee'
aa =: i.4 3
aa -: i.4 3

a =: 3.14
alta =: b =: 'first and last letter'
erase <'a'
alta -: b

Ich_liebe_dich =: 'Je t''aime.'
(#Ich_liebe_dich) -: 0{$Ich_liebe_dich

('first';'second';'third') =: 'Cogito'; 'ergo'; i.12
first  -: 'Cogito'
second -: 'ergo'
third  -: i.12

('a_man';'j_k';'j_k_l';'j_k_l_m') =: 123 456 789 012
a_man -: 123
j_k  -: 456
j_k_l  -: 789
j_k_l_m  -: 012

names =: ' abc def ghi'
0 0$ (names)   =: i. 3 3
(".;.1 names) -: i. 3 3
4!:55 ;: names

'p q r' =: <"_1 i.3 4
p -: 0 + i.4
q -: 4 + i.4
r -: 8 + i.4

'abc' -: 'p q r' local 'abc'
p -: 0 + i.4
q -: 4 + i.4
r -: 8 + i.4

'abc' -: 'p q r' global 'abc'
p -: 'a'
q -: 'b'
r -: 'c'

'p' =: i.12
p -: i.12
(<'abc') =: x=: ?4 5$100
abc -: x

0 0$ ('p';'q';'r') =: o.4 5 6
p -: o.4
q -: o.5
r -: o.6

'p q r' =: <'Ich liebe dich'
p -: 'Ich liebe dich'
p -: q
p -: r

'Cogito ergo sum'=:i.3 4
Cogito -:   i.4
ergo   -: 4+i.4
sum    -: 8+i.4

'abc'=: i.7
abc -: i.7

NB.  The following lines generate no display

a=:12
'a'=:12

'domain error'      -: ". etx '2 3         =. 9'
'domain error'      -: ". etx '3.5         =. 9'
'domain error'      -: ". etx '3j4         =. 9'

'ill-formed number' -: ". etx '_abc =. 9'

'ill-formed name'   -: ". etx '''p+9'' =. 9'
'ill-formed name'   -: ". etx '''3ab'' =. 9'
'ill-formed name'   -: ". etx '''p___'' =. 9'
'ill-formed name'   -: ". etx '''p__a_b_c'' =. 9'
'ill-formed name'   -: ". etx '''p__a_b_c_d'' =. 9'
'ill-formed name'   -: ". etx '''p__a_b_c_d_e'' =. 9'
'ill-formed name'   -: ". etx '''p__9ab'' =. 9'
'ill-formed name'   -: ". etx '''p__9'' =. 9'
'ill-formed name'   -: ". etx '''p__ab_'' =. 9'
'ill-formed name'   -: ". etx '''p__ab__9'' =. 9'


'domain error'      -: ". etx '(''p'';2 3  ) =. 9'
'domain error'      -: ". etx '(''p'';3.5  ) =. 9'
'domain error'      -: ". etx '(''p'';3j4  ) =. 9'

'ill-formed name'   -: ". etx '(''p'';''p+9'') =. 9'
'ill-formed name'   -: ". etx '(''p'';''3ab'') =. 9'  
'ill-formed name'   -: ". etx '(''p'';''_ab'') =. 9' 
'ill-formed name'   -: ". etx '''   '' =. 9'
'ill-formed name'      -: ". etx '(''p'';'''') =. 9'
'domain error'   -: ". etx ''' `a'' =. +'
'domain error'   -: ". etx '''a b'' =. +'

'rank error'        -: ". etx '(<3 4$''a'') =. i.3 4'
'rank error'        -: ". etx '(''p'';3 4$''a'') =. 9'

'length error'      -: ". etx '''pqr s'' =. 4 5 6'
'length error'      -: ". etx '(''p'';''q'';''r'') =. 4 5'
'length error'      -: ". etx '(,<''abc'') =. i.5'

NB. Check again inside definition
3 : 0"0 i. 9
try.
 select. y
 case. 0 do. '' =. 5
 case. 1 do. '' =: 5
 case. 2 do. ' ' =. 5
 case. 3 do. ' ' =: 5
 case. 4 do. ' `a' =: +`''
 case. 0 do. 'a b' =. i. 3
 case. 5 do. 'a b' =. <"0 i. 3
 case. 6 do. 'a b' =. i. 1 3
 case. 7 do. 'a b___' =. i. 2
 case. 8 do. 'a+b' =. i. 3
 end.
 0  NB. should have failed
catch.
 1
end.
)

NB. successful assignment inside definition
3 : 0 ''
z =. $0
'local' =. 5
z =. z , local -: 5
'local1 local2' =. 9 3
z =. z , local1 -: 9
z =. z , local2 -: 3
'`a' =. +`''
z =. z , 9 -: 7 a 2
'`a b' =. -`*
z =. z , _7 -: 3 a 2 b 5
'local' =. <5
z =. z , local -: <5
'local' =. 6 7
z =. z , local -: 6 7
'local' =. <8 7 6
z =. z , local -: <8 7 6
'local1 local2 local3' =. 4;7;2
z =. z , local1 -: 4
z =. z , local2 -: 7
z =. z , local3 -: 2
'local1 local2 local3' =. <6
z =. z , local1 -: 6
z =. z , local2 -: 6
z =. z , local3 -: 6
'local1 local2 local3' =. 'f'
z =. z , local1 -: 'f'
z =. z , local2 -: 'f'
z =. z , local3 -: 'f'
'local1 local2 local3' =. 'ghi'
z =. z , local1 -: 'g'
z =. z , local2 -: 'h'
z =. z , local3 -: 'i'
'global' =: 5
z =. z , global__ -: 5
'global1 global2' =: 9 3
z =. z , global1__ -: 9
z =. z , global2__ -: 3
'global' =: <5
z =. z , global__ -: <5
'global' =: 6 7
z =. z , global__ -: 6 7
'global' =: <8 7 6
z =. z , global__ -: <8 7 6
'global1 global2 global3' =: 4;7;2
z =. z , global1__ -: 4
z =. z , global2__ -: 7
z =. z , global3__ -: 2
'global1 global2 global3' =: <6
z =. z , global1__ -: 6
z =. z , global2__ -: 6
z =. z , global3__ -: 6
'global1 global2 global3' =: 'f'
z =. z , global1__ -: 'f'
z =. z , global2__ -: 'f'
z =. z , global3__ -: 'f'
'global1 global2 global3' =: 'ghi'
z =. z , global1__ -: 'g'
z =. z , global2__ -: 'h'
z =. z , global3__ -: 'i'
z
)

NB. Assigning to local name OK if to locative
3 : 0 ''
s =: cocreate''
n =. 4
n__s =: 5
n__s =: 5 + 6
n__s =: 0$<''
coerase s
1
)



4!:55 ;:'Cogito a abc ergo p q r sum x'
4!:55 ;:'Cogito a abc ergo p q r sum x'


NB. naming side effects -------------------------------------------------

fa =: 3 : 0
 fa=:+/
 2*fa y
)

(2*+/x) -: fa x=:?30$1000
(  +/x) -: fa x

fb =: 3 : ('abc=:25'; 'y+abc')
abc =: 12
37 -: fb abc
50 -: fb abc

abc =: 12
37 -: (3 : ('abc=:25'; 'y+abc')) abc
50 -: (3 : ('abc=:25'; 'y+abc')) abc

37      -: (3&* + 4!:55@((   <'abc')"_)) abc=:12
_1 -: 4!:0 <'abc'
(30$37) -: (3&* + 4!:55@((30$<'abc')"_)) abc=:12
_1 -: 4!:0 <'abc'
37      -: (4!:55@((   <'abc')"_) + 3&*) abc=:12
_1 -: 4!:0 <'abc'
(30$37) -: (4!:55@((30$<'abc')"_) + 3&*) abc=:12
_1 -: 4!:0 <'abc'

lf      =: 10{a.
write   =: 1!:2
sscript =: 0!:0

t=:'x=:'&,@":@i.&.>10-i.10
x=:<'foo.x'
(;t,&.>lf) write x
sscript (20)$x
0 -: x
0!:0 x=:20$<'foo.x'
0 -: x
1!:55 <'foo.x'

x=:>|.t
(".x) -: i."0>:i.10
x -: i.10


NB. reassignment on the same line ---------------------------------------

t=:123456
x=:17
t=:7!:0 ''
x=:i.1e5
(17,i.1e5) -: (x=:17),x
1200 > y=:t -~ 7!:0 ''

x=:i.1e3
(17,i.1e3) -: (17 [ 4!:55 <'x'),x

4!:55 ;:'b t x y'
4!:55 ;:'b t x y'

fa =: 3 : 0
 if. y do.
  x=:i.1e3
  b,(17,i.1e3) -: (x=:17 [ b=.fa y-1),x
 else.
  i.0
 end.
)

fa 1
fa 2
fa 3
fa 4
fa 5  

4!:55 ;:'b fa x'

fb =: 3 : 0
 if. y do.
  x=:i.1e3
  b, (17,i.1e3) -: (17 [ b=.fb y-1 [ 4!:55 <'x'),x
 else.
  i.0
 end.
)

fb 1
fb 2
fb 3
fb 4
fb 5  


NB. lookups of nonassigned names skip local lookup if there have been no assignments
'noun result was required' -: 3 : '(y) =. 1 [ one =. 1 if. do. end. two' etx 'one'
1 -: 3 : '(y) =. 1 [ one =. 1 if. do. end. two' 'two'
2 -: 3 : '(y) =. 2 [ one =. 1 if. do. end. one' 'one'
1 -: 3 : '(y) =. 2 [ one =. 1 if. do. end. one' 'two'
2 -: 'one' 4 : '(x) =. 2 [ (y) =. 1 [ one =. 1 if. do. end. one' 'one'
2 -: 'two' 4 : '(x) =. 2 [ (y) =. 1 [ one =. 1 if. do. end. two' 'two'
1 -: 'one' 4 : '(x) =. 2 [ (y) =. 1 [ one =. 1 if. do. end. two' 'two'

gval_z_ =: 0
fa =. 3 : '(y) =. 1 [ one =. 1 for. i. 10000 do. gval ] gval ] gval  ] gval  ] gval  ] gval  ] gval  ] gval  ] gval end. gval'
NB. too close to rely on now (100 (6!:2) 'fa ''one''') < (100 (6!:2) 'fa ''two''')  NB. Not much difference, but it should be faster

NB. leaks that have been fixed
NB. a =. 4 - 3  NB. allocate a as an integer
a =. 300 + 7!:0''  NB. 300 is slop to allow in memory alloc
1: ]^:0: <"0 i. 1e4
a > 7!:0''

1: +: M. i. 1000
a > 7!:0''

1: (xxx =: +: M.) i. 1000
4!:55 <'xxx'
a > 7!:0''

0 = ('=',':') +./@:E. a =. 2 , :: (13!:12@'') '2'  NB. For a while the =. was changed to =:

NB. Test that arithmetic dyads migrate outer loop to inner loop
p =: 0.5 * i. 10000 [ q =: 0.5
(p +"0 0"1 _ q) -: (p +"1 _ q)
THRESHOLD +. ((10000) 6!:2 'p + q') > 0.5 * (10000) 6!:2 'p +"1 _ q'
(p +"0 0"0 _ q) -: (p +"0 _ q)
THRESHOLD +. ((10000) 6!:2 'p + q') > 0.5 * (10000) 6!:2 'p +"0 _ q'
(p +"0 0"0 0 q) -: (p +"0 0 q)
THRESHOLD +. ((10000) 6!:2 'p + q') > 0.5 * (10000) 6!:2 'p +"0 0 q'
p =: i. 10000 2 [ q =: i. 10000 2
(p +"0 0"1 q) -: (p +"1 q)
THRESHOLD +. ((10000) 6!:2 'p + q') > 0.5 * (10000) 6!:2 'p +"1 q'
p =: i. 10000 2 [ q =: i. 10000
(p +"0 0"1 0 q) -: (p +"1 0 q)
THRESHOLD +. ((10000) 6!:2 'p + q') > 0.6 * (10000) 6!:2 'p +"1 0 q'


4!:55 ;:'Cogito Ich_liebe_dich a a_man aa abc alta b def erase ergo'
4!:55 ;:'fa fb first global global1 global2 global3'
4!:55 ;:'ghi global j_k j_k_l j_k_l_m lf local names p pqr q r s second sscript gval_z_'
4!:55 ;:'sum t third write x y'


