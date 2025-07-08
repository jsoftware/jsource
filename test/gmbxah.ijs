prolog './gmbxah.ijs'
NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. A. ------------------------------------------------------------------

NB. literal
q=: x=: <"0 (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x

q=: x=: <"0 (<5!:2 <'g'), (u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x

q=: x=: <"0 (<5!:2 <'g'), (10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x

NB. symbol
q=: x=: <"0 (<5!:2 <'g'), <@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x

q=: x=: <"0 (<5!:2 <'g'), <@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x

q=: x=: <"0 (<5!:2 <'g'), <@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j A. x)      -: j A. q [ j=: ?20$#x


NB. C. ------------------------------------------------------------------

q=: x=: (1,0<?50$4) <;.1]51?100
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (C.x)         -: C.q
(mbxcheck_jmf_ q), (x C.y)       -: q C.y=: ?100$1e6
(mbxcheck_jmf_ q), ((<0 _1) C. x)-: (<0 _1) C. q


NB. e. ------------------------------------------------------------------

NB. literal
q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1}(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1}(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

NB. symbol
q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1}<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1}<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

q=: x=: (?50$#x){x=: <"0 (<5!:2 <'g') 1}<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q
q=: x=: (?50$#x){x=: <"1 ?10 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (e. x)        -: e. q
(mbxcheck_jmf_ q), (e.!.0 x)     -: e.!.0 q

NB. literal
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

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1}(u:&.>) ;:'COGITO, ERGO SUM.'
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

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1}(10&u:&.>) ;:'Cogito, Ergo Sum.'
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

NB. symbol
q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1}<@(s:"0) ;:'Cogito, ergo sum.'
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

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1}<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
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

q=: x=: (?50$#x){x=:     (<5!:2 <'g') 1}<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
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
1!:55 ::1: f,f1




epilog''

