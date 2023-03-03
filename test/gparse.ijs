prolog './gparse.ijs'
NB. parsing edge cases ----------------------------------

'syntax error' -: ex 'a =: 5 b =: )' [  a =: 0
a = 5

'domain error' -: ex '''a'' 2 : ''u'' 3 =. 6'
a = 5
6 -: ex '(''a'' 2 : ''u'' 3) =. 6'
a = 6

NB. erase local a
4!:55 ;:'a '

'syntax error' -: ex ') 5'
'syntax error' -: ex ') +'
'syntax error' -: ex ') &'
'syntax error' -: ex ') /'
'syntax error' -: ex ') ('
'syntax error' -: ex 'a (=.) 5'
'syntax error' -: ex '(++'
'syntax error' -: ex '(//'
'syntax error' -: ex '(@'
'syntax error' -: ex ']a =. (1 + 2'

a =: +: 2 &
'4&' -: 5!:5 <'a'

NB. test nvr management: many names on a line, and multiple identical nvrs
a =: 1
4097 = ". 'a' ,~ ; 4096 # <'a + '

NB. Make sure deferred frees are performed, by creating an 800MB array and repeatedly assigning to it
NB. a =: 1e8$2
a =: 1e7$2
multi =: 3 : 'a =: a =: a [ a + a'
NB. a -: multi^:100 a
a -: multi^:10 a

a =: 3 : 0
locnm =. >: y
locnm [ ". 'locnm =. 1;2;3'
)
NB. We don't protect local names (for speed).  That makes the assignment-in-". destroy the value
NB. 10 11 -: 3 : 0 (3 4)
NB. (a 5) + a y
NB. )
a =: 3 : 0
locnm =: >: y
locnm [ ". 'locnm =: 1;2;3'
)
10 11 -: 3 : 0 (3 4)
(a 5) + a y
)
a =: 3 : 0
locnm =: >: y
locnm [ 4!:55 <'locnm'
)
10 11 -: 3 : 0 (3 4)
(a 5) + a y
)
a =: 3 : 0
locnm =. >: y
locnm [ 4!:55 <'locnm'
)
10 11 -: 3 : 0 (3 4)
(a 5) + a y
)


t =: 3 : 'y'
'valence error' -: 2 t etx 5
t =: {{ x [ y }}
'valence error' -: t etx 5

t=: {{
inner =. {{ x + y}}
10 inner y
}}

15 = t 5
'valence error' -: 6 t etx 5

NB. Test display of error spacing
'|domain error: efx|   1    +''a''' -:&(}.~ i:&'|') efx '1 + ''a'''
'|domain error: efx|   2    /' -:&(}.~ i:&'|') efx '2/'
'|domain error: efx|       i.0.5' -:&(}.~ i:&'|') efx 'i. 0.5'
'|domain error: efx|   2    @+i.5' -:&(}.~ i:&'|') efx '2@+ i. 5'
'|domain error: efx|       (+:+:+:)i.5' -:&(}.~ i:&'|') efx '(+: +: +:) i. 5'
'|domain error: efx|       (+:+:)i.5' -:&(}.~ i:&'|') efx '(+: +:) i. 5'
'|value error: undefname|   i__undefname    =:5' -:&(}.~ i:&'|') efx 'i__undefname =: 5'
'|domain error: efx|   n__efx    =:5' -:&(}.~ i:&'|') efx 'n__efx =: 5'
'|domain error: efx|public assignment to a name with a private value|   y    =:1' -:&(}.~ i:&'|')  efx '{{ y =: 1}} 2'
'|syntax error: efx|       )123' -:&(}.~ i:&'|') efx ')123'
'|syntax error: efx|   )123    (' -:&(}.~ i:&'|') efx ')123('
'|value error: undef|   (undef undef)    (1+undef+".)''5 + 6''' -:&(}.~ i:&'|') efx '(undef undef) (1 + undef + ".) ''5 + 6'''
'|value error: undef|   (undef undef)    (1+undef+".)''+ 6''' -:&(}.~ i:&'|') efx '(undef undef) (1 + undef + ".) ''+ 6'''

NB. Verify that undefname is OK, but undefined u creates an error, in an explicit def
t =: 1 : 0
undefname/ if. 0 do. return. end.
v/
)

0 = {{ test [ [ [ (]'test')=. y }} 0  NB. stacking name that is defined earlier in the line by indirect assignment

'|value error: t|       v/' -: efx '+ t 5'

'|ill-formed number: efx|   1+1qs|     ^|   3     :(''1+1qs'',(10{a.))' -:&(}.~ i:&'|') efx '3 : (''1+1qs'',(10{a.))'


NB. Verify AR assignment works for 1 & more names
'`a' =: +`''
3 -: 4!:0<'a'
(,'+') -: 5!:5 <'a'
'`a' =: {. (+-)`''
3 -: 4!:0<'a'
('+ -') -: 5!:5 <'a'
'`a bc' =: {. (- + %)`''
3 -: 4!:0<'a'
('- + %') -: 5!:5 <'a'
3 -: 4!:0<'bc'
('- + %') -: 5!:5 <'bc'
'`a bc' =: (*-)`(+)
3 -: 4!:0<'a'
('* -') -: 5!:5 <'a'
3 -: 4!:0<'bc'
(,'+') -: 5!:5 <'bc'

NB. Test single-word lines
swd =: 1 : 0
y
5
)
5 -: + swd ''


5 -: + (1 : 0) 
+
if. do. 5 end.
)

5 -: + (1 : 0) 
+
5
)

swd =: 1 : 0
+ if. 0 do. return. end.
u if. 0 do. return. end.
& if. 0 do. return. end.
/ if. 0 do. return. end.
undefname if. 0 do. return. end.
5
)
5 -: + swd



NB. Verify noun assignments to all parts of speech are honored
3 = +('aa'=:/) i. 3
5 = +/('aa'=:@:)*: i. 3
_6 = ('aa'=: -) 6
1 2 3 -: 'aa' =: 1 2 3

NB. check uv must be assigned if used in explicit defs, but xymn ok
3 : 'x ] y&(4!:55 <''y'') ] m ] n if. do. end. 1' 4
'value error' -: 3 : '* u' etx 4
'value error' -: 3 : '* v' etx 4
'noun result was required' -: 3 : 'zz&(4!:55 <''y'')' etx 4
NB. t =. * x  NB. OK from console - can't really check without breaking tests when run inside explicit def

a =: 1
1 = a ]:  NB. failed when arg returned modifier
a ]:

'syntax error' -: ". etx 'Oc)'
'syntax error' -: ". etx 'Oc('

NB. display of decorated constants
a =: 'a'
'|domain error: efx|   a    +1.' -:&(}.~ i:&'|') efx 'a +  1.'
'|domain error: efx|   a    +1' -:&(}.~ i:&'|') efx 'a +  1'
'|domain error: efx|   a    +0' -:&(}.~ i:&'|') efx 'a +  0'
'|domain error: efx|   a    +01' -:&(}.~ i:&'|') efx 'a +  01'
'|domain error: efx|   a    +1 1 01' -:&(}.~ i:&'|') efx 'a +  01 1 1'
'|domain error: efx|   a    +1j0' -:&(}.~ i:&'|') efx 'a +  1j0'
'|domain error: efx|   a    +1x' -:&(}.~ i:&'|') efx 'a +  1x'
'|domain error: efx|   a    +1r1' -:&(}.~ i:&'|') efx 'a +  1r1'
'|domain error: efx|   a    +1000000' -:&(}.~ i:&'|') efx 'a +  1e6'
'|domain error: efx|   a    +1e6' -:&(}.~ i:&'|') efx 'a +  1.e6'
'|domain error: efx|   a    +_2' -:&(}.~ i:&'|') efx 'a +  _2'
'|domain error: efx|   a    +_2.' -:&(}.~ i:&'|') efx 'a +  _2.'
'|domain error: efx|   a    +_ _1' -:&(}.~ i:&'|') efx 'a +  _ _1.'
'|domain error: efx|   a    +__ _1' -:&(}.~ i:&'|') efx 'a +  __ _1.'
'|domain error: efx|   a    +_. _1' -:&(}.~ i:&'|') efx 'a +  _. _1.'

4!:55 ;:'a aa bc locnm multi swd t '



epilog''

