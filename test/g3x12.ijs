prolog './g3x12.ijs'
NB. 3!:12 ----------------------------------------------------------------

ToLower=: 3 : 0`((((97+i.26){a.)(65+i.26)}a.) {~ a. i. ])@.(2 = 3!:0)
x=. I. 26 > n=. ((65+i.26){a.) i. t=. ,y
($y) $ ((x{n) { (97+i.26){a.) x}t
)

ToUpper=: 3 : 0`((((65+i.26){a.)(97+i.26)}a.) {~ a. i. ])@.(2 = 3!:0)
x=. I. 26 > n=. ((97+i.26){a.) i. t=. ,y
($y) $ ((x{n) { (65+i.26){a.) x}t
)

(ToLower -: 0&(3!:12)) a.
(ToLower -: 0&(3!:12)) 234 123$a.
(ToLower -: 0&(3!:12)) u: a.
(ToLower -: 0&(3!:12)) u: 234 123$a.
(ToLower -: 0&(3!:12)) 10&u: a.
(ToLower -: 0&(3!:12)) 10&u: 234 123$a.
(ToLower -: 0&(3!:12)) 7&u: 16b1ffff+i.256
(ToLower -: 0&(3!:12)) 234 123$ 7&u: 16b1ffff+i.256
(ToLower -: 0&(3!:12)) 9&u: 1bf7fff000 +i.256
(ToLower -: 0&(3!:12)) 234 123$ 9&u: 1bf7fff000 +i.256
(ToLower -: 0&(3!:12)) 2 16$'AbCdEf'
(ToLower -: 0&(3!:12)) 2 16$u: 'AbCdEf'
(ToLower -: 0&(3!:12)) 2 16$10&u: 'AbCdEf'
(ToLower -: 0&(3!:12)) i. 3 4
(ToLower -: 0&(3!:12)) 0.5 + i. 3 4
(ToLower -: 0&(3!:12)) 1j1 + i. 3 4
(ToLower -: 0&(3!:12)) x: i. 3 4
(ToLower -: 0&(3!:12)) 1r2 3r4
(ToLower -: 0&(3!:12)) <a.
(ToLower -: 0&(3!:12)) <234 123$a.
(ToLower -: 0&(3!:12)) 3 4$<a.
(ToLower -: 0&(3!:12)) 3 4$<234 123$a.
(ToLower -: 0&(3!:12))&> 3 4$<a.
(ToLower -: 0&(3!:12))&> 3 4$<234 123$a.

(ToUpper -: 1&(3!:12)) a.
(ToUpper -: 1&(3!:12)) 234 123$a.
(ToUpper -: 1&(3!:12)) u: a.
(ToUpper -: 1&(3!:12)) u: 234 123$a.
(ToUpper -: 1&(3!:12)) 10&u: a.
(ToUpper -: 1&(3!:12)) 10&u: 234 123$a.
(ToUpper -: 1&(3!:12)) 7&u: 16b1ffff+i.256
(ToUpper -: 1&(3!:12)) 234 123$ 7&u: 16b1ffff+i.256
(ToUpper -: 1&(3!:12)) 9&u: 1bf7fff000 +i.256
(ToUpper -: 1&(3!:12)) 234 123$ 9&u: 1bf7fff000 +i.256
(ToUpper -: 1&(3!:12)) 2 16$'AbCdEf'
(ToUpper -: 1&(3!:12)) 2 16$u: 'AbCdEf'
(ToUpper -: 1&(3!:12)) 2 16$10&u: 'AbCdEf'
(ToUpper -: 1&(3!:12)) i. 3 4
(ToUpper -: 1&(3!:12)) 0.5 + i. 3 4
(ToUpper -: 1&(3!:12)) 1j1 + i. 3 4
(ToUpper -: 1&(3!:12)) x: i. 3 4
(ToUpper -: 1&(3!:12)) 1r2 3r4
(ToUpper -: 1&(3!:12)) <a.
(ToUpper -: 1&(3!:12)) <234 123$a.
(ToUpper -: 1&(3!:12)) 3 4$<a.
(ToUpper -: 1&(3!:12)) 3 4$<234 123$a.
(ToUpper -: 1&(3!:12))&> 3 4$<a.
(ToUpper -: 1&(3!:12))&> 3 4$<234 123$a.

(] -: 0&(3!:12)) $. i. 3 4
(] -: 1&(3!:12)) $. i. 3 4
'nonce error' -: ToLower etx $. i. 3 4
'nonce error' -: ToUpper etx $. i. 3 4

'domain error' -: _1&(3!:12) etx ''
'domain error' -: 2&(3!:12) etx ''

4!:55 ;:'ToLower ToUpper'

epilog''

