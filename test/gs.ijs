prolog './gs.ijs'
NB. S: ------------------------------------------------------------------

phi=: * -.@%@~.&.q:      NB. Euler's totient function
t=: 5!:2 <'phi'

(;:'* -.@%@~.&.q:') -: < S: 0 t

(i. 6 4) -: ] S:0 S:1  <"2 <"1 <"0 i. 2 3 4
(i. 1 24) -: ] S:0 S:2  <"2 <"1 <"0 i. 2 3 4
(<"0 i. 1 6 4) -: ] S:1 S:2  <"2 <"1 <"0 i. 2 3 4

(>: i. (QKTEST{10000 100)) -: >: S:0 <"0 i. (QKTEST{10000 100)
3 : 'for. i. 100 do. assert. (>: i. (QKTEST{10000 100)) -: >: S:0 <"0 i. (QKTEST{10000 100) end.' ''  NB. check for memory leak

(>: i. (QKTEST{10000 100)) -: 1 + S:0 <"0 i. (QKTEST{10000 100)
(>: i. (QKTEST{10000 100)) -: (<1) + S:0 <"0 i. (QKTEST{10000 100)

(>: i. 6 4) -: 1 + S:0 S:1  <"2 <"1 <"0 i. 2 3 4
(>: i. 1 24) -: 1 + S:0 S:2  <"2 <"1 <"0 i. 2 3 4



'domain error' -: ] S: 1 etx 1;<<1 2 3
'domain error' -: ; S: 2 etx 1;<<1 2 3




epilog''

