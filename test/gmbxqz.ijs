NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. s: ------------------------------------------------------------------

q=: x=: ;:'Cogito, ergo sum. ignorance apathy'
(mbxcheck_jmf_ q), x           -: q
(mbxcheck_jmf_ q), (  s: x)    -:     s: q
(mbxcheck_jmf_ q), (< s: x)    -: <   s: q
(mbxcheck_jmf_ q), (x;s: x)    -: x ; s: q
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: x
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: q

q=: x=: 0 s: 10
(mbxcheck_jmf_ q), x -: q
(mbxcheck_jmf_ q), 10 s: q
(mbxcheck_jmf_ q), x -: 0 s: 10
(mbxcheck_jmf_ q), q -: 0 s: 10


NB. S: ------------------------------------------------------------------

q=: x=: 5!:2 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (< S: 0 x)    -: <   S: 0 q
(mbxcheck_jmf_ q), (x;S: 0 x)    -: x ; S: 0 q
(mbxcheck_jmf_ q), (x;S: 0 x)    -: q ; S: 0 x
(mbxcheck_jmf_ q), (x;S: 0 x)    -: q ; S: 0 q

q=: x=: 5!:2 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x (; <@;) S: 0 1 {:: x) -: (x (; <@;) S: 0 1 {:: q)
(mbxcheck_jmf_ q), (x (; <@;) S: 0 1 {:: x) -: (q (; <@;) S: 0 1 {:: x)
(mbxcheck_jmf_ q), (x (; <@;) S: 0 1 {:: x) -: (q (; <@;) S: 0 1 {:: q)

q=: x=: 5!:2 <'g'
r=: y=: 5!:2 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x (; <@;) S: 0 1 {:: y) -: (x (; <@;) S: 0 1 {:: r)
(mbxcheck_jmf_ q), (x (; <@;) S: 0 1 {:: y) -: (q (; <@;) S: 0 1 {:: y)
(mbxcheck_jmf_ q), (x (; <@;) S: 0 1 {:: y) -: (q (; <@;) S: 0 1 {:: r)


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j mean q r t x y'


