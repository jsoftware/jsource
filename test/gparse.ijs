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



NB. Test display of error spacing
'|domain error: efx|   1    +''a''' -: efx '1 + ''a'''
'|domain error: efx|   2    /' -: efx '2/'
'|domain error: efx|       i.0.5' -: efx 'i. 0.5'
'|domain error: efx|   2    @+i.5' -: efx '2@+ i. 5'
'|domain error: efx|       (+:+:+:)i.5' -: efx '(+: +: +:) i. 5'
'|domain error: efx|       (+:+:)i.5' -: efx '(+: +:) i. 5'
'|value error: undefname|   i__undefname    =:5' -: efx 'i__undefname =: 5'
'|domain error: efx|   n__efx    =:5' -: efx 'n__efx =: 5'
'|syntax error: efx|   )123' -: efx ')123'
'|value error: undef|   (undef undef)    (1+undef+".)''5 + 6''' -: efx '(undef undef) (1 + undef + ".) ''5 + 6'''
'|value error: undef|   (undef undef)    (1+undef+".)''+ 6''' -: efx '(undef undef) (1 + undef + ".) ''+ 6'''
'|domain error: efx|   5.7    ([:;]<@(+/\);.2)i.5' -: efx '5.7 ([:;]<@(+/\);.2) i. 5'

NB. Verify that undefname is OK, but undefined u creates an error, in an explicit def
t =: 1 : 0
undefname/ if. 0 do. return. end.
v/
)

'|value error: t|       v/' -: efx '+ t 5'

'|ill-formed number|   1+1qs|     ^|   3     :(''1+1qs'',(10{a.))' -: efx '3 : (''1+1qs'',(10{a.))'


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

NB. check the special names must be assigned if used in explicit defs
'value error' -: 3 : '* x' etx 4
'value error' -: 3 : '* u' etx 4
'value error' -: 3 : '* v' etx 4
'value error' -: 3 : '* m' etx 4
'value error' -: 3 : '* n' etx 4
'value error' -: 3 : 'y&(4!:55 <''y'')' etx 4
'noun result was required' -: 3 : 'zz&(4!:55 <''y'')' etx 4
NB. t =. * x  NB. OK from console - can't really check without breaking tests when run inside explicit def

4!:55 ;:'a aa bc locnm multi swd t '



epilog''

