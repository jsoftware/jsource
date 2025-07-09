prolog './g132.ijs'
NB. %:y -----------------------------------------------------------------

test =: ^&0.5 -: %:

test 1=?2 3 4$2
test _20+?50$50
test o. _20+?50$50
test r. ?3 4 5$100
test i.0 3 4
test i.3 0

1e_15 > | 1.41421356237309504880 - %:  2
1e_15 > | 1.73205080756887729352 - %:  3
1e_15 > | 3.16227766016837933199 - %: 10
1e_15 > | 1.61803398874989482820 - -:>:%:5

1e_15 > | 0j1.41421356237309504880 - %: _2

'domain error' -: %: etx 'abc'
'domain error' -: %: etx u:'abc'
'domain error' -: %: etx 10&u:'abc'
'domain error' -: %: etx s:@<"0 'abc'
'domain error' -: %: etx 3;4 5


NB. x%:y ----------------------------------------------------------------

'domain error' -: 3 4 %: etx 'ab'
'domain error' -: 3 4 %:~etx 'ab'
'domain error' -: 3 4 %: etx u:'ab'
'domain error' -: 3 4 %:~etx u:'ab'
'domain error' -: 3 4 %: etx 10&u:'ab'
'domain error' -: 3 4 %:~etx 10&u:'ab'
'domain error' -: 3 4 %: etx s:@<"0 'ab'
'domain error' -: 3 4 %:~etx s:@<"0 'ab'
'domain error' -: 3 4 %: etx 3;4 5
'domain error' -: 3 4 %:~etx 3;4 5

'length error' -: 3 4 %: etx i.4 3
'length error' -: 3 4 %:~etx i.4 3




epilog''

