NB. if/else -------------------------------------------------------------

fa =. 3 : 'if. y do. 1 else. 0 end.'

1 = fa 1
1 = fa 1 0 0 0
1 = fa 1 2 3

1 = fa 2
1 = fa 2 3 4
1 = fa 3 4$5 0
1 = fa _1 0 0

1 = fa 1 2.5
1 = fa 0.05
1 = fa _1 0 2.5

1 = fa 1 3j4
1 = fa 0j1

1 = fa '0'
1 = fa (16$0){a.
1 = fa 'abcd'
1 = fa 0 1 2{a.
1 = fa <0

1 = fa 0;0;1
1 = fa i.0
1 = fa ''
1 = fa o.i.4 0 3

0 = fa 0
0 = fa 0 1 1 1
0 = fa 0 1 2
0 = fa i.2 3 4
0 = fa o.i.2 3
0 = fa 0 3j4
0 = fa 0 0 0
0 = fa 3 4$0 1
0 = fa 3 4$0 _2
0 = fa 3 4$0 3.4 5
0 = fa 2 3$0 4j5

fb =. 3 : 0
 if. 
  t=.y+5
  t>10 
 do.   
  1 
 else. 
  0 
 end.
)

1 = fb 5.5
1 = fb 6+i.2 3
0 = fb 4.2
0 = fb 5 6 7

fc =. 3 : 0
 'zero' 
 if. y do. 'one' end.
)

'one'  -: fc 1
'one'  -: fc 2
'zero' -: fc 0
'zero' -: fc 0 1 2

fd =. 3 : 0
 if.
  'zero'
  y 
 do. 
 'one' 
 end.
)

'one'   -: fd 1
'one'   -: fd 2
(i.0 0) -: fd 0
(i.0 0) -: fd 0 1 2

fe =. 3 : '''result'' if. y do. end.'
'result' -: fe 0
'result' -: fe 1
'result' -: fe 1 0 2 0

ff =. 3 : 'if. do. 1 2 3 end.'
1 2 3 -: ff 0
1 2 3 -: ff 4 5
1 2 3 -: ff 'abc'

fg =. 3 : 0
 if.     0=y do. 'zero'
 elseif. 1=y do. 'one'
 elseif. 2=y do. 'two'
 elseif. 1   do. 'big' end.
)

'zero' -: fg 0
'one'  -: fg 1
'two'  -: fg 2
'big'  -: fg 3
'big'  -: fg 17.5
'big'  -: fg 'abc'

fi =. 3 : 'if. y do. ''good'' return. end. ''bad'''

'good' -: fi 1 2 3
'good' -: fi 1
'bad'  -: fi 0

fj =. 3 : 'if. 23=y do. ''is 23'' else. ''not 23'' end.'
'is 23'  -: fj 23
'not 23' -: fj 17

fk =. 3 : 'if. (4-4)+23=y do. ''is 23'' else. ''not 23'' end.'
'is 23'  -: fk 23
'not 23' -: fk 17

fl =. 3 : 'if. (4.5-4.5)+23=y do. ''is 23'' else. ''not 23'' end.'
'is 23'  -: fl 23
'not 23' -: fl 17

fm =. 3 : 'if. (4j5-4j5)+23=y do. ''is 23'' else. ''not 23'' end.'
'is 23'  -: fm 23
'not 23' -: fm 17


4!:55 ;:'fa fb fc fd fe ff fg fi fj fk fl fm t '


