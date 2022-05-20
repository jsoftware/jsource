prolog './gassert.ijs'
NB. assert --------------------------------------------------------------

ws=: 9!:40 ''
9!:41 ]1  NB. retain comments and whitespace

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
word u:'kerygmatic'
word u:'lucubrations'
word 10&u:'kerygmatic'
word 10&u:'lucubrations'

'assertion failure' -: word etx 1 2 3
'assertion failure' -: word etx ,: 'kerygmatic'
'assertion failure' -: word etx ,: u:'kerygmatic'
'assertion failure' -: word etx ,: 10&u:'kerygmatic'
'assertion failure' -: word etx 'zzz'
'assertion failure' -: word etx u:'zzz'
'assertion failure' -: word etx 10&u:'zzz'

'control error' -: ex '3 : s' [ s=: 'assert.',lf,'if. y do. 1 end.'
'control error' -: ex '3 : s' [ s=: 'assert.',lf,'assert. 0=y',lf,'2'
'control error' -: ex '3 : s' [ s=: 'assert. if. do. end.'
'control error' -: ex '3 : s' [ s=: 'assert. assert.'


s=: 0 : 0
 if. y do. 'true' else. 'false' end.
 assert.
)

'control error' -: ex '3 : s' 

NB. assert doesn't run when 0=9!:35''
f =: 3 : 0
assert. 0 < y =. +: y
y
)
4 -: f 2
'assertion failure' -: f etx  _2
9!:35 ] 0
2 -: f 2
_2 -: f _2
9!:35 ] 1



9!:41 ws

4!:55 ;:'f lf s word ws'


epilog''

