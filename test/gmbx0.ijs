prolog './gmbx0.ijs'
NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. = -------------------------------------------------------------------

NB. literal
q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: (<5!:2 <'g') 1}(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: (<5!:2 <'g') 1}(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

NB. symbol
q=: x=: (<5!:2 <'g') 1}<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: (<5!:2 <'g') 1}<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: (<5!:2 <'g') 1}<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

NB. literal
q=: x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: <"0 (<5!:2 <'g') 1}(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: <"0 (<5!:2 <'g') 1}(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: <"0 +&.>?10 2 3$10
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

NB. symbol
q=: x=: <"0 (<5!:2 <'g') 1}<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: <"0 (<5!:2 <'g') 1}<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: <"0 (<5!:2 <'g') 1}<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

q=: x=: <"0 +&.>?10 2 3$10
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (=j{x)        -: = j{q [ j=: ?$~#x

NB. literal
q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = q

q=: x=: (<5!:2 <'g') 1}(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = q

q=: x=: (<5!:2 <'g') 1}(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = q

NB. symbol
q=: x=: (<5!:2 <'g') 1}<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = q

q=: x=: (<5!:2 <'g') 1}<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = q

q=: x=: (<5!:2 <'g') 1}<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{q [ j=: ?$~#x
(mbxcheck_jmf_ q), (x=j{x)       -: q = j{x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = x
(mbxcheck_jmf_ q), (x=j{x)       -: (j{q) = q

NB. literal
q=: x=: (?10 3$#x){x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{q [ j=: ?#x
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 q

q=: x=: (?10 3$#x){x=: <"0 (<5!:2 <'g') 1}(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{q [ j=: ?#x
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 q

q=: x=: (?10 3$#x){x=: <"0 (<5!:2 <'g') 1}(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{q [ j=: ?#x
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 q

NB. symbol
q=: x=: (?10 3$#x){x=: <"0 (<5!:2 <'g') 1}<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{q [ j=: ?#x
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 q

q=: x=: (?10 3$#x){x=: <"0 (<5!:2 <'g') 1}<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{q [ j=: ?#x
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 q

q=: x=: (?10 3$#x){x=: <"0 (<5!:2 <'g') 1}<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{q [ j=: ?#x
(mbxcheck_jmf_ q), (x="1 j{x)    -: q ="1 j{x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 x
(mbxcheck_jmf_ q), (x="1 j{x)    -: (j{q) ="1 q


NB. =: ------------------------------------------------------------------

NB. literal
q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q

q=: x=: (<5!:2 <'g'), (u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q

q=: x=: (<5!:2 <'g'), (10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q

NB. symbol
q=: x=: (<5!:2 <'g'), <@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q

q=: x=: (<5!:2 <'g'), <@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q

q=: x=: (<5!:2 <'g'), <@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q

x=: 2 2 2 2 2 2 2$x
q=: 2 2 2 2 2 2 2$q
(mbxcheck_jmf_ q), x             -: q

x=: x;2
q=: q;2
(mbxcheck_jmf_ q), x             -: q

x=: >{.x
q=: >{.q
(mbxcheck_jmf_ q), x             -: q

x=: ,x
q=: ,q
(mbxcheck_jmf_ q), x             -: q

'allocation error' -: ex 'q=: i.&.>10^i.7'

NB. < -------------------------------------------------------------------

NB. literal
q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (<x)          -: <q
(mbxcheck_jmf_ q), (<"0 x)       -: <"0 q
(mbxcheck_jmf_ q), (<"1 x)       -: <"1 q
(mbxcheck_jmf_ q), (<"2 x)       -: <"2 q

q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'),(u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (<x)          -: <q
(mbxcheck_jmf_ q), (<"0 x)       -: <"0 q
(mbxcheck_jmf_ q), (<"1 x)       -: <"1 q
(mbxcheck_jmf_ q), (<"2 x)       -: <"2 q

q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'),(10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (<x)          -: <q
(mbxcheck_jmf_ q), (<"0 x)       -: <"0 q
(mbxcheck_jmf_ q), (<"1 x)       -: <"1 q
(mbxcheck_jmf_ q), (<"2 x)       -: <"2 q

NB. symbol
q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'),<@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (<x)          -: <q
(mbxcheck_jmf_ q), (<"0 x)       -: <"0 q
(mbxcheck_jmf_ q), (<"1 x)       -: <"1 q
(mbxcheck_jmf_ q), (<"2 x)       -: <"2 q

q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'),<@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (<x)          -: <q
(mbxcheck_jmf_ q), (<"0 x)       -: <"0 q
(mbxcheck_jmf_ q), (<"1 x)       -: <"1 q
(mbxcheck_jmf_ q), (<"2 x)       -: <"2 q

q=: x=: (?2 3 4$#x){x=: (<5!:2 <'g'),<@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (<x)          -: <q
(mbxcheck_jmf_ q), (<"0 x)       -: <"0 q
(mbxcheck_jmf_ q), (<"1 x)       -: <"1 q
(mbxcheck_jmf_ q), (<"2 x)       -: <"2 q


NB. > -------------------------------------------------------------------

NB. literal
q=: x=: ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>  x)        -: >    q
(mbxcheck_jmf_ q), (>{.x)        -: > {. q

q=: x=: (u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>  x)        -: >    q
(mbxcheck_jmf_ q), (>{.x)        -: > {. q

q=: x=: (10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>  x)        -: >    q
(mbxcheck_jmf_ q), (>{.x)        -: > {. q

NB. symbol
q=: x=: <@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>  x)        -: >    q
(mbxcheck_jmf_ q), (>{.x)        -: > {. q

q=: x=: <@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>  x)        -: >    q
(mbxcheck_jmf_ q), (>{.x)        -: > {. q

q=: x=: <@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>  x)        -: >    q
(mbxcheck_jmf_ q), (>{.x)        -: > {. q

NB. literal
q=: x=: <"0 ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >   q
(mbxcheck_jmf_ q), (>{.x)        -: > {.q
x=: (<1;2;3;4) 1}x
q=: (<1;2;3;4) 1}q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(>x)           -: >q

q=: x=: <"0 (u:&.>) ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >   q
(mbxcheck_jmf_ q), (>{.x)        -: > {.q
x=: (<1;2;3;4) 1}x
q=: (<1;2;3;4) 1}q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(>x)           -: >q

q=: x=: <"0 (10&u:&.>) ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >   q
(mbxcheck_jmf_ q), (>{.x)        -: > {.q
x=: (<1;2;3;4) 1}x
q=: (<1;2;3;4) 1}q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(>x)           -: >q

NB. symbol
q=: x=: <"0 <@(s:"0) ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >   q
(mbxcheck_jmf_ q), (>{.x)        -: > {.q
x=: (<1;2;3;4) 1}x
q=: (<1;2;3;4) 1}q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(>x)           -: >q

q=: x=: <"0 <@(s:"0) ([: u: 128+a.&i.)&.> ;:'COGITO, ERGO SUM.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >   q
(mbxcheck_jmf_ q), (>{.x)        -: > {.q
x=: (<1;2;3;4) 1}x
q=: (<1;2;3;4) 1}q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(>x)           -: >q

q=: x=: <"0 <@(s:"0) (10 u: 65536+a.&i.)&.> ;:'Cogito, Ergo Sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >   q
(mbxcheck_jmf_ q), (>{.x)        -: > {.q
x=: (<1;2;3;4) 1}x
q=: (<1;2;3;4) 1}q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q),(>x)           -: >q

NB. literal
q=: x=: ((<4$<'x'),<<"0 ]2 3$'abcdef') ((1;0;2);<0;1;0)} <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >q

q=: x=: ((<4$<'x'),<<"0 ]2 3$u:'ABCDEF') ((1;0;2);<0;1;0)} <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >q

q=: x=: ((<4$<'x'),<<"0 ]2 3$10&u:'AbCdEf') ((1;0;2);<0;1;0)} <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >q

NB. symbol
q=: x=: ((<4$<'x'),<<"0 ]<@(s:"0) 2 3$'abcdef') ((1;0;2);<0;1;0)} <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >q

q=: x=: ((<4$<'x'),<<"0 ]<@(s:"0) 2 3$u:'ABCDEF') ((1;0;2);<0;1;0)} <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >q

q=: x=: ((<4$<'x'),<<"0 ]<@(s:"0) 2 3$10&u:'AbCdEf') ((1;0;2);<0;1;0)} <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (>x)          -: >q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'
1!:55 ::1: f,f1




epilog''

