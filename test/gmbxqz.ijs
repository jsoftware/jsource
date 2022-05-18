prolog './gmbxqz.ijs'
NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'

q=: x=: (] , <@(s:"0)) ;:'Cogito, ergo sum. ignorance apathy'
(mbxcheck_jmf_ q), x           -: q
q=: x=: (] , <@(s:"0)) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM. IGNORANCE APATHY'
(mbxcheck_jmf_ q), x           -: q
q=: x=: (] , <@(s:"0)) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum. Ignorance Apathy'
(mbxcheck_jmf_ q), x           -: q

NB. s: ------------------------------------------------------------------

q=: x=: ;:'Cogito, ergo sum. ignorance apathy'
(mbxcheck_jmf_ q), x           -: q
(mbxcheck_jmf_ q), (  s: x)    -:     s: q
(mbxcheck_jmf_ q), (< s: x)    -: <   s: q
(mbxcheck_jmf_ q), (x;s: x)    -: x ; s: q
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: x
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: q

q=: x=: (u:&.>) ;:'COGITO, ERGO SUM. IGNORANCE APATHY'
(mbxcheck_jmf_ q), x           -: q
(mbxcheck_jmf_ q), (  s: x)    -:     s: q
(mbxcheck_jmf_ q), (< s: x)    -: <   s: q
(mbxcheck_jmf_ q), (x;s: x)    -: x ; s: q
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: x
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: q

q=: x=: (10&u:&.>) ;:'Cogito, Ergo Sum. Ignorance Apathy'
(mbxcheck_jmf_ q), x           -: q
(mbxcheck_jmf_ q), (  s: x)    -:     s: q
(mbxcheck_jmf_ q), (< s: x)    -: <   s: q
(mbxcheck_jmf_ q), (x;s: x)    -: x ; s: q
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: x
(mbxcheck_jmf_ q), (x;s: x)    -: q ; s: q

NB. symbols allocated but never disposed, size increased on re-run
1 [ unmap_jmf_ 'q1'
4!:55 ;:'q1'
f=: <jpath'~temp/q1.jmf'
1!:55 ::1: f
1 [ createjmf_jmf_ f,< 4e6 >. <.1.1* 7!:5 <'x' [[ x=: 0 s: 10
map_jmf_ (<'q1'),f,'';0
'' -: q1

q1=: x=: 0 s: 10
(mbxcheck_jmf_ q1), x -: q1
(mbxcheck_jmf_ q1), 10 s: q1
(mbxcheck_jmf_ q1), x -: 0 s: 10
(mbxcheck_jmf_ q1), q1 -: 0 s: 10

1 [ unmap_jmf_ 'q1'
4!:55 ;:'q1'
1!:55 ::1: f

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



epilog''

