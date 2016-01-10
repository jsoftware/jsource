NB. mapped boxed arrays -------------------------------------------------

0!:0 <testpath,'gmbx.ijs'


NB. [ -------------------------------------------------------------------

q=: x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.'
y=: j{x [ j=: ?2 3 4$#x
r=: j{q
(mbxcheck_jmf_ q), x                 -: q
(mbxcheck_jmf_ q), ([x)              -: ([q)
(mbxcheck_jmf_ q), (x[y)             -: (q[r)
(mbxcheck_jmf_ q), (x[y)             -: (x[r)
(mbxcheck_jmf_ q), (x[y)             -: (q[y)


NB. ] -------------------------------------------------------------------

q=: x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.'
y=: j{x [ j=: ?2 3 4$#x
r=: j{q
(mbxcheck_jmf_ q), x                 -: q
(mbxcheck_jmf_ q), (]x)              -: (]q)
(mbxcheck_jmf_ q), (x]y)             -: (q]r)
(mbxcheck_jmf_ q), (x]y)             -: (x]r)
(mbxcheck_jmf_ q), (x]y)             -: (q]y)


NB. { -------------------------------------------------------------------

q=: x=: ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ({ x)         -: {q
(mbxcheck_jmf_ q), (j{x)         -: j{q [ j=: i.#q
(mbxcheck_jmf_ q), (j{x)         -: j{q [ j=: ?20##q

q=: x=: <"0 <"0 i.10 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (1 0{"2 x)    -: 1 0{"2 q
(mbxcheck_jmf_ q), (1 0 1{ x)    -: 1 0 1{ q
(mbxcheck_jmf_ q), (j{x)         -: j{q [ j=: <?$q
(mbxcheck_jmf_ q), (j{x)         -: j{q [ j=: <"1 ?12 3$$q

y=: ?3 5 7 11$1e6
q=: x=: <"1 ?(2 3,$$y)$$y
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x{y)         -: q{y
q=: x=: <0 1;2 3 4;5;6 7 8
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x{y)         -: q{y
q=: x=: (<0 1;2 3 4;5;6 7 8),<1 0;2 4 4;1;8 6 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x{y)         -: q{y
q=: x=: <(<0 1);2 3 4;5;6 7 8
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x{y)         -: q{y
q=: x=: 2 3 4;5;6 7 8
(mbxcheck_jmf_ q), ((<0 1;x){y)   -: (<0 1;q){y

y=: $. (2 3 4 5 ?@$ 10) * 0=2 3 4 5 ?@$ 6
q=: x=: <0;2 1;3
(x{y) -: q{y
mbxcheck_jmf_ q


NB. {. ------------------------------------------------------------------

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 7{.x)       -:  7{.q
(mbxcheck_jmf_ q), (_7{.x)       -: _7{.q
(mbxcheck_jmf_ q), ( 2{.x)       -:  2{.q
(mbxcheck_jmf_ q), (_2{.x)       -: _2{.q
(mbxcheck_jmf_ q), ( 7{.!.y x)   -:  7{.!.y q [ y=: <5!:2 <'mean'
(mbxcheck_jmf_ q), (_7{.!.y x)   -: _7{.!.y q
(mbxcheck_jmf_ q), ( 2{.!.y x)   -:  2{.!.y q
(mbxcheck_jmf_ q), (_2{.!.y x)   -: _2{.!.y q
q=: x=: <"0 <"0 i.2 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 3 7{.x)     -:  3 7{.q
(mbxcheck_jmf_ q), (_3 7{.x)     -: _3 7{.q
(mbxcheck_jmf_ q), ( 3 2{.x)     -:  3 2{.q
(mbxcheck_jmf_ q), (_3 2{.x)     -: _3 2{.q
(mbxcheck_jmf_ q), ( 3 7{.!.y x) -:  3 7{.!.y q [ y=: <5!:2 <'mean'
(mbxcheck_jmf_ q), (_3 7{.!.y x) -: _3 7{.!.y q
(mbxcheck_jmf_ q), ( 3 2{.!.y x) -:  3 2{.!.y q
(mbxcheck_jmf_ q), (_3 2{.!.y x) -: _3 2{.!.y q

q=: x=: (<5!:2 <'g'), ;:'Cogito, ergo sum.'
r=: y=: <(1;2);3
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ r), y             -: r
(mbxcheck_jmf_ q), (10{.!.y x)   -: 10{.!.y q
(mbxcheck_jmf_ q), (10{.!.y x)   -: 10{.!.r x
(mbxcheck_jmf_ q), (10{.!.y x)   -: 10{.!.r q


NB. {: ------------------------------------------------------------------

q=: x=: (?10 2 3$#x){x=: <"0 (<5!:2 <'g'),;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ({:x)         -: {:q
(mbxcheck_jmf_ q), ({:"1 x)      -: {:"1 q
(mbxcheck_jmf_ q), ({:"2 x)      -: {:"2 q
f=: 3 : '{:y'
(mbxcheck_jmf_ q), (f x)         -: f q
(mbxcheck_jmf_ q), (f"1 x)       -: f"1 q
(mbxcheck_jmf_ q), (f"2 x)       -: f"2 q


NB. {:: -----------------------------------------------------------------

q=: x=: 5!:2 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ({::x)        -: {::q
(mbxcheck_jmf_ q), (j{::x)       -: j{::q     [ j=: 0;0;1
(mbxcheck_jmf_ q), (j{::&.><x)   -: j{::&.><q [ j=: < S:1{::q
(mbxcheck_jmf_ q), (< S: 0 q)    -: j{::&.><q [ j=: < S:1{::q

q=: x=: 0;0;0;0
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x{::y)       -: q {:: y=: 5!:2 <'g'

q=: x=: < S: 1 {:: 5!:2 <'g'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), (x{::&.><y)   -: q {::&.> <y=: 5!:2 <'g'


NB. } -------------------------------------------------------------------

q=: x=: (?20$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum. boustrophedonic jet'
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{x) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{q) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{q) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{q) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
q=: (j{x) (i.#q)}q
x=: (j{x) (i.#x)}x
(mbxcheck_jmf_ q), x             -: q

q=: x=: (?7 2 3$#x){x=: (<5!:2 <'g'),;:'Cogito, ergo sum. boustrophedonic jet'
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{x) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{q) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{q) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
x=: (j{q) (i.#x)}x
q=: (j{q) (i.#q)}q
(mbxcheck_jmf_ q), x             -: q
j=: ?$~#x
q=: (j{x) (i.#q)}q
x=: (j{x) (i.#x)}x
(mbxcheck_jmf_ q), x             -: q

q=: x=: ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
t=: <"0 i.12
t=: (<q) 1}t
q=: (<'abc') 2}q
t -: (<0),(<q),<"0 ] 2+i.10
mbxcheck_jmf_ q
q=: (<'ergo') 2}q
t -: (<0),(<x),<"0 ] 2+i.10

q=: x=: ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
q=: (<q) 1}q
x=: (<x) 1}x
(mbxcheck_jmf_ q), x             -: q
q=: (<,',') 1}q
x=: (<,',') 1}x
(mbxcheck_jmf_ q), x             -: q

q=: x=: 0;1
(mbxcheck_jmf_ q), x                 -: q
q=: (i.&.>10000 4200) 0 1}q
x=: (i.&.>10000 4200) 0 1}x
(mbxcheck_jmf_ q), x                 -: q

q=: x=: 0;1;(2000$a:);3
(mbxcheck_jmf_ q), x                 -: q

q=: (<2) 2}q
x=: (<2) 2}x
(mbxcheck_jmf_ q), x                 -: q

q=: (<i.30000) 0}q
x=: (<i.30000) 0}x
(mbxcheck_jmf_ q), x                 -: q

q=: <"0 i.5
x=: <"0 i.5
y=: (<3e6$'boustrophedonic')2}<"0 'abc'
'allocation error' -: ex 'q=: y 0 1 2}q'
(mbxcheck_jmf_ q), x -: q

y=: 3 4 5 ?@$ 100
q=: x=: <(<2);1 3
(_5 x}y) -: _5 q}y
mbxcheck_jmf_ q

q=: x=: <"1 ]5 3 ?@$ $y
(_5 x}y) -: _5 q}y
mbxcheck_jmf_ q

q=: x=: (<1;2 3),<0;2 1
(_5 x}y) -: _5 q}y
mbxcheck_jmf_ q

q=: x=: ;:'one two three four five'
y=: ;:'eins zwei drei vier funf'
t=: 5 ?@$ 2
(t}q,:y) -: t}x,:y
t=: t+2-2
(t}q,:y) -: t}x,:y
mbxcheck_jmf_ q
q=: t}q,:y
mbxcheck_jmf_ q


NB. }. ------------------------------------------------------------------

q=: x=: (<5!:2 <'g') 1};:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 7}.x)       -:  7}.q
(mbxcheck_jmf_ q), (_7}.x)       -: _7}.q
(mbxcheck_jmf_ q), ( 2}.x)       -:  2}.q
(mbxcheck_jmf_ q), (_2}.x)       -: _2}.q
q=: x=: <"0 <"0 i.5 3 4
(mbxcheck_jmf_ q), x             -: q
(mbxcheck_jmf_ q), ( 3  1}.x)    -:  3  1}.q
(mbxcheck_jmf_ q), (_3  1}.x)    -: _3  1}.q
(mbxcheck_jmf_ q), ( 3 _2}.x)    -:  3 _2}.q
(mbxcheck_jmf_ q), (_3 _2}.x)    -: _3 _2}.q

q=: x=: (<"0 ?5$10) ,. +&.>i.5 10
(mbxcheck_jmf_ q), x                 -: q
(mbxcheck_jmf_ q), (f"1 x)           -: f"1 q                [ ".'f=: >@{. $ }.'
(mbxcheck_jmf_ q), (j f"_1 x)        -: j f"_1 q [ j=: ?5$10 [ ".'f=: [ {. ]   '
(mbxcheck_jmf_ q), (({."1 x) f"_1 x) -: ({."1 q) f"_1 q      [ ".'f=: >@[ {. ] '
q=: x=: (<"0 ?5$10) ,. <"0 +&.>i.5 10
(mbxcheck_jmf_ q), x                 -: q
(mbxcheck_jmf_ q), (f"1 x)           -: f"1 q                [ ".'f=: >@{. $ }.'
(mbxcheck_jmf_ q), (j f"_1 x)        -: j f"_1 q [ j=: ?5$10 [ ".'f=: [ {. ]   '
(mbxcheck_jmf_ q), (({."1 x) f"_1 x) -: ({."1 q) f"_1 q      [ ".'f=: >@[ {. ] '


NB. }: ------------------------------------------------------------------

q=: x=: (?10 2 3$#x){x=: <"0 (<5!:2 <'g') , ;:'Cogito, ergo sum.'
(mbxcheck_jmf_ q), x                 -: q
(mbxcheck_jmf_ q), (}:x)         -: }:q
(mbxcheck_jmf_ q), (}:"0 x)      -: }:"0 q
(mbxcheck_jmf_ q), (}:"1 x)      -: }:"1 q
(mbxcheck_jmf_ q), (}:"2 x)      -: }:"2 q


1 [ unmap_jmf_ 'q'
1 [ unmap_jmf_ 'r'


4!:55 ;:'f f1 g j mean q r t x y'


