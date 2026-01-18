prolog './gibs.ijs'
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

x test y [ x=: <"(1) (QKTEST{::1019 2;119 2)?@$100 [ y=: <"(1) (QKTEST{::877 2;877 2)?@$100
y test y

x test y [ x=: i.&.> (QKTEST{::1000 100) ?@# 750 [ y=: i.&.> (QKTEST{::1019 119) ?@# 880
y test y

NB. literal
a=: ;:'chthonic kakistocracy kleptocracy eleemosynary amanuensis paronomasiac'
a=: a,;:'onomatopoeia metonymic metronymic paraclete parousia pauline exegesis'
a=: a,(<"0 ] 20 ?@$ 100)
x test y [ x=: a{~ (QKTEST{::811 2;81 2) ?@$ #a [ y=: a{~ (QKTEST{::947 2;947 2) ?@$#a
y test y

x test y [ x=: (QKTEST{::811 0;81 0) $a: [ y=: (QKTEST{::947 0;47 0) $a:
y test y

NB. literal2
a=: ([: u: 128+a.&i.)&.> ;:'chthonic kakistocracy kleptocracy eleemosynary amanuensis paronomasiac'
a=: a,([: u: 128+a.&i.)&.> ;:'onomatopoeia metonymic metronymic paraclete parousia pauline exegesis'
a=: a,(<"0 ] 20 ?@$ 100)
x test y [ x=: a{~ (QKTEST{::811 2;81 2) ?@$ #a [ y=: a{~ (QKTEST{::947 2;947 2) ?@$#a
y test y

x test y [ x=: (QKTEST{::811 0;81 0) $a: [ y=: (QKTEST{::947 0;47 0) $a:
y test y

NB. literal4
a=: (10 u: 65536+3&u:)&.> ;:'chthonic kakistocracy kleptocracy eleemosynary amanuensis paronomasiac'
a=: a,(10 u: 65536+3&u:)&.> ;:'onomatopoeia metonymic metronymic paraclete parousia pauline exegesis'
a=: a,(<"0 ] 20 ?@$ 100)
x test y [ x=: a{~ (QKTEST{::811 2;81 2) ?@$ #a [ y=: a{~ (QKTEST{::947 2;947 2) ?@$#a
y test y

x test y [ x=: (QKTEST{::811 0;81 0)$a: [ y=: (QKTEST{::947 0;47 0)$a:
y test y

NB. symbol
a=: s:@<"0&.> ;:'chthonic kakistocracy kleptocracy eleemosynary amanuensis paronomasiac'
a=: <"0@s: ;:'chthonic kakistocracy kleptocracy eleemosynary amanuensis paronomasiac'
a=: a,s:@<"0&.> ;:'onomatopoeia metonymic metronymic paraclete parousia pauline exegesis'
a=: a,<"0@s: ' onomatopoeia paraclete parousia paronomasiac parousia paraclete onomatopoeia paraclete'
a=: a,<"0@s: ;:'onomatopoeia metonymic metronymic paraclete parousia pauline exegesis'
a=: a,(<"0 ] 20 ?@$ 100)
x test y [ x=: a{~ (QKTEST{::811 2;81 2) ?@$ #a [ y=: a{~ (QKTEST{::947 2;947 2) ?@$#a
y test y

x test y [ x=: (QKTEST{::811 0;81 0) $a: [ y=: (QKTEST{::947 0;47 0) $a:
y test y

(600 ?@$ 1000) (I.@e. -: I.@(e."e.)) 465 ?@$ 1000
(7!:2 'a I.@e. i. 4') > 2 * (7!:2 'a I.@(e."e.) i. 4') [ a =: i. 2000


epilog''

