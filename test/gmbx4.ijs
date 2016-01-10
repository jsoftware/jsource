NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. # -------------------------------------------------------------------

q=: x=: (?20$#x){x=: <"0 (<5!:2 <'g'), ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: ?(#x)$5
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: ?(#x)$2
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: ?5
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: 0
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: 1
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: j./?(2,#x)$10
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: j./?2$10
(mbxcheck_jmf_ q), (j#x)         -: j # q [ j=: 0j2
q=: x=: <"0 <"0 i.5 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: 1 2 0 4 3
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: 1 1 0 1 1
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: 3 
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: 1 
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: j./?2 5$10
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: j./?2$10 
(mbxcheck_jmf_ q), (j#   x)      -: j #   q [ j=: 0j3 
(mbxcheck_jmf_ q), (j#"1 x)      -: j #"1 q [ j=: 1 2 0 4
(mbxcheck_jmf_ q), (j#"1 x)      -: j #"1 q [ j=: 1 1 0 1
(mbxcheck_jmf_ q), (j#"1 x)      -: j #"1 q [ j=: 3
(mbxcheck_jmf_ q), (j#"1 x)      -: j #"1 q [ j=: 1 
(mbxcheck_jmf_ q), (j#"1 x)      -: j #"1 q [ j=: j./?2 4$10
(mbxcheck_jmf_ q), (j#"1 x)      -: j #"1 q [ j=: j./?2$10

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (3j2#!.y x)   -: 3j2#!.y q
(mbxcheck_jmf_ q), (3j2#!.y x)   -: 3j2#!.r x
(mbxcheck_jmf_ q), (3j2#!.y x)   -: 3j2#!.r q


NB. / -------------------------------------------------------------------

q=: x=: (?2 12$2){(<5!:2 <'g'),< i.2 3 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (= /x)        -: = /q
(mbxcheck_jmf_ q), (~:/x)        -: ~:/q
(mbxcheck_jmf_ q), (, /x)        -: , /q
(mbxcheck_jmf_ q), (; /x)        -: ; /q
(mbxcheck_jmf_ q), (f /x)        -: (f=: =         )/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: ~:        )/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: ,         )/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: ;         )/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: 4 : 'x= y')/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: 4 : 'x~:y')/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: 4 : 'x, y')/q
(mbxcheck_jmf_ q), (f /x)        -: (f=: 4 : 'x; y')/q 
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (= /x)        -: = /q
(mbxcheck_jmf_ q), (~:/x)        -: ~:/q
(mbxcheck_jmf_ q), (, /x)        -: , /q
(mbxcheck_jmf_ q), (; /x)        -: ; /q
f=: =
(mbxcheck_jmf_ q), (f /x)        -: f/q           
f=: ~:
(mbxcheck_jmf_ q), (f /x)        -: f/q        
f=: ,
(mbxcheck_jmf_ q), (f /x)        -: f/q         
f=: ;
(mbxcheck_jmf_ q), (f /x)        -: f/q        
f=: 4 : 'x= y'
(mbxcheck_jmf_ q), (f /x)        -: f/q
f=: 4 : 'x~:y'
(mbxcheck_jmf_ q), (f /x)        -: f/q 
f=: 4 : 'x, y'
(mbxcheck_jmf_ q), (f /x)        -: f/q
f=: 4 : 'x; y' 
(mbxcheck_jmf_ q), (f /x)        -: f/q
q=: x=: +`%
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x/y)         -: q/y=: 1+?5$10
(mbxcheck_jmf_ q), (x/y)         -: q/y=: 1+?5$10x

q=: x=: <"1 ]11 7 ?@$ 100
(,.&.>/x) -: ,.&.>/q
mbxcheck_jmf_ q

q=: x=: <"1 <"0 ] 11 7 ?@$ 100
(,.&.>/x) -: ,.&.>/q
mbxcheck_jmf_ q


NB. /. ------------------------------------------------------------------

q=: x=:     (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x < /.x)     -: q < /. q
(mbxcheck_jmf_ q), (x < /.x)     -: q < /. x
(mbxcheck_jmf_ q), (x < /.x)     -: x < /. q
(mbxcheck_jmf_ q), (x # /.x)     -: q # /. q
(mbxcheck_jmf_ q), (x [ /.x)     -: q [ /. q
(mbxcheck_jmf_ q), (x ] /.x)     -: q ] /. q
(mbxcheck_jmf_ q), (x {./.x)     -: q {./. q
(mbxcheck_jmf_ q), (x {:/.x)     -: q {:/. q
(mbxcheck_jmf_ q), (x }./.x)     -: q }./. q
(mbxcheck_jmf_ q), (x }:/.x)     -: q }:/. q
(mbxcheck_jmf_ q), (x 3&$/.x)    -: q 3&$/. q
q=: x=: <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x < /.x)     -: q < /. q
(mbxcheck_jmf_ q), (x < /.x)     -: q < /. x
(mbxcheck_jmf_ q), (x < /.x)     -: x < /. q
(mbxcheck_jmf_ q), (x # /.x)     -: q # /. q
(mbxcheck_jmf_ q), (x [ /.x)     -: q [ /. q
(mbxcheck_jmf_ q), (x ] /.x)     -: q ] /. q
(mbxcheck_jmf_ q), (x {./.x)     -: q {./. q
(mbxcheck_jmf_ q), (x {:/.x)     -: q {:/. q
(mbxcheck_jmf_ q), (x }./.x)     -: q }./. q
(mbxcheck_jmf_ q), (x }:/.x)     -: q }:/. q
(mbxcheck_jmf_ q), (x 3&$/.x)    -: q 3&$/. q

q=: x=: (<@('a'&,)@":)`(<@('b'&,)@":)`(<@('c'&,)@":)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x/.y)        -: q/.y=: i.4 5


NB. /: ------------------------------------------------------------------

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (/: x)        -: /: q 
(mbxcheck_jmf_ q), (/:~x)        -: /:~q 
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (/: x)        -: /: q 
(mbxcheck_jmf_ q), (/:~x)        -: /:~q 
(mbxcheck_jmf_ q), (/:   x)      -: /:   q 
(mbxcheck_jmf_ q), (/:"1 x)      -: /:"1 q 
(mbxcheck_jmf_ q), (/:"2 x)      -: /:"2 q 

q=: x=: (?7 2$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum.'
r=: y=: (?7 2$#y){y=: (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x/:y)        -: x/:r
(mbxcheck_jmf_ q), (x/:y)        -: q/:y
(mbxcheck_jmf_ q), (x/:y)        -: q/:r


NB. \ -------------------------------------------------------------------

q=: x=: (?5 4 3$2){(<5!:2 <'g'),< i.2 3 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,/\x)        -: ,/\q
f=: ,
(mbxcheck_jmf_ q), (f/\x)        -: f/\q

q=: x=: (?20$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum.' 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 3 <\x)      -:  3 <\q
(mbxcheck_jmf_ q), (_3 <\x)      -: _3 <\q 
(mbxcheck_jmf_ q), ( 30<\x)      -:  30<\q
(mbxcheck_jmf_ q), (_30<\x)      -: _30<\q 
(mbxcheck_jmf_ q), ( 3 [\x)      -:  3 [\q
(mbxcheck_jmf_ q), (_3 [\x)      -: _3 [\q 
(mbxcheck_jmf_ q), ( 30[\x)      -:  30[\q
(mbxcheck_jmf_ q), (_30[\x)      -: _30[\q 
f=: |.
(mbxcheck_jmf_ q), ( 3 f\x)      -:  3 f\q 
(mbxcheck_jmf_ q), (_3 f\x)      -: _3 f\q 
(mbxcheck_jmf_ q), ( 30 f\x)     -:  30 f\q 
(mbxcheck_jmf_ q), (_30 f\x)     -: _30 f\q
q=: x=: (?20$#x){x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.' 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 3 <\x)      -:  3 <\q
(mbxcheck_jmf_ q), (_3 <\x)      -: _3 <\q 
(mbxcheck_jmf_ q), ( 30<\x)      -:  30<\q
(mbxcheck_jmf_ q), (_30<\x)      -: _30<\q 
(mbxcheck_jmf_ q), ( 3 [\x)      -:  3 [\q
(mbxcheck_jmf_ q), (_3 [\x)      -: _3 [\q 
(mbxcheck_jmf_ q), ( 30[\x)      -:  30[\q
(mbxcheck_jmf_ q), (_30[\x)      -: _30[\q 
f=: |.
(mbxcheck_jmf_ q), ( 3 f\x)      -:  3 f\q 
(mbxcheck_jmf_ q), (_3 f\x)      -: _3 f\q 
(mbxcheck_jmf_ q), ( 30 f\x)     -:  30 f\q
(mbxcheck_jmf_ q), (_30 f\x)     -: _30 f\q

q=: x=: (<@('a'&,)@":)`(<@('b'&,)@":)`(<@('c'&,)@":)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x\y)         -: q\y=: i.7
(mbxcheck_jmf_ q), ( 3 x\y)      -:  3 q\y=: i.7
(mbxcheck_jmf_ q), (_3 x\y)      -: _3 q\y=: i.7


NB. \. ------------------------------------------------------------------

q=: x=: <"0 ?12 3$1e6
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,/\.x)       -: ,/\.q
f=: ,
(mbxcheck_jmf_ q), (f/\.x)       -: f/\.q 
f=: 4 : 'x,y'
(mbxcheck_jmf_ q), (f/\.x)       -: f/\.q 
q=: x=: <"0 <"0 ?12 3$1e6
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,/\.x)       -: ,/\.q
f=: ,
(mbxcheck_jmf_ q), (f/\.x)       -: f/\.q
f=: 4 : 'x,y'
(mbxcheck_jmf_ q), (f/\.x)       -: f/\.q 

q=: x=: (?20$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum.' 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 3 <\.x)     -:  3 <\.q
(mbxcheck_jmf_ q), (_3 <\.x)     -: _3 <\.q 
(mbxcheck_jmf_ q), ( 30<\.x)     -:  30<\.q
(mbxcheck_jmf_ q), (_30<\.x)     -: _30<\.q 
(mbxcheck_jmf_ q), ( 3 [\.x)     -:  3 [\.q
(mbxcheck_jmf_ q), (_3 [\.x)     -: _3 [\.q 
(mbxcheck_jmf_ q), ( 30[\.x)     -:  30[\.q
(mbxcheck_jmf_ q), (_30[\.x)     -: _30[\.q 
f=: |.
(mbxcheck_jmf_ q), ( 3 f\.x)     -:  3 f\.q 
(mbxcheck_jmf_ q), (_3 f\.x)     -: _3 f\.q
(mbxcheck_jmf_ q), ( 30 f\.x)    -:  30 f\.q 
(mbxcheck_jmf_ q), (_30 f\.x)    -: _30 f\.q
q=: x=: (?20$#x){x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.' 
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 3 <\.x)     -:  3 <\.q
(mbxcheck_jmf_ q), (_3 <\.x)     -: _3 <\.q 
(mbxcheck_jmf_ q), ( 30<\.x)     -:  30<\.q
(mbxcheck_jmf_ q), (_30<\.x)     -: _30<\.q 
(mbxcheck_jmf_ q), ( 3 [\.x)     -:  3 [\.q
(mbxcheck_jmf_ q), (_3 [\.x)     -: _3 [\.q 
(mbxcheck_jmf_ q), ( 30[\.x)     -:  30[\.q
(mbxcheck_jmf_ q), (_30[\.x)     -: _30[\.q 
f=: |.
(mbxcheck_jmf_ q), ( 3 f\.x)     -:  3 f\.q 
(mbxcheck_jmf_ q), (_3 f\.x)     -: _3 f\.q 
(mbxcheck_jmf_ q), ( 30 f\.x)    -:  30 f\.q
(mbxcheck_jmf_ q), (_30 f\.x)    -: _30 f\.q

q=: x=: (<@('a'&,)@":)`(<@('b'&,)@":)`(<@('c'&,)@":)
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x\.y)        -: q\.y=: i.7
(mbxcheck_jmf_ q), ( 3 x\.y)     -:  3 q\.y=: i.7
(mbxcheck_jmf_ q), (_3 x\.y)     -: _3 q\.y=: i.7


NB. \: ------------------------------------------------------------------

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (\: x)        -: \: q 
(mbxcheck_jmf_ q), (\:~x)        -: \:~q 
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (\: x)        -: \: q 
(mbxcheck_jmf_ q), (\:~x)        -: \:~q 
(mbxcheck_jmf_ q), (\:   x)      -: \:   q 
(mbxcheck_jmf_ q), (\:"1 x)      -: \:"1 q 
(mbxcheck_jmf_ q), (\:"2 x)      -: \:"2 q 

q=: x=: (?7 2$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum.'
r=: y=: (?7 2$#y){y=: (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x\:y)        -: x\:r
(mbxcheck_jmf_ q), (x\:y)        -: q\:y
(mbxcheck_jmf_ q), (x\:y)        -: q\:r


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j mean q r t x y'


