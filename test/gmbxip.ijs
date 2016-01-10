NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. i. ------------------------------------------------------------------

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x i. x)      -: q i. q
(mbxcheck_jmf_ q), (x i. x)      -: q i. x
(mbxcheck_jmf_ q), (x i. x)      -: x i. q
f=: i.
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q  
(mbxcheck_jmf_ q), (x i.!.0 x)   -: q i.!.0 q
(mbxcheck_jmf_ q), (x i.!.0 x)   -: q i.!.0 x
(mbxcheck_jmf_ q), (x i.!.0 x)   -: x i.!.0 q
f=: i.!.0
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q 
q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x i. x)      -: q i. q
(mbxcheck_jmf_ q), (x i. x)      -: q i. x
(mbxcheck_jmf_ q), (x i. x)      -: x i. q
f=: i.
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q
(mbxcheck_jmf_ q), (x i.!.0 x)   -: q i.!.0 q
(mbxcheck_jmf_ q), (x i.!.0 x)   -: q i.!.0 x
(mbxcheck_jmf_ q), (x i.!.0 x)   -: x i.!.0 q
f=: i.!.0
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q
q=: x=: (?60$#x){x=: <"0 ] 10 3$(<5!:2 <'g'),+&.>?29$1e6
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x i. x)      -: q i. q
(mbxcheck_jmf_ q), (x i. x)      -: q i. x
(mbxcheck_jmf_ q), (x i. x)      -: x i. q
(mbxcheck_jmf_ q), (x i.!.0 x)   -: q i.!.0 q
(mbxcheck_jmf_ q), (x i.!.0 x)   -: q i.!.0 x
(mbxcheck_jmf_ q), (x i.!.0 x)   -: x i.!.0 q

q=: x=: <"1 ] 100 3 ?@$ 5
(i.!.0~ x) -: i.!.0~q
mbxcheck_jmf_ q


NB. i: ------------------------------------------------------------------

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x i: x)      -: q i: q
(mbxcheck_jmf_ q), (x i: x)      -: q i: x
(mbxcheck_jmf_ q), (x i: x)      -: x i: q
f=: i:
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q 
(mbxcheck_jmf_ q), (x i:!.0 x)   -: q i:!.0 q
(mbxcheck_jmf_ q), (x i:!.0 x)   -: q i:!.0 x
(mbxcheck_jmf_ q), (x i:!.0 x)   -: x i:!.0 q
f=: i:!.0
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q
q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x i: x)      -: q i: q
(mbxcheck_jmf_ q), (x i: x)      -: q i: x
(mbxcheck_jmf_ q), (x i: x)      -: x i: q
f=: i:
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q
(mbxcheck_jmf_ q), (x i:!.0 x)   -: q i:!.0 q
(mbxcheck_jmf_ q), (x i:!.0 x)   -: q i:!.0 x
(mbxcheck_jmf_ q), (x i:!.0 x)   -: x i:!.0 q
f=: i:!.0
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q
q=: x=: (?60$#x){x=: <"0 ] 10 3$(<5!:2 <'g'),+&.>?29$1e6
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x i: x)      -: q i: q
(mbxcheck_jmf_ q), (x i: x)      -: q i: x
(mbxcheck_jmf_ q), (x i: x)      -: x i: q
(mbxcheck_jmf_ q), (x i:!.0 x)   -: q i:!.0 q
(mbxcheck_jmf_ q), (x i:!.0 x)   -: q i:!.0 x
(mbxcheck_jmf_ q), (x i:!.0 x)   -: x i:!.0 q


NB. I. ------------------------------------------------------------------

q=: x=: /:~ <"0 ]89 ?@$ 79
r=: y=:     <"0 ]97 ?@$ 89
(x I. y) -: q I. y
(x I. y) -: x I. r
(x I. y) -: q I. r
mbxcheck_jmf_ q
mbxcheck_jmf_ r

q=: x=: /:~ <"0 ]89 3 ?@$ 5
r=: y=:     <"0 ]97 3 ?@$ 5
(x I. y) -: q I. y
(x I. y) -: x I. r
(x I. y) -: q I. r
mbxcheck_jmf_ q
mbxcheck_jmf_ r


NB. L. ------------------------------------------------------------------

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (L. x)        -: L. q
(mbxcheck_jmf_ q), (L.&.>x)      -: L.&.> q


NB. L: ------------------------------------------------------------------

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (#  L: _1 x)  -: #  L: _1 q
(mbxcheck_jmf_ q), (": L:  0 x)  -: ": L:  0 q
(mbxcheck_jmf_ q), (x ,L:  1 x)  -: x ,L: 1 q
(mbxcheck_jmf_ q), (x ,L:  1 x)  -: q ,L: 1 x
(mbxcheck_jmf_ q), (x ,L:  1 x)  -: q ,L: 1 q


NB. p. ------------------------------------------------------------------

y=: -: _10+?4 5$20
q=: x=: < ?3$10 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (p. x)        -: p. q
(mbxcheck_jmf_ q), (x p. y)      -: q p. y
q=: x=: 2; ?3$10 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (p. x)        -: p. q
(mbxcheck_jmf_ q), (x p. y)      -: q p. y
q=: x=: < o. _5+?3$10 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (p. x)        -: p. q
(mbxcheck_jmf_ q), (x p. y)      -: q p. y
q=: x=: 2;o. _5+?3$10 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (p. x)        -: p. q
(mbxcheck_jmf_ q), (x p. y)      -: q p. y
q=: x=: < j./_5+?2 3$10 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (p. x)        -: p. q
(mbxcheck_jmf_ q), (x p. y)      -: q p. y
q=: x=: 2;j./_5+?2 3$10 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (p. x)        -: p. q
(mbxcheck_jmf_ q), (x p. y)      -: q p. y

q=: y=: <3.5 11
x=: <11 22 33 ,. 2 3 5 ,. 1 3 7
(x p. y) -: x p. q
mbxcheck_jmf_ q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j mean q r t x y'


