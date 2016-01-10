NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. : -------------------------------------------------------------------

q=: x=: <"0 ?100$100

f=: 4 : 0
 y=. (<i.>x{y) x}y
)

(mbxcheck_jmf_ q), x             -: q

j=: ?#x
x=: j f x
q=: j f q
(mbxcheck_jmf_ q), x             -: q

f=: 3 : 0
 y=. (-. (3{.&.>y) e. <'NB.')#y
 y=. y -.&.><;:13 10{a.
 y=. ;: ; y
 y=. (-. ({.&>y) e. '''_0123456789')#y
 y=. ~. /:~ y
)

q=: f q=: <;._2 (1!:1) <testpath,'gmbx3.ijs'
x=: f x=: <;._2 (1!:1) <testpath,'gmbx3.ijs'
(mbxcheck_jmf_ q), x             -: q

q=: x=: 't=.y+y';'*:t'
(3 : q -: 3 : x) 2 3 13
mbxcheck_jmf_ q

q=: x=: 't=.y+y';'*:t';':';'x*y'
7 (3 : q -: 3 : x) 2 3 13
mbxcheck_jmf_ q


NB. , -------------------------------------------------------------------

q=: x=: (?10 2 3$#x){x=: <"0 (<5!:2 <'g') ,;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,x)          -: ,q
(mbxcheck_jmf_ q), (,"0 x)       -: ,"0 q
(mbxcheck_jmf_ q), (,"1 x)       -: ,"1 q
(mbxcheck_jmf_ q), (,"2 x)       -: ,"2 q
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,x)          -: ,q
(mbxcheck_jmf_ q), (,"0 x)       -: ,"0 q
(mbxcheck_jmf_ q), (,"1 x)       -: ,"1 q
(mbxcheck_jmf_ q), (,"2 x)       -: ,"2 q

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x, 0 3 1{x)  -: q, 0 3 1{q
(mbxcheck_jmf_ q), (x,~0 3 1{x)  -: q,~0 3 1{q
(mbxcheck_jmf_ q), (x, 0 1{q)    -: q, 0 1{x
(mbxcheck_jmf_ q), (x,~0 1{q)    -: q,~0 1{x
(mbxcheck_jmf_ q), (x, 1{x)      -: q, 1{q
(mbxcheck_jmf_ q), (x,~1{x)      -: q,~1{q
(mbxcheck_jmf_ q), (x, 1{q)      -: q, 1{x
(mbxcheck_jmf_ q), (x,~1{q)      -: q,~1{x

q=: x=: (?10 2 3$#x){x=: (<5!:2 <'g') ,;:'Cogito, ergo sum.'
r=: y=: (?10 2 3$#y){y=: (<5!:2 <'g') ,;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x,y)         -: x,r
(mbxcheck_jmf_ q), (x,y)         -: q,y
(mbxcheck_jmf_ q), (x,y)         -: q,r
(mbxcheck_jmf_ q), (x,"0 y)      -: x,"0 r
(mbxcheck_jmf_ q), (x,"0 y)      -: q,"0 y
(mbxcheck_jmf_ q), (x,"0 y)      -: q,"0 r
(mbxcheck_jmf_ q), (x,"1 y)      -: x,"1 r
(mbxcheck_jmf_ q), (x,"1 y)      -: q,"1 y
(mbxcheck_jmf_ q), (x,"1 y)      -: q,"1 r
(mbxcheck_jmf_ q), (x,"2 y)      -: x,"2 r
(mbxcheck_jmf_ q), (x,"2 y)      -: q,"2 y
(mbxcheck_jmf_ q), (x,"2 y)      -: q,"2 r

q=: x=: 2 3$(<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
t=: <"0 i.1 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x,!.y t)     -: x,!.r t
(mbxcheck_jmf_ q), (x,!.y t)     -: q,!.y t
(mbxcheck_jmf_ q), (x,!.y t)     -: q,!.r t


NB. ,. ------------------------------------------------------------------

q=: x=:     (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,. x)        -: ,. q
q=: x=: (?10 2 3$#x){x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,.x)         -: ,.q
(mbxcheck_jmf_ q), (,."0 x)      -: ,."0 q
(mbxcheck_jmf_ q), (,."1 x)      -: ,."1 q
(mbxcheck_jmf_ q), (,."2 x)      -: ,."2 q
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,.   x)      -: ,. q
(mbxcheck_jmf_ q), (,."1 x)      -: ,."1 q

q=: x=: (?10 2 3$#x){x=: (<5!:2 <'g') ,;:'Cogito, ergo sum.'
r=: y=: (?10 2 3$#y){y=: (<5!:2 <'g') ,;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x,.y)        -: x,.r
(mbxcheck_jmf_ q), (x,.y)        -: q,.y
(mbxcheck_jmf_ q), (x,.y)        -: q,.r
(mbxcheck_jmf_ q), (x,."0 y)     -: x,."0 r
(mbxcheck_jmf_ q), (x,."0 y)     -: q,."0 y
(mbxcheck_jmf_ q), (x,."0 y)     -: q,."0 r
(mbxcheck_jmf_ q), (x,."1 y)     -: x,."1 r
(mbxcheck_jmf_ q), (x,."1 y)     -: q,."1 y
(mbxcheck_jmf_ q), (x,."1 y)     -: q,."1 r
(mbxcheck_jmf_ q), (x,."2 y)     -: x,."2 r
(mbxcheck_jmf_ q), (x,."2 y)     -: q,."2 y
(mbxcheck_jmf_ q), (x,."2 y)     -: q,."2 r

q=: x=: 5 2 3$(<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
t=: <"0 i.5 2 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x,.!.y t)    -: x,.!.r t
(mbxcheck_jmf_ q), (x,.!.y t)    -: q,.!.y t
(mbxcheck_jmf_ q), (x,.!.y t)    -: q,.!.r t


NB. ,: ------------------------------------------------------------------

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,:x)         -: ,:q
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (,:   x)      -: ,:   q
(mbxcheck_jmf_ q), (,:"0 x)      -: ,:"0 q
(mbxcheck_jmf_ q), (,:"1 x)      -: ,:"1 q
(mbxcheck_jmf_ q), (,:"2 x)      -: ,:"2 q

q=: x=:  <"0 (<5!:2 <'g') 1};:'Cogito, ergo sum.'
y=: j{x [ j=: ?~#x
r=: j{q
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), y             -: r
(mbxcheck_jmf_ q), (x,:y)        -: q,:r
(mbxcheck_jmf_ q), (x,:y)        -: q,:y
(mbxcheck_jmf_ q), (x,:y)        -: x,:r

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
t=: 'abc';2
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (x,:!.y t)    -: x,:!.r t
(mbxcheck_jmf_ q), (x,:!.y t)    -: q,:!.y t
(mbxcheck_jmf_ q), (x,:!.y t)    -: q,:!.r t


NB. ; -------------------------------------------------------------------

q=: x=: ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (;x)          -: ;q
q=: x=: (?4$20)#&.> <"0 ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (;x)          -: ;q
q=: x=: <"0@?@(3 4&$)&.>10$100
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (;x)          -: ;q
q=: x=: (i.2 3) ; (i.7) ; i.3 2 1
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (;x)          -: ;q

q=: x=: (?7 2 3$#x){x=: (5!:2 <'mean'),;:'Cogito, ergo sum.'
r=: y=: (?7 2 3$#y){y=: (5!:2 <'mean'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ r), (x;y)         -: x;r
(mbxcheck_jmf_ r), (x;y)         -: q;y
(mbxcheck_jmf_ r), (x;y)         -: q;r
(mbxcheck_jmf_ r), (x;<y)        -: x;<r
(mbxcheck_jmf_ r), (x;<y)        -: q;<y
(mbxcheck_jmf_ r), (x;<y)        -: q;<r
(mbxcheck_jmf_ r), (x;"0 y)      -: x;"0 r
(mbxcheck_jmf_ r), (x;"0 y)      -: q;"0 y
(mbxcheck_jmf_ r), (x;"0 y)      -: q;"0 r
(mbxcheck_jmf_ r), (x;"1 y)      -: x;"1 r
(mbxcheck_jmf_ r), (x;"1 y)      -: q;"1 y
(mbxcheck_jmf_ r), (x;"1 y)      -: q;"1 r
(mbxcheck_jmf_ r), (x;"2 y)      -: x;"2 r
(mbxcheck_jmf_ r), (x;"2 y)      -: q;"2 y
(mbxcheck_jmf_ r), (x;"2 y)      -: q;"2 r

q=: ;:'Cogito, ergo sum.'
mbxcheck_jmf_ q
t=: q;1234567
q=: (<5!:2 <'g') 1}q
t -: q;1234567
t=: q;<q
q=: (<,',') 1}q
t -: q;<q       
mbxcheck_jmf_ q


NB. ;. ------------------------------------------------------------------

q=: x=: (?40$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (< ;.1 x)     -: < ;.1 q
(mbxcheck_jmf_ q), ($ ;.1 x)     -: $ ;.1 q
(mbxcheck_jmf_ q), (# ;.1 x)     -: # ;.1 q
(mbxcheck_jmf_ q), (, ;.1 x)     -: , ;.1 q
(mbxcheck_jmf_ q), ([ ;.1 x)     -: [ ;.1 q
(mbxcheck_jmf_ q), (] ;.1 x)     -: ] ;.1 q
(mbxcheck_jmf_ q), ({.;.1 x)     -: {.;.1 q
(mbxcheck_jmf_ q), ({:;.1 x)     -: {:;.1 q
(mbxcheck_jmf_ q), (}.;.1 x)     -: }.;.1 q
(mbxcheck_jmf_ q), (}:;.1 x)     -: }:;.1 q
(mbxcheck_jmf_ q), (|.;.1 x)     -: |.;.1 q
(mbxcheck_jmf_ q), ({.`{:`[;.1 x)-: {.`{:`[;.1 q
(mbxcheck_jmf_ q), (j < ;.1 x)   -: j < ;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j $ ;.1 x)   -: j $ ;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j # ;.1 x)   -: j # ;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j , ;.1 x)   -: j , ;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j [ ;.1 x)   -: j [ ;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j ] ;.1 x)   -: j ] ;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j {.;.1 x)   -: j {.;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j {:;.1 x)   -: j {:;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j }.;.1 x)   -: j }.;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j }:;.1 x)   -: j }:;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j |.;.1 x)   -: j |.;.1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j {.`{:;.1 x)-: j {.`{:;.1 q [ j=: 0=?(#x)$4
q=: x=: (?40$#x){x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (< ;.1 x)     -: < ;.1 q
(mbxcheck_jmf_ q), ($ ;.1 x)     -: $ ;.1 q
(mbxcheck_jmf_ q), (# ;.1 x)     -: # ;.1 q
(mbxcheck_jmf_ q), (, ;.1 x)     -: , ;.1 q
(mbxcheck_jmf_ q), ([ ;.1 x)     -: [ ;.1 q
(mbxcheck_jmf_ q), (] ;.1 x)     -: ] ;.1 q
(mbxcheck_jmf_ q), ({.;.1 x)     -: {.;.1 q
(mbxcheck_jmf_ q), ({:;.1 x)     -: {:;.1 q
(mbxcheck_jmf_ q), (}.;.1 x)     -: }.;.1 q
(mbxcheck_jmf_ q), (}:;.1 x)     -: }:;.1 q
(mbxcheck_jmf_ q), (|.;.1 x)     -: |.;.1 q
(mbxcheck_jmf_ q), ({.`{:`[;.1 x)-: {.`{:`[;.1 q
(mbxcheck_jmf_ q), (j < ;.2 x)   -: j < ;.2 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j $ ;._1 x)  -: j $ ;._1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j # ;._1 x)  -: j # ;._1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j , ;.2 x)   -: j , ;.2 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j [ ;.2 x)   -: j [ ;.2 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j ] ;.2 x)   -: j ] ;.2 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j {.;.2 x)   -: j {.;.2 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j {:;.2 x)   -: j {:;.2 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j }.;._1 x)  -: j }.;._1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j }:;._1 x)  -: j }:;._1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j |.;._1 x)  -: j |.;._1 q [ j=: 0=?(#x)$4
(mbxcheck_jmf_ q), (j {.`{:;.2 x)-: j {.`{:;.2 q [ j=: 0=?(#x)$4

q=: x=: 1;1 0 1 0 0
y=: 7 5 ?@$ 100
(x <;.1 y) -: q <;.1 y
mbxcheck_jmf_ q


NB. ;: ------------------------------------------------------------------

me=: (i.#a.) e. (a.i.''''),,(a.i.'Aa')+/i.26
se=: 2 2 2 $ 0 0 1 1  0 3 1 0
y=: 'Now is the time all good men'
q=: x=: 0;se;me
(x;:y) -: q;:y
mbxcheck_jmf_ q

q=: x=: (a.&-. ; ]) '''',a.{~,(a.i.'Aa')+/i.26
((0;se;<x);:y) -: (0;se;<q);:y
mbxcheck_jmf_ q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'

4!:55 ;:'f f1 g j me mean q r se t x y'


