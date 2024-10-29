prolog './g15x.ijs'
NB. 15!: ----------------------------------------------------------------

NB. a small memory leak is expected on the next line
2 = {:15!:1 ((15!:8) 10),0 5 4  NB. reference count

test=: 3 : 0   NB. windows only
1
)

test ''

t=: 100 ?@$ 1e6
t -:      15!:1 (15!:14 <'t'),0,(*/$t),3!:0 t
(15!:14 <'t') = (15!:12 <'t') + 8*SZI
t=: 100 4 1?@$ 0
t -: ($t)$15!:1 (15!:14 <'t'),0,(*/$t),3!:0 t
(15!:14 <'t') = (15!:12 <'t') +10*SZI

'domain error' -: 15!:6  etx <'test'
'domain error' -: 15!:6  etx ;:'t test'
'domain error' -: 15!:6  etx <u:'test'
'domain error' -: 15!:6  etx u:&.> ;:'t test'
'domain error' -: 15!:6  etx <10&u:'test'
'domain error' -: 15!:6  etx 10&u:&.> ;:'t test'
'domain error' -: 15!:6  etx s:@<"0&.> <'test'
'domain error' -: 15!:6  etx <"0@s: <'test'
'domain error' -: 15!:14 etx <'test'
'domain error' -: 15!:14 etx ;:'t test'
'domain error' -: 15!:14 etx <u:'test'
'domain error' -: 15!:14 etx u:&.> ;:'t test'
'domain error' -: 15!:14 etx <10&u:'test'
'domain error' -: 15!:14 etx 10&u:&.> ;:'t test'
'domain error' -: 15!:14 etx s:@<"0&.> <'test'
'domain error' -: 15!:14 etx <"0@s: <'test'

'value error'  -: 15!:6  etx <'undefinedname'
'value error'  -: 15!:6  etx ;:'t undefinedname'
'value error'  -: 15!:14 etx <'undefinedname'
'value error'  -: 15!:14 etx ;:'t undefinedname'

h =: i. 2 3 4
i =: 15!:18 h
i -: h
0 = 64 | 0 { 13!:_4 i
h =: 3 4 (15!:18) 'abc'
h -: 3 4 ($,) 'abc'
0 = 64 | 0 { 13!:_4 h
h =: (15!:18) 'abc'
0 = 64 | 0 { 13!:_4 h


4!:55 ;:'h i jt t test'



epilog''

