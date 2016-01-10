NB. i.!.0 and associates ------------------------------------------------

NB. i.!.0 using grading and binary search
NB. currently invoked only for boxed arrays where
NB. - each target item has more than one element, or
NB. - some opened target item has more than one numeric element

test=: 4 : 0
 assert.    (~.!.0            -: ~.         ) y
 assert.    (~:!.0            -: ~:         ) y
 assert.    (I.@(~:!.0)       -: I.@~:      ) y
 assert. x  (   (i.!.0)       -:    i.      ) y
 assert.    (x&(i.!.0)        -: x&i.       ) y
 assert. x  (   (i:!.0)       -:    i:      ) y
 assert.    (x&(i:!.0)        -: x&i:       ) y
 assert. x  (   (-.!.0)       -:    -.      ) y
 assert.    (   (-.!.0)&x     -:    -.&x    ) y
 assert. x  (   (e.!.0)       -:    e.      ) y
 assert.    (   (e.!.0)&x     -:    e.&x    ) y
 assert. x  ((e.!.0 i. 0:)    -: (e.i.0:)   ) y
 assert.    ((e.!.0 i. 0:)&x  -: (e.i.0:)&x ) y
 assert. x  ((e.!.0 i. 1:)    -: (e.i.1:)   ) y
 assert.    ((e.!.0 i. 1:)&x  -: (e.i.1:)&x ) y
 assert. x  ((e.!.0 i: 0:)    -: (e.i:0:)   ) y
 assert.    ((e.!.0 i: 0:)&x  -: (e.i:0:)&x ) y
 assert. x  ((e.!.0 i: 1:)    -: (e.i:1:)   ) y
 assert.    ((e.!.0 i: 1:)&x  -: (e.i:1:)&x ) y
 assert. x  (  + /@(e.!.0)    -:   + /@e.   ) y
 assert.    (  + /@(e.!.0)&x  -:   + /@e.&x ) y
 assert. x  (([: +/ e.!.0)    -:   + /@e.   ) y
 assert.    (([: +/ e.!.0)&x  -:   + /@e.&x ) y
 assert. x  (  +./@(e.!.0)    -:   +./@e.   ) y
 assert.    (  +./@(e.!.0)&x  -:   +./@e.&x ) y
 assert. x  (([:+./ e.!.0)    -:   +./@e.   ) y
 assert.    (([:+./ e.!.0)&x  -:   +./@e.&x ) y
 assert. x  (  *./@(e.!.0)    -:   *./@e.   ) y
 assert.    (  *./@(e.!.0)&x  -:   *./@e.&x ) y
 assert. x  (([:*./ e.!.0)    -:   *./@e.   ) y
 assert.    (([:*./ e.!.0)&x  -:   *./@e.&x ) y
 assert. x  (   I.@(e.!.0)    -:    I.@e.   ) y
 assert.    (   I.@(e.!.0)&x  -:    I.@e.&x ) y
 assert. x  (([: I. e.!.0)    -:    I.@e.   ) y
 assert.    (([: I. e.!.0)&x  -:    I.@e.&x ) y
 1
)

x test y [ x=: <"(1) 1019 2?@$100 [ y=: <"(1) 877 2?@$100
y test y

x test y [ x=: i.&.> 1000 ?@# 750 [ y=: i.&.> 1019 ?@# 880
y test y

a=: ;:'chthonic kakistocracy kleptocracy eleemosynary amanuensis paronomasiac'
a=: a,;:'onomatopoeia metonymic metronymic paraclete parousia pauline exegesis'
a=: a,(<"0 ] 20 ?@$ 100)
x test y [ x=: a{~ 811 2?@$ #a [ y=: a{~ 947 2 ?@$#a
y test y

x test y [ x=: 811 0$a: [ y=: 947 0$a:
y test y

NB. mapped boxed arrays 

0!:0 <testpath,'gmbx.ijs'
q=: x=: (811 2 ?@$ #u){u=: 17 3;(<"0] 30 ?@$ 100), (<5!:2 <'mean'), ;:'Cogito, ergo sum.'
r=: y=: (547 2 ?@$ #u){u

(mbxcheck_jmf_ q), q test y
(mbxcheck_jmf_ r), x test r
(mbxcheck_jmf_ q), q test r
(mbxcheck_jmf_ q), q test q


4!:55 ;:'a f f1 g mean test u x y'


