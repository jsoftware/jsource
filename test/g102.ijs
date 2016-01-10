NB. +:y -----------------------------------------------------------------

test=:+~"_ -: +:
test 1=?2 3 4$2
test _20+?50$50
test o. _20+?50$50
test r. ?3 4 5$100
test i.0 3 4
test i.3 0

dr  =: 5!:2
mr  =: 1 : ('f=.+"x'; '{. >2{ dr <''f''')
lr  =: 1 : '5!:5 <''x'''

0 -: +: mr
0 -: -: mr
0 -: *: mr
0 -: %: mr

(+: b. _1) -: -: lr
(-: b. _1) -: +: lr
(*: b. _1) -: %: lr
(%: b. _1) -: *: lr

7 -: +: 3.5

'domain error' -: +: etx 'abc'
'domain error' -: +: etx 3;4 5
'domain error' -: +: etx <!.0?5$2


NB. x+:y ----------------------------------------------------------------

(2 2$1 0 0 0) -: +:/~ 0 1
1 0 0 0 -: 0 0 1 1 +: 0 1 0 1

(2 2$1 0 0 0) -: +:/~ 0 1{0 1 4 5 6
1 0 0 0 -: 0 0 1 1 +: 0 1 0 1{0 1 4 5 6
1 0 0 0 -: 0 0 1 1 +:~0 1 0 1{0 1 4 5 6

(2 2$1 0 0 0) -: +:/~ 0 1{0 1 4.5 6 _7.89
1 0 0 0 -: 0 0 1 1 +: 0 1 0 1{0 1 4.5 6 _7.89
1 0 0 0 -: 0 0 1 1 +:~0 1 0 1{0 1 4.5 6 _7.89

(2 2$1 0 0 0) -: +:/~ 0 1{0 1 4.5j6 _7.89
1 0 0 0 -: 0 0 1 1 +: 0 1 0 1{0 1 4.5j6 _7.89
1 0 0 0 -: 0 0 1 1 +:~0 1 0 1{0 1 4.5j6 _7.89

'domain error' -: 0 1 +: etx 'ab'
'domain error' -: 0 1 +:~etx 'ab'
'domain error' -: 0 1 +: etx 2
'domain error' -: 0 1 +:~etx 2
'domain error' -: 0 1 +: etx 3.4  0
'domain error' -: 0 1 +:~etx 3.4  0
'domain error' -: 0 1 +: etx 0j1  1
'domain error' -: 0 1 +:~etx 0j1  1
'domain error' -: 1   +: etx <'asfd'
'domain error' -: 1   +:~etx <'asfd'

'length error' -: 0 1   +: etx 0 1 0
'length error' -: 0 1   +:~etx 0 1 0
'length error' -: 0 1 0 +: etx ?4 3$2
'length error' -: 0 1 0 +:~etx ?4 3$2

4!:55 ;:'dr lr mr test'


