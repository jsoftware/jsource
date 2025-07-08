prolog './gmbx6.ijs'
NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. ". ------------------------------------------------------------------

NB. literal
q=: x=: ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (".'x')       -: ".'q'
(mbxcheck_jmf_ q), (".'|.x')     -: ".'|.q'

q=: x=: (u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (".'x')       -: ".'q'
(mbxcheck_jmf_ q), (".'|.x')     -: ".'|.q'

q=: x=: (10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (".'x')       -: ".'q'
(mbxcheck_jmf_ q), (".'|.x')     -: ".'|.q'


NB. ": ------------------------------------------------------------------

NB. literal
q=: x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (":x)         -: ":q
(mbxcheck_jmf_ q), (0 1":x)      -: 0 1":q
(mbxcheck_jmf_ q), (0 1":2 3$x)  -: 0 1":2 3$q

q=: x=: <"0 (<5!:2 <'g'),(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (":x)         -: ":q
(mbxcheck_jmf_ q), (0 1":x)      -: 0 1":q
(mbxcheck_jmf_ q), (0 1":2 3$x)  -: 0 1":2 3$q

q=: x=: <"0 (<5!:2 <'g'),(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (":x)         -: ":q
(mbxcheck_jmf_ q), (0 1":x)      -: 0 1":q
(mbxcheck_jmf_ q), (0 1":2 3$x)  -: 0 1":2 3$q

NB. symbol
q=: x=: <"0 (<5!:2 <'g'),<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (":x)         -: ":q
(mbxcheck_jmf_ q), (0 1":x)      -: 0 1":q
(mbxcheck_jmf_ q), (0 1":2 3$x)  -: 0 1":2 3$q

q=: x=: <"0 (<5!:2 <'g'),<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (":x)         -: ":q
(mbxcheck_jmf_ q), (0 1":x)      -: 0 1":q
(mbxcheck_jmf_ q), (0 1":2 3$x)  -: 0 1":2 3$q

q=: x=: <"0 (<5!:2 <'g'),<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (":x)         -: ":q
(mbxcheck_jmf_ q), (0 1":x)      -: 0 1":q
(mbxcheck_jmf_ q), (0 1":2 3$x)  -: 0 1":2 3$q


NB. ` -------------------------------------------------------------------

q=: x=: 1: ` (* $:@<:)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x@.*"0 j)     -: q@.*"0 j=: ?40$30

q=: x=: +`%
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x/j)          -: q/j=: 1+?30$30

NB. literal
q=: x=: ('a'&;)`('b'&;)`('c'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x;._1 j)      -: q;._1 j=: ' bou stro phe don ic'

q=: x=: ('a'&;)`('b'&;)`('c'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x;._1 j)      -: q;._1 j=: u:' BOU STRO PHE DON IC'

q=: x=: ('a'&;)`('b'&;)`('c'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x;._1 j)      -: q;._1 j=: 10&u:' Bou Stro Phe Don Ic'

NB. symbol
q=: x=: ('a'&;)`('b'&;)`('c'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x;._1 j)      -: q;._1 j=: s: ' bou stro phe don ic'

q=: x=: ('a'&;)`('b'&;)`('c'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x;._1 j)      -: q;._1 j=: s: u: 128+a.i. ' BOU STRO PHE DON IC'

q=: x=: ('a'&;)`('b'&;)`('c'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(x;._1 j)      -: q;._1 j=: s: 10&u: 65536+a.i. ' Bou Stro Phe Don Ic'

q=: x=: ('a'&;)`('b'&;)`('c'&;)
r=: y=: ('a'&;)`('b'&;)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ r), (x`y)         -: x`r
(mbxcheck_jmf_ r), (x`y)         -: q`y
(mbxcheck_jmf_ r), (x`y)         -: q`r


NB. @. ------------------------------------------------------------------

q=: x=: +`-`*
(x@.2 0 1) -: q@.2 0 1
mbxcheck_jmf_ q

q=: x=: 0;2;1
(+`-`*@.x) -: +`-`*@.q
mbxcheck_jmf_ q


NB. &.> -----------------------------------------------------------------

NB. literal
q=: x=: ;:'Cogito, ergo sum. boustrophedonic chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: (u:&.>) ;:'COGITO, ERGO SUM. BOUSTROPHEDONIC CHTHONIC'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: (10&u:&.>) ;:'Cogito, Ergo Sum. Boustrophedonic Chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
NB. symbol
q=: x=: <@(s:"0) ;:'Cogito, ergo sum. boustrophedonic chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: <@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM. BOUSTROPHEDONIC CHTHONIC'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: <@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum. Boustrophedonic Chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x) ,&.>x) -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
NB. literal
q=: x=: (<5!:2 <'g'), <"0 ;:'Cogito, ergo sum. boustrophedonic chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: (<5!:2 <'g'), <"0 (u:&.>) ;:'COGITO, ERGO SUM. BOUSTROPHEDONIC CHTHONIC'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: (<5!:2 <'g'), <"0 (10&u:&.>) ;:'Cogito, Ergo Sum. Boustrophedonic Chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
NB. symbol
q=: x=: (<5!:2 <'g'), <"0 <@(s:"0) ;:'Cogito, ergo sum. boustrophedonic chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: (<5!:2 <'g'), <"0 <@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM. BOUSTROPHEDONIC CHTHONIC'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 
q=: x=: (<5!:2 <'g'), <"0 <@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum. Boustrophedonic Chthonic'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ($&.> x)      -: $&.> q
(mbxcheck_jmf_ q), (|.&.> x)     -: |.&.> q
(mbxcheck_jmf_ q), (3|.&.> x)    -: 3|.&.> q
(mbxcheck_jmf_ q), (x i.&.>'o')  -: q i.&.>'o'
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.x) ,&.>{:q 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:x 
(mbxcheck_jmf_ q), (({.x),&.>{:x)-: ({.q) ,&.>{:q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{x) ,&.>q 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>x 
(mbxcheck_jmf_ q), ((0{x),&.>x)  -: (0{q) ,&.>q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: x ,&.>0{q 
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{x
(mbxcheck_jmf_ q), (x,&.> 0{x)   -: q ,&.>0{q
(mbxcheck_jmf_ q), (x,&.>x)      -: x ,&.>q
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>x
(mbxcheck_jmf_ q), (x,&.>x)      -: q ,&.>q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: x ,&.>|.q
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.x
(mbxcheck_jmf_ q), (x,&.>|.x)    -: q ,&.>|.q
 

1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'
1!:55 ::1: f,f1




epilog''

