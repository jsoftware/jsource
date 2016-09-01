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

NB. test nvr management: many names on a line, and multiple identical nvrs
a =: 1
4097 = ". 'a' ,~ ; 4096 # <'a + '

NB. Make sure deferred frees are performed, by creating an 800MB array and repeatedly assigning to it
NB. a =: 1e8$2
a =: 1e7$2
multi =: 3 : 'a =: a =: a [ a + a'
NB. a -: multi^:100 a
a -: multi^:10 a

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

NB. Verify that undefname is OK, but undefined x. creates an error, in an explicit def
t =: 3 : 0
undefname/
x/
y
)

'|value error: t|       x/' -: efx 't 5'

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
swd =: 3 : 0
+
y
&
/
undefname
5
)
5 -: swd''

NB. Verify noun assignments to all parts of speech are honored
3 = +('aa'=:/) i. 3
5 = +/('aa'=:@:)*: i. 3
_6 = ('aa'=: -) 6
1 2 3 -: 'aa' =: 1 2 3

4!:55 ;:'a aa bc multi swd t '


