NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. A. ------------------------------------------------------------------

q=: x=: <"0 (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x


NB. C. ------------------------------------------------------------------

q=: x=: (1,0<?50$4) <;.1]51?100
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (C.x)         -: C.q
(mbxcheck_jmf_ q), (x C.y)       -: q C.y=: ?100$1e6
(mbxcheck_jmf_ q), ((<0 _1) C. x)-: (<0 _1) C. q


NB. e. ------------------------------------------------------------------

q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1};:'Cogito, ergo sum.'
r=: (j=: ?30$+:#x){q,t=: <"0 ?(#x)$1e5
y=: j{x,t
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x e. y)      -: q e. r
(mbxcheck_jmf_ q), (x e. y)      -: q e. y
(mbxcheck_jmf_ q), (x e. y)      -: x e. r
f=: e.
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q 
(mbxcheck_jmf_ q), (x e.!.0 y)   -: q e.!.0 r
(mbxcheck_jmf_ q), (x e.!.0 y)   -: q e.!.0 y
(mbxcheck_jmf_ q), (x e.!.0 y)   -: x e.!.0 r
f=: e.!.0
(mbxcheck_jmf_ q), (x f"1 0 x)   -: q f"1 0 q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j mean q r t x y'


