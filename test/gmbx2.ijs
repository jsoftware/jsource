NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. ^: ------------------------------------------------------------------

q=: x=: <13
(>:^:x 1) -: >:^:q 1
mbxcheck_jmf_ q


NB. $ -------------------------------------------------------------------

q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($x)          -: $q
(mbxcheck_jmf_ q), (3$x)         -: 3$q
(mbxcheck_jmf_ q), (7$x)         -: 7$q
(mbxcheck_jmf_ q), (3$"0 x)      -: 3$"0 q
(mbxcheck_jmf_ q), (3$"1 x)      -: 3$"1 q
(mbxcheck_jmf_ q), (3$"2 x)      -: 3$"2 q

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (2 5$!.y x)   -: 2 5$!.y q
(mbxcheck_jmf_ q), (2 5$!.y x)   -: 2 5$!.r x
(mbxcheck_jmf_ q), (2 5$!.y x)   -: 2 5$!.r q


NB. $. ------------------------------------------------------------------

q=: x=: 2 3 4;0
(1$.x) -: 1$.q
mbxcheck_jmf_ q

y=: $. 2 3 4 5 ?@$ 5
q=: x=: 2;0 1
(x$.y) -: q$.y
mbxcheck_jmf_ q


NB. ~. ------------------------------------------------------------------

q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (~.   x)      -: ~.   q
(mbxcheck_jmf_ q), (~."0 x)      -: ~."0 q
(mbxcheck_jmf_ q), (~."1 x)      -: ~."1 q
(mbxcheck_jmf_ q), (~."2 x)      -: ~."2 q
q=: x=: <"1 ?20 2 3$5
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (~.x)         -: ~.q
(mbxcheck_jmf_ q), (~.!.0 x)     -: ~.!.0 q


NB. ~: ------------------------------------------------------------------

q=: x=: (?7 2 3$#x){x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (~:j{x)       -: ~: j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (~:"0 j{x)    -: ~:"0 j{q
(mbxcheck_jmf_ q), (~:"1 j{x)    -: ~:"1 j{q
(mbxcheck_jmf_ q), (~:"2 j{x)    -: ~:"2 j{q
q=: x=: <"0 +&.>?10 2 3$10
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (~:    j{x)   -: ~:    j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (~:!.0 j{x)   -: ~:!.0 j{q [ j=: ?$~#x

q=: x=: (?7 2 3$#x){x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x~:j{x)      -: q ~: j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x~:"0 j{x)   -: q ~:"0 j{q
(mbxcheck_jmf_ q), (x~:"1 j{x)   -: q ~:"1 j{q
(mbxcheck_jmf_ q), (x~:"2 j{x)   -: q ~:"2 j{q
(mbxcheck_jmf_ q), (x~:j{x)      -: q ~: j{x
(mbxcheck_jmf_ q), (x~:j{x)      -: (j{q) ~: x
(mbxcheck_jmf_ q), (x~:j{x)      -: (j{q) ~: q
q=: x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), (x~:j{x)      -: q ~: j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x~:j{x)      -: q ~: j{x
(mbxcheck_jmf_ q), (x~:j{x)      -: (j{q) ~: x
(mbxcheck_jmf_ q), (x~:j{x)      -: (j{q) ~: q
(mbxcheck_jmf_ q), (x~:!.0 j{x)  -: (j{q) ~:!.0 q


NB. |. ------------------------------------------------------------------

q=: x=: (?30$#x){x=: <"0 (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (|.x)         -: |. q
(mbxcheck_jmf_ q), (|."1 x)      -: |."1 q
(mbxcheck_jmf_ q), (|."2 x)      -: |."2 q

q=: x=: (?30$#x){x=: <"0 (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 0|.x)       -:  0|. q
(mbxcheck_jmf_ q), ( 1|.x)       -:  1|. q
(mbxcheck_jmf_ q), (_2|.x)       -: _2|. q
(mbxcheck_jmf_ q), ( 0|.!.'' x)  -:  0|.!.'' q
(mbxcheck_jmf_ q), ( 1|.!.'' x)  -:  1|.!.'' q
(mbxcheck_jmf_ q), (_2|.!.'' x)  -: _2|.!.'' q
(mbxcheck_jmf_ q), ( 0|.!.y x)   -:  0|.!.y q [ y=: <?888
(mbxcheck_jmf_ q), ( 1|.!.y x)   -:  1|.!.y q
(mbxcheck_jmf_ q), (_2|.!.y x)   -: _2|.!.y q
(mbxcheck_jmf_ q), ( 0|.!.y x)   -:  0|.!.y q [ y=: 0{q
(mbxcheck_jmf_ q), ( 1|.!.y x)   -:  1|.!.y q
(mbxcheck_jmf_ q), (_2|.!.y x)   -: _2|.!.y q
q=: x=: <"0 <"0 ? 20 2 3$4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 0|.x)       -:  0|. q
(mbxcheck_jmf_ q), ( 1|.x)       -:  1|. q
(mbxcheck_jmf_ q), (_2|.x)       -: _2|. q
(mbxcheck_jmf_ q), ( 1|."0 x)    -:  1|."0 q
(mbxcheck_jmf_ q), ( 1|."1 x)    -:  1|."1 q
(mbxcheck_jmf_ q), ( 1|."2 x)    -:  1|."2 q
(mbxcheck_jmf_ q), ( 0 1|.x)     -:  0 1|. q
(mbxcheck_jmf_ q), ( 2 3 1|.x)   -:  2 3 1|. q
(mbxcheck_jmf_ q), (j|."_1 x)    -: j|."_1 q [ j=: ?(#x)$1{$x

q=: x=: <5!:2 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (|.x)         -: |.q
(mbxcheck_jmf_ q), (2|.x)        -: 2|.q

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (10|.!.y x)   -: 10|.!.y q
(mbxcheck_jmf_ q), (10|.!.y x)   -: 10|.!.r x
(mbxcheck_jmf_ q), (10|.!.y x)   -: 10|.!.r q


NB. |: ------------------------------------------------------------------

q=: x=: 3 7$(<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (|:x)         -: |: q
(mbxcheck_jmf_ q), (0 1|:x)      -: 0 1|: q
(mbxcheck_jmf_ q), ((<0 1)|:x)   -: (<1 0)|: q
q=: x=: 2 3 7$(<"0 ?35$50),(<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (|:x)         -: |: q
(mbxcheck_jmf_ q), (0 1|:x)      -: 0 1|: q
(mbxcheck_jmf_ q), (1 0|:x)      -: 1 0|: q
(mbxcheck_jmf_ q), (1 0|:"2 x)   -: 1 0|:"2 q
(mbxcheck_jmf_ q), (2 0 1|: x)   -: 2 0 1|: q
(mbxcheck_jmf_ q), ((2;0 1)|: x) -: (2;0 1)|: q
(mbxcheck_jmf_ q), ((2 0;1)|: x) -: (2 0;1)|: q

q=: x=: 0 1;2 3
(x|:y) -: q|:y=: ?20 3 10 7$100
mbxcheck_jmf_ q

1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j mean q r t x y'


