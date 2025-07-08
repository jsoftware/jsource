prolog './g232.ijs'
NB. |: ------------------------------------------------------------------

randuni''

NB. mask  =: =/ i.@>:@(>./)
NB. vec   =: >@{@:(i.&.>)@((<./ .+) 127&*@-.)
NB. ind   =: vec +/ .* (#. |:)
NB. mask  =: i.@>:@(>./) =/ ] 
NB. canta =: ($@] ind mask@[) { ,@]

X     =: +/ .*
mask  =: = /: ~.
rho   =: <./@#
ind   =: (#:i.)@:rho X #.~
canta =: (mask@[ ind $@]) { ,@]

rank  =: #@$
pfill =: (i.@[ -. |) , |
en    =: - #@;
ci    =: (/:@pfill ;) { i.@en , en + (#&> # i.@#)@]
cant2 =: (rank@] ci [) canta ]

cant1 =: i.@-@#@$ |: ]

vfy =: 3 : 0
 :
 a =. (#$y) pfill x
 ((x|:y) -: a|:y), ((x|:y) -: (+&.>x)|:y), ($x|:y) -: a{$y
 )

x =: a.{~?(?~5)$256
(|:x) -: (|.i.$$x)|:x
x     -: ''|:x
x     -: _1|:x
0 1     vfy x
(p=:?~#$x) vfy x

NB. Boolean
(p=:(?#$a)?#$a) vfy a =: 1=?(4?6)$2
(|: -: cant1) a
(|: -: cant1) a=:? 8 32$2
(|: -: cant1) a=:?32  8$2
(|: -: cant1) a=:? 8  8$2
(|: -: cant1) a=:?13 13$2
(|: -: cant1) a=:?13  7$2
(|: -: cant1) a=:? 7 13$2

NB. literal
(p=:(?#$a)?#$a) vfy a =: a.{~?(4?6)$256
(|: -: cant1) a
(|: -: cant1) a=:a.{~? 8 32$256
(|: -: cant1) a=:a.{~?32  8$256
(|: -: cant1) a=:a.{~? 8  8$256
(|: -: cant1) a=:a.{~?13 13$256
(|: -: cant1) a=:a.{~?13  7$256
(|: -: cant1) a=:a.{~? 7 13$256

NB. literal2
(p=:(?#$a)?#$a) vfy a =: adot1{~?(4?6)$(#adot1)
(|: -: cant1) a
(|: -: cant1) a=:adot1{~? 8 32$(#adot1)
(|: -: cant1) a=:adot1{~?32  8$(#adot1)
(|: -: cant1) a=:adot1{~? 8  8$(#adot1)
(|: -: cant1) a=:adot1{~?13 13$(#adot1)
(|: -: cant1) a=:adot1{~?13  7$(#adot1)
(|: -: cant1) a=:adot1{~? 7 13$(#adot1)

NB. literal4
(p=:(?#$a)?#$a) vfy a =: adot2{~?(4?6)$(#adot2)
(|: -: cant1) a
(|: -: cant1) a=:adot2{~? 8 32$(#adot2)
(|: -: cant1) a=:adot2{~?32  8$(#adot2)
(|: -: cant1) a=:adot2{~? 8  8$(#adot2)
(|: -: cant1) a=:adot2{~?13 13$(#adot2)
(|: -: cant1) a=:adot2{~?13  7$(#adot2)
(|: -: cant1) a=:adot2{~? 7 13$(#adot2)

NB. symbol
(p=:(?#$a)?#$a) vfy a =: sdot0{~?(4?6)$(#sdot0)
(|: -: cant1) a
(|: -: cant1) a=:sdot0{~? 8 32$(#sdot0)
(|: -: cant1) a=:sdot0{~?32  8$(#sdot0)
(|: -: cant1) a=:sdot0{~? 8  8$(#sdot0)
(|: -: cant1) a=:sdot0{~?13 13$(#sdot0)
(|: -: cant1) a=:sdot0{~?13  7$(#sdot0)
(|: -: cant1) a=:sdot0{~? 7 13$(#sdot0)

NB. integer
(p=:(?#$a)?#$a) vfy a =: ?(4?6)$111256
(|: -: cant1) a
(|: -: cant1) a=:_1e6+? 8 32$2e6
(|: -: cant1) a=:_1e6+?32  8$2e6
(|: -: cant1) a=:_1e6+? 8  8$2e6
(|: -: cant1) a=:_1e6+?13 13$2e6
(|: -: cant1) a=:_1e6+?13  7$2e6
(|: -: cant1) a=:_1e6+? 7 13$2e6

NB. floating point
(p=:(?#$a)?#$a) vfy a =: o.?(4?6)$111256
(|: -: cant1) a
(|: -: cant1) a=:o._1e6+? 8 32$2e6
(|: -: cant1) a=:o._1e6+?32  8$2e6
(|: -: cant1) a=:o._1e6+? 8  8$2e6
(|: -: cant1) a=:o._1e6+?13 13$2e6
(|: -: cant1) a=:o._1e6+?13  7$2e6
(|: -: cant1) a=:o._1e6+? 7 13$2e6

NB. complex
(p=:(?#$a)?#$a) vfy a =: ^0j1*?(4?6)$111
(|: -: cant1) a
(|: -: cant1) a=:r._1e6+? 8 32$2e6
(|: -: cant1) a=:r._1e6+?32  8$2e6
(|: -: cant1) a=:r._1e6+? 8  8$2e6
(|: -: cant1) a=:r._1e6+?13 13$2e6
(|: -: cant1) a=:r._1e6+?13  7$2e6
(|: -: cant1) a=:r._1e6+? 7 13$2e6

NB. boxed
x =: (+&.>?20$100), ;:'((?#$a)?#$a) vfy a =: ^0j1*?(4?6)$111'
(p=:(?#$a)?#$a) vfy a =: x{~?(4?6)$#x
(|: -: cant1) a
(|: -: cant1) a=:x{~? 8 32$#x=:;:'deip no so phist epi cur ean ex cell ence'
(|: -: cant1) a=:x{~?32  8$#x
(|: -: cant1) a=:x{~? 8  8$#x
(|: -: cant1) a=:x{~?13 13$#x
(|: -: cant1) a=:x{~?13  7$#x
(|: -: cant1) a=:x{~? 7 13$#x
(|: -: cant1) a=:x{~? 8 32$#x=:(u:&.>) ;:'deip no so phist epi cur ean ex cell ence'
(|: -: cant1) a=:x{~?32  8$#x
(|: -: cant1) a=:x{~? 8  8$#x
(|: -: cant1) a=:x{~?13 13$#x
(|: -: cant1) a=:x{~?13  7$#x
(|: -: cant1) a=:x{~? 7 13$#x
(|: -: cant1) a=:x{~? 8 32$#x=:(10&u:&.>) ;:'deip no so phist epi cur ean ex cell ence'
(|: -: cant1) a=:x{~?32  8$#x
(|: -: cant1) a=:x{~? 8  8$#x
(|: -: cant1) a=:x{~?13 13$#x
(|: -: cant1) a=:x{~?13  7$#x
(|: -: cant1) a=:x{~? 7 13$#x
(|: -: cant1) a=:x{~? 8 32$#x=:s:@<"0&.> ;:'deip no so phist epi cur ean ex cell ence'
(|: -: cant1) a=:x{~? 8 32$#x=:<"0@s: ;:'deip no so phist epi cur ean ex cell ence'
(|: -: cant1) a=:x{~?32  8$#x
(|: -: cant1) a=:x{~? 8  8$#x
(|: -: cant1) a=:x{~?13 13$#x
(|: -: cant1) a=:x{~?13  7$#x
(|: -: cant1) a=:x{~? 7 13$#x

id0 =: =&i.
id1 =: 1: (<0 1)&|:@i.@$@]} ($&0)&(,~)
(id0 4) -: id1 4
(id0 0) -: id1 0
(id0 7) -: id1 7

t -: |: t=:'a'
t -: |: t=:9
t -: |: t=:3j4
t -: |: t=:<i.3 4

t -: ''|: t=:'a'
t -: ''|: t=:9
t -: ''|: t=:3j4
t -: ''|: t=:<i.3 4

(x=:2 2?2) (|: -: cant2"1 _) y=:?  5 7$100
(x=:3 3?3) (|: -: cant2"1 _) y=:?3 5 7$100

((i.4 6)A. i.4) (|: -: cant2)"1 _ x=:?5 6 7 8$2e6


NB. |: main diagonals ---------------------------------------------------

diag =: <@;~"0@:i.@(<./)@$ { ]

(diag -: (<0 1)&|:) x=:?4 5$100
(diag -: (<0 1)&|:) x=:?5 2$100
(diag -: (<0 1)&|:) x=:?5 5$100

((<0 1)&|:&.(<"_2) x) -: (0 1;+&.>2}.i.#$x)|:x=:a.{~?2 3      $#256
((<0 1)&|:&.(<"_2) x) -: (0 1;+&.>2}.i.#$x)|:x=:a.{~?2 3 4    $#256
((<0 1)&|:&.(<"_2) x) -: (0 1;+&.>2}.i.#$x)|:x=:a.{~?2 3 4 1  $#256
((<0 1)&|:&.(<"_2) x) -: (0 1;+&.>2}.i.#$x)|:x=:a.{~?2 3 4 1 2$#256

(0 1;2;3) (|: -: cant2) x=:?2 3 4 5$100
(0 1;2;3) (|: -: cant2) x=:?3 2 4 5$100
(0 1;2;3) (|: -: cant2) x=:?3 3 4 5$100

'' -: (<0 1)|: x=:?0 5$100
'' -: (<0 1)|: x=:?5 0$100
'' -: (<0 1)|: x=:?0 0$100

(i.2 0 5) -: (0 1;2;3) |: x=:?2 3 0 5$100
(i.0 4 5) -: (0 1;2;3) |: x=:?0 3 4 5$100
(i.0 2 3) -: (0 1;2;3) |: x=:?0 0 2 3$100


NB. |: on matrices ------------------------------------------------------

mi   =: i.@{: +/ {: * i.@{.
cant =: mi@$ { ,

NB. Boolean
(    |: -: cant) 1=?50 45$2
(1 0&|: -: cant) 1=?40 60$2
(|:"2 -: cant"2) ?3 1 100$2
(|:"2 -: cant"2) ?3 100 0$2
(|:"2 -: cant"2) a=:?   8 32$2
(|:"2 -: cant"2) a=:?4 32  8$2
(|:"2 -: cant"2) a=:?5  8  8$2
(|:"2 -: cant"2) a=:?1 13 13$2
(|:"2 -: cant"2) a=:?3 13  7$2
(|:"2 -: cant"2) a=:?2  7 13$2

NB. literal
(  |: -: cant) (?45 45$#a){a=:'foo upon thee 1=?10 20$2'
(0&|: -: cant) (?10 200$#a){a
(|:"2 -: cant"2) (?400 1$#a){a
(|:"2 -: cant"2) (?0 1000$#a){a
(|:"2 -: cant"2) a=:a.{~?   8 32$256
(|:"2 -: cant"2) a=:a.{~?4 32  8$256
(|:"2 -: cant"2) a=:a.{~?5  8  8$256
(|:"2 -: cant"2) a=:a.{~?1 13 13$256
(|:"2 -: cant"2) a=:a.{~?3 13  7$256
(|:"2 -: cant"2) a=:a.{~?2  7 13$256

NB. literal2
(  |: -: cant) (?45 45$#a){a=:u:'foo upon thee 1=?10 20$2'
(0&|: -: cant) (?10 200$#a){a
(|:"2 -: cant"2) (?400 1$#a){a
(|:"2 -: cant"2) (?0 1000$#a){a
(|:"2 -: cant"2) a=:adot1{~?   8 32$(#adot1)
(|:"2 -: cant"2) a=:adot1{~?4 32  8$(#adot1)
(|:"2 -: cant"2) a=:adot1{~?5  8  8$(#adot1)
(|:"2 -: cant"2) a=:adot1{~?1 13 13$(#adot1)
(|:"2 -: cant"2) a=:adot1{~?3 13  7$(#adot1)
(|:"2 -: cant"2) a=:adot1{~?2  7 13$(#adot1)

NB. literal4
(  |: -: cant) (?45 45$#a){a=:10&u:'foo upon thee 1=?10 20$2'
(0&|: -: cant) (?10 200$#a){a
(|:"2 -: cant"2) (?400 1$#a){a
(|:"2 -: cant"2) (?0 1000$#a){a
(|:"2 -: cant"2) a=:adot2{~?   8 32$(#adot2)
(|:"2 -: cant"2) a=:adot2{~?4 32  8$(#adot2)
(|:"2 -: cant"2) a=:adot2{~?5  8  8$(#adot2)
(|:"2 -: cant"2) a=:adot2{~?1 13 13$(#adot2)
(|:"2 -: cant"2) a=:adot2{~?3 13  7$(#adot2)
(|:"2 -: cant"2) a=:adot2{~?2  7 13$(#adot2)

NB. symbol
(  |: -: cant) (?45 45$#a){a=:s:@<"0 'foo upon thee 1=?10 20$2'
(0&|: -: cant) (?10 200$#a){a
(|:"2 -: cant"2) (?400 1$#a){a
(|:"2 -: cant"2) (?0 1000$#a){a
(|:"2 -: cant"2) a=:sdot0{~?   8 32$(#sdot0)
(|:"2 -: cant"2) a=:sdot0{~?4 32  8$(#sdot0)
(|:"2 -: cant"2) a=:sdot0{~?5  8  8$(#sdot0)
(|:"2 -: cant"2) a=:sdot0{~?1 13 13$(#sdot0)
(|:"2 -: cant"2) a=:sdot0{~?3 13  7$(#sdot0)
(|:"2 -: cant"2) a=:sdot0{~?2  7 13$(#sdot0)

NB. integer
(    |: -: cant) ?50 50$212341
(1 0&|: -: cant) ?60 45$212341
(|:"2 -: cant"2) ?400 1$123541
(|:"2 -: cant"2) ?0 1000$123456
(|:"2 -: cant"2) a=:_1e6+?   8 32$2e6
(|:"2 -: cant"2) a=:_1e6+?4 32  8$2e6
(|:"2 -: cant"2) a=:_1e6+?5  8  8$2e6
(|:"2 -: cant"2) a=:_1e6+?1 13 13$2e6
(|:"2 -: cant"2) a=:_1e6+?3 13  7$2e6
(|:"2 -: cant"2) a=:_1e6+?2  7 13$2e6

NB. floating point
(  |: -: cant) o.?35 35$212341
(0&|: -: cant) o.?30 40$212341
(|:"2 -: cant"2) o.?1 400$123541
(|:"2 -: cant"2) o.?100 0$123456
(|:"2 -: cant"2) a=:o._1e6+?   8 32$2e6
(|:"2 -: cant"2) a=:o._1e6+?4 32  8$2e6
(|:"2 -: cant"2) a=:o._1e6+?5  8  8$2e6
(|:"2 -: cant"2) a=:o._1e6+?1 13 13$2e6
(|:"2 -: cant"2) a=:o._1e6+?3 13  7$2e6
(|:"2 -: cant"2) a=:o._1e6+?2  7 13$2e6

NB. complex
(    |: -: cant) ^0j1*?30 25$21234
(1 0&|: -: cant) ^0j1*?25 25$21234
(|:"2 -: cant"2) ^0j1*?1 256$1235
(|:"2 -: cant"2) ^0j1*?127 0$1234
(|:"2 -: cant"2) a=:r._1e6+?   8 32$2e6
(|:"2 -: cant"2) a=:r._1e6+?4 32  8$2e6
(|:"2 -: cant"2) a=:r._1e6+?5  8  8$2e6
(|:"2 -: cant"2) a=:r._1e6+?1 13 13$2e6
(|:"2 -: cant"2) a=:r._1e6+?3 13  7$2e6
(|:"2 -: cant"2) a=:r._1e6+?2  7 13$2e6

NB. boxed
(  |: -: cant) (?30 25$#x){x=:;:'Cogito, ergo sum. +/2 12 341 CBC News'
(0&|: -: cant) (?30 30$#x){x
(|:"2 -: cant"2) (?255 1$#x){x
(|:"2 -: cant"2) (?0 0$#x){x
(|:"2 -: cant"2) a=:x{~?   8 32$#x
(|:"2 -: cant"2) a=:x{~?4 32  8$#x
(|:"2 -: cant"2) a=:x{~?5  8  8$#x
(|:"2 -: cant"2) a=:x{~?1 13 13$#x
(|:"2 -: cant"2) a=:x{~?3 13  7$#x
(|:"2 -: cant"2) a=:x{~?2  7 13$#x
(  |: -: cant) (?30 25$#x){x=:(u:&.>) ;:'Cogito, ergo sum. +/2 12 341 CBC News'
(0&|: -: cant) (?30 30$#x){x
(|:"2 -: cant"2) (?255 1$#x){x
(|:"2 -: cant"2) (?0 0$#x){x
(|:"2 -: cant"2) a=:x{~?   8 32$#x
(|:"2 -: cant"2) a=:x{~?4 32  8$#x
(|:"2 -: cant"2) a=:x{~?5  8  8$#x
(|:"2 -: cant"2) a=:x{~?1 13 13$#x
(|:"2 -: cant"2) a=:x{~?3 13  7$#x
(|:"2 -: cant"2) a=:x{~?2  7 13$#x
(  |: -: cant) (?30 25$#x){x=:(10&u:&.>) ;:'Cogito, ergo sum. +/2 12 341 CBC News'
(0&|: -: cant) (?30 30$#x){x
(|:"2 -: cant"2) (?255 1$#x){x
(|:"2 -: cant"2) (?0 0$#x){x
(|:"2 -: cant"2) a=:x{~?   8 32$#x
(|:"2 -: cant"2) a=:x{~?4 32  8$#x
(|:"2 -: cant"2) a=:x{~?5  8  8$#x
(|:"2 -: cant"2) a=:x{~?1 13 13$#x
(|:"2 -: cant"2) a=:x{~?3 13  7$#x
(|:"2 -: cant"2) a=:x{~?2  7 13$#x
(  |: -: cant) (?30 25$#x){x=:s:@<"0&.> ;:'Cogito, ergo sum. +/2 12 341 CBC News'
(  |: -: cant) (?30 25$#x){x=:<"0@s: ;:'Cogito, ergo sum. +/2 12 341 CBC News'
(0&|: -: cant) (?30 30$#x){x
(|:"2 -: cant"2) (?255 1$#x){x
(|:"2 -: cant"2) (?0 0$#x){x
(|:"2 -: cant"2) a=:x{~?   8 32$#x
(|:"2 -: cant"2) a=:x{~?4 32  8$#x
(|:"2 -: cant"2) a=:x{~?5  8  8$#x
(|:"2 -: cant"2) a=:x{~?1 13 13$#x
(|:"2 -: cant"2) a=:x{~?3 13  7$#x
(|:"2 -: cant"2) a=:x{~?2  7 13$#x

(1 0 2    |:x) -: |:&.(<"_2) x=:?3 4 5$1000
(1 0 2 3  |:x) -: |:&.(<"_2) x=:?2 3 4 2$1000
(1 0 2 3 4|:x) -: |:&.(<"_2) x=:a.{~?2 3 4 2 3$#a.
(1 0 2 3 4|:x) -: |:&.(<"_2) x=:adot1{~?2 3 4 2 3$#adot1
(1 0 2 3 4|:x) -: |:&.(<"_2) x=:adot2{~?2 3 4 2 3$#adot2
(1 0 2 3 4|:x) -: |:&.(<"_2) x=:sdot0{~?2 3 4 2 3$#sdot0

(i.0 13 7) -: |:"2 i.0 7 13
(i.a,0)    -: |:   i.0,a=:<._1+2^31
(i.0,a)    -: |:   i.a,0

'domain error' -: 'abc'   |: etx i.3 4
'domain error' -: (<'abc')|: etx i.3 4
'domain error' -: (u:'abc')   |: etx i.3 4
'domain error' -: (<u:'abc')|: etx i.3 4
'domain error' -: (10&u:'abc')   |: etx i.3 4
'domain error' -: (<10&u:'abc')|: etx i.3 4
'domain error' -: (s:@<"0 'abc')   |: etx i.3 4
'domain error' -: (s:@<"0&.> <'abc')|: etx i.3 4
'domain error' -: (<"0@s: <'abc')|: etx i.3 4
'domain error' -: 0.5     |: etx i.3 4
'domain error' -: (<0.5)  |: etx i.3 4
'domain error' -: 3j4     |: etx i.3 4
'domain error' -: (<3j4)  |: etx i.3 4
'domain error' -: (<<0)   |: etx i.3 4
'domain error' -: (0;<<1) |: etx i.3 4
'domain error' -: (0;'a') |: etx i.3 4
'domain error' -: (0;u:'a') |: etx i.3 4
'domain error' -: (0;10&u:'a') |: etx i.3 4
'domain error' -: (0;s:@<"0 'a') |: etx i.3 4

'index error'  -: 0 0     |: etx i.3 4
'index error'  -: (<0 0)  |: etx i.3 4
'index error'  -: 2       |: etx i.3 4
'index error'  -: (<2 2)  |: etx i.3 4
'index error'  -: (i.2 3) |: etx i.3 4


NB. |:"r ----------------------------------------------------------------

(2 1  |:x) -: |:"2 x=:?4 3 2  $100
(_1 _2|:x) -: |:"2 x=:?2 3 4 5$100

(|:"2 -: cant1"2) ?2 3 4$2 
(|:"2 -: cant1"2) a.{~?2 3 4$#a.
(|:"2 -: cant1"2) adot1{~?2 3 4$#adot1
(|:"2 -: cant1"2) adot2{~?2 3 4$#adot2
(|:"2 -: cant1"2) sdot0{~?2 3 4$#sdot0
(|:"2 -: cant1"2) ?2 3 4$1000 
(|:"2 -: cant1"2) o.?2 3 4$1000 
(|:"2 -: cant1"2) r.?2 3 4$1000 
(|:"2 -: cant1"2) x{~?2 3 4$#x=:(+/i.2 3);;:'Cogito, ergo sum.'
(|:"2 -: cant1"2) x{~?2 3 4$#x=:(+/i.2 3);(u:&.>) ;:'Cogito, ergo sum.'
(|:"2 -: cant1"2) x{~?2 3 4$#x=:(+/i.2 3);(10&u:&.>) ;:'Cogito, ergo sum.'
(|:"2 -: cant1"2) x{~?2 3 4$#x=:(+/i.2 3);s:@<"0&.> ;:'Cogito, ergo sum.'
(|:"2 -: cant1"2) x{~?2 3 4$#x=:(+/i.2 3);<"0@s: ;:'Cogito, ergo sum.'

1 0   (|:"2 -: cant2"2) ?3 4 5$1000
1 0   (|:"3 -: cant2"3) ?2 3 4 5$1000
1 2 0 (|:"3 -: cant2"3) ?2 3 4 5$1000

(1 0,:0 1) (|:"1 2 -: cant2"1 2) a.{~?2 3 4$#a.
(1 0,:0 1) (|:"1 2 -: cant2"1 2) a.{~?2 3 4 5$#a.
(1 0,:0 2) (|:"1 3 -: cant2"1 3) a.{~?2 3 4 5$#a.
(1 0,:0 1) (|:"1 2 -: cant2"1 2) adot1{~?2 3 4$#adot1
(1 0,:0 1) (|:"1 2 -: cant2"1 2) adot1{~?2 3 4 5$#adot1
(1 0,:0 2) (|:"1 3 -: cant2"1 3) adot1{~?2 3 4 5$#adot1
(1 0,:0 1) (|:"1 2 -: cant2"1 2) adot2{~?2 3 4$#adot2
(1 0,:0 1) (|:"1 2 -: cant2"1 2) adot2{~?2 3 4 5$#adot2
(1 0,:0 2) (|:"1 3 -: cant2"1 3) adot2{~?2 3 4 5$#adot2
(1 0,:0 1) (|:"1 2 -: cant2"1 2) sdot0{~?2 3 4$#sdot0
(1 0,:0 1) (|:"1 2 -: cant2"1 2) sdot0{~?2 3 4 5$#sdot0
(1 0,:0 2) (|:"1 3 -: cant2"1 3) sdot0{~?2 3 4 5$#sdot0

3 : 0 ''
if. -. IF64 do.
 assert. (-: 1 0 2&|:) 1 1 2 $ 10 u: 1110024676 _539451   NB. verify float not used for moving 2 C4Ts
end.
1
)

NB. magic cubes due to Professor James G. Mauldon

magic=: 4 : 0
 assert. (,3)=$x
 assert. x=<.x
 assert. 0=#$y
 assert. y=<.y
 s=. y
 m=. 3 3{.3 4$x
 s #. s | +/ .*&m (#: i.) 3#s
)

magicprop=: 3 : 0
 s=. +/ (<0 1 2) |: y
 z=.     *./, s = +/y
 z=. z , *./, s = +/"1 y
 z=. z , *./, s = +/"2 (0 1;0 2;1 2)|:"0 _ y
)

vfy=: 4 : 0
 m=. x magic y
 (1 1 1 -: magicprop m) >: *./1=y+.x,+/x
)

3 4 5 vfy 1
3 4 5 vfy"1 0 >: i.5 10
3 4 5 vfy"1 0 p: i.4 5

2 4 6 vfy 1
2 4 6 vfy"1 0 >: i.5 10
2 4 6 vfy"1 0 p: i.4 5

(x=: >:3?20) vfy"1 0 y=: >:?4 5$80 


NB. |: inverse ----------------------------------------------------------

vfy=: 4 : 'y -: x&|:^:_1 x|:y' " 1 _

x=: ?2 3 5 7 11 13$1000
((23$0) ? #$x) vfy x
((23$1) ? #$x) vfy x
((23$2) ? #$x) vfy x
((23$3) ? #$x) vfy x
((23$4) ? #$x) vfy x
((23$5) ? #$x) vfy x
((23$6) ? #$x) vfy x


NB. |: symmetric array  -------------------------------------------------

NB. two transposes suffice to test for symmetry
NB. 0. (p|:q|:X) -: (p{q)|:X
NB. 1. Therefore, the following are equivalent:
NB.       P            (] -: |:)"1 _ X 
NB.       (subgroup P) (] -: |:)"1 _ X
NB. 2. The two permutations 0&C. and _2&C. generate the whole group

perm=: i.@! A. i.

x=: (4$8)$0
x=: (?1e6) (<"1 (perm #$x){?$x)}x
x=: (?1e6) (<"1 (perm #$x){?$x)}x
x=: (?1e6) (<"1 (perm #$x){?$x)}x

sym0=: [: *./ perm@#@$ (] -: |:)"1 _ ]
sym1=: (-: 0&|:) *. (-: _2&|:)

(sym0 -: sym1) x


randfini''


epilog''

