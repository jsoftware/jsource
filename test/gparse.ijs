NB. parsing edge cases ----------------------------------

'syntax error' -: ex 'a =: 5 b =: )' [  a =: 0
a = 5

'domain error' -: ex '''a'' 2 : ''u'' 3 =. 6'
a = 5
6 -: ex '(''a'' 2 : ''u'' 3) =. 6'
a = 6

'syntax error' -: ex ') 5'
'syntax error' -: ex ') +'
'syntax error' -: ex ') &'
'syntax error' -: ex ') /'
'syntax error' -: ex ') ('

NB. test nvr management: many names on a line, and multiple identical nvrs
a =: 1
4097 = ". 'a' ,~ ; 4096 # <'a + '

NB. Make sure deferred frees are performed, by creating an 800MB array and repeatedly assigning to it
a =: 1e8$2
multi =: 3 : 'a =: a =: a [ a + a'
a -: multi^:100 a

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


4!:55 ;:'a t '


