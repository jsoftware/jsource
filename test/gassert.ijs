NB. assert --------------------------------------------------------------

1: 9!:41 ]1

lf=: 10{a.

f=: 3 : 0
 assert. y
 1
)

1 -: f 1
1 -: f 1 1 1
1 -: f (?3$5)$1
1 -: f ''
1 -: f }. 314 1 1 1
1 -: f }. 3.4 1 1 1
1 -: f }. 3j4 1 1 1
1 -: f }. 31x 1 1 1
1 -: f }. 3r4 1 1 1

'assertion failure' -: f etx 0 
'assertion failure' -: f etx 0 0 0
'assertion failure' -: f etx 1 1 1 0
'assertion failure' -: f etx ' 2 3'
'assertion failure' -: f etx 1;2 3
'assertion failure' -: f etx 1 2 3
'assertion failure' -: f etx 1 2.3
'assertion failure' -: f etx 1 2j3
'assertion failure' -: f etx 1 2r3
'assertion failure' -: f etx 1 233x
'assertion failure' -: f etx s: ' 1 2 3'

s=: 1 (5!:7) <'f'
(2{.>s{~<0;1) -: 25 65535

s=: 5!:5 <'f'
s -: '3 : 0',lf,' assert. y',lf,' 1',lf,')'

word=: 3 : 0
 assert. 1>:#$y
 assert. y e. a.
 assert. +./ 'aeiouyAEIOUY' e. ,y
 1
)

word 'kerygmatic'
word 'lucubrations'

'assertion failure' -: word etx 1 2 3
'assertion failure' -: word etx ,: 'kerygmatic'
'assertion failure' -: word etx 'zzz'

'control error' -: ex '3 : s' [ s=: 'assert.',lf,'if. y do. 1 end.'
'control error' -: ex '3 : s' [ s=: 'assert.',lf,'assert. 0=y',lf,'2'

s=: 0 : 0
 if. y do. 'true' else. 'false' end.
 assert.
)

'control error' -: ex '3 : s' 


4!:55 ;:'f lf s word'

