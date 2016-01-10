NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. -. ------------------------------------------------------------------

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x -. j{x)    -: q -. j{q [ j=: ?3$#q
(mbxcheck_jmf_ q), (x -. j{x)    -: q -. j{x 
(mbxcheck_jmf_ q), (x -. j{x)    -: x -. j{q 
q=: x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x -. j{x)    -: q -. j{q [ j=: ?3$#q
(mbxcheck_jmf_ q), (x -. j{x)    -: q -. j{x 
(mbxcheck_jmf_ q), (x -. j{x)    -: x -. j{q 
q=: x=: <"0 +&.>?10 2 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x -. j{x)    -: q -. j{q [ j=: ?3$#q
(mbxcheck_jmf_ q), (x -. j{x)    -: q -. j{x 
(mbxcheck_jmf_ q), (x -. j{x)    -: x -. j{q 


NB. -: ------------------------------------------------------------------

q=: x=: (?5 2 3$#x){x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -:   q
(mbxcheck_jmf_ q), ((0{x)-:"2 x) -: (0{q) -:"2 q
(mbxcheck_jmf_ q), ((1{x)-:"2 x) -: (1{q) -:"2 q
(mbxcheck_jmf_ q), (x -:"0 a:{x) -: x     -:"0 a:{q
(mbxcheck_jmf_ q), (x -:"0 a:{x) -: q     -:"0 a:{x
(mbxcheck_jmf_ q), (x -:"0 a:{x) -: q     -:"0 a:{q
(mbxcheck_jmf_ q), (x -:"1 a:{x) -: x     -:"1 a:{q
(mbxcheck_jmf_ q), (x -:"1 a:{x) -: q     -:"1 a:{x
(mbxcheck_jmf_ q), (x -:"1 a:{x) -: q     -:"1 a:{q
(mbxcheck_jmf_ q), (x -:"2 a:{x) -: x     -:"2 a:{q
(mbxcheck_jmf_ q), (x -:"2 a:{x) -: q     -:"2 a:{x
(mbxcheck_jmf_ q), (x -:"2 a:{x) -: q     -:"2 a:{q
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ((1|.x)-:"1 x)-:"1 (1|.q) -:"1 q
(mbxcheck_jmf_ q), ((1|.x)-:"2 x)-:"1 (1|.q) -:"2 q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j mean q r t x y'


