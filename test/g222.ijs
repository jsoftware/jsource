prolog './g222.ijs'
NB. ~:y -----------------------------------------------------------------

randuni''

(,1) -: ~:0
(,1) -: ~:'a'
(,1) -: ~:u:'a'
(,1) -: ~:10&u:'a'
(,1) -: ~:s:@<"0 'a'
(,1) -: ~:243
(,1) -: ~:2.71828
(,1) -: ~:3j4.54e2
(,1) -: ~:<i.12

(,1) -: ~:,1
(,1) -: ~:1 3 4$a.
(,1) -: ~:1 3 4$adot1
(,1) -: ~:1 3 4$adot2
(,1) -: ~:1 3 4$sdot0
(,1) -: ~:i.1 2 3 4
(,1) -: ~:o.i.1 2 3
(,1) -: ~:r.i.1 0 3
(,1) -: ~:1 3 2$;:'Cogito, ergo sum.'
(,1) -: ~:1 3 2$(u:&.>) ;:'Cogito, ergo sum.'
(,1) -: ~:1 3 2$(10&u:&.>) ;:'Cogito, ergo sum.'
(,1) -: ~:1 3 2$s:@<"0&.> ;:'Cogito, ergo sum.'
(,1) -: ~:1 3 2$<"0@s: ;:'Cogito, ergo sum.'

''   -: ~:0$0
''   -: ~:0 3 4$a.
''   -: ~:0 3 4$adot1
''   -: ~:0 3 4$adot2
''   -: ~:0 3 4$sdot0
''   -: ~:i.0 2 3 4
''   -: ~:o.i.0 2 3
''   -: ~:^0j1*i.0 2 0 3
''   -: ~:0 3 2$;:'Cogito, ergo sum.'
''   -: ~:0 3 2$(u:&.>) ;:'Cogito, ergo sum.'
''   -: ~:0 3 2$(10&u:&.>) ;:'Cogito, ergo sum.'
''   -: ~:0 3 2$s:@<"0&.> ;:'Cogito, ergo sum.'
''   -: ~:0 3 2$<"0@s: ;:'Cogito, ergo sum.'

(5{.1) -: ~:5#1
(5{.1) -: ~:5#1 3 4$a.
(5{.1) -: ~:5#1 3 4$adot1
(5{.1) -: ~:5#1 3 4$adot2
(5{.1) -: ~:5#1 3 4$sdot0
(5{.1) -: ~:5#i.1 2 3 4
(5{.1) -: ~:5#o.i.1 2 3
(5{.1) -: ~:5#^0j1*i.1 0 3
(5{.1) -: ~:5#1 3 2$;:'Cogito, ergo sum.'
(5{.1) -: ~:5#1 3 2$(u:&.>) ;:'Cogito, ergo sum.'
(5{.1) -: ~:5#1 3 2$(10&u:&.>) ;:'Cogito, ergo sum.'
(5{.1) -: ~:5#1 3 2$s:@<"0&.> ;:'Cogito, ergo sum.'
(5{.1) -: ~:5#1 3 2$<"0@s: ;:'Cogito, ergo sum.'

test =: 3 : '(1=type b), (($b)-:,#y), b-:(i.#y)=i.~y [ b=.~:y' 

test ?2
test a.{~?#a.
test adot1{~?#adot1
test adot2{~?#adot2
test sdot0{~?#sdot0
test ?2023
test o.?2023
test j./?2$3324
test ('foo';'upon';'thee'){~?3
test (u:&.>'foo';'upon';'thee'){~?3
test (10&u:&.>'foo';'upon';'thee'){~?3
test (s:@<"0&.> 'foo';'upon';'thee'){~?3
test (<"0@s: 'foo';'upon';'thee'){~?3

test ?20 2$2
test a.{~50+?40 2$3
test ?20 2$3
test o.?20 2$3
test j./?2 82 2$3
test ('foo';'upon';'thee'){~?20 2$3
test (u:&.>'foo';'upon';'thee'){~?20 2$3
test (10&u:&.>'foo';'upon';'thee'){~?20 2$3
test (s:@<"0&.> 'foo';'upon';'thee'){~?20 2$3
test (<"0@s: 'foo';'upon';'thee'){~?20 2$3

test=: 3 : 0  NB. ~: without rank
 yy=: y
 b=: ~: yy
 assert. 1=type b
 assert. 1=#$b
 assert. b -:&# yy
 assert. b -: (i.@# = i.~) yy
 assert. (b#yy) -: ~.yy
 1
)

test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 0 1
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u:'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u:'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:@<"0 'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: _10+?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$2e9
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: o.?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: j./?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (10&u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: s:@<"0&.> ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: <"0@s: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: x: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: %/0 1+x:?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u: ?5$65536
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u: RAND32 ?5$C4MAX
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: u: 128+a.i. ' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: 10&u: 65536+a.i. ' miasma chthonic chronic kakistocracy dado'

test=: 3 : 0   NB. ~:"_1
 yy=: y
 b=: ~:"_1 yy
 assert. 1=type b
 assert. (<:#$yy)=#$b
 assert. (1{$b) = 1{$yy
 assert. b -: (i.@# = i.~)"_1 yy
 assert. ((<"1 b)#&.><"_1 yy) -: <@~."_1 yy
 1
)

test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 0 1
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 'abcde'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u:'abcde'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u:'abcde'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:@<"0 'abcde'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$20
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: _10+?5$20
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$2e9
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: o.?5$20
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: j./?2 5$20
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (10&u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: s:@<"0&.> ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: <"0@s: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: x: ?5$20
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: %/0 1+x:?2 5$20
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u: ?5$65536
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u: RAND32 ?5$C4MAX
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: u: 128+a.i. ' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 3 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: 10&u: 65536+a.i. ' miasma chthonic chronic kakistocracy dado'


NB. ~:"r y --------------------------------------------------------------

g =: 3 : '~:y'

(g"0  -: ~:"0 ) y=:?2 3 4 5$2
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:'ab'{~?2 3 4 5$2
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:(u:'ab'){~?2 3 4 5$2
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:(10&u:'ab'){~?2 3 4 5$2
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:?2 3 4 5$3
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:o.?5 4 3 2$2
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:r.?5 4 3 2$2
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:(?5 4 3 2$#x){x=:;:'kakistocracy piscatory sequipedalian'
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:(?5 4 3 2$#x){x=:(u:&.>) ;:'kakistocracy piscatory sequipedalian'
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:(?5 4 3 2$#x){x=:(10&u:&.>) ;:'kakistocracy piscatory sequipedalian'
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:(?5 4 3 2$#x){x=:s:@<"0&.> ;:'kakistocracy piscatory sequipedalian'
(g"0  -: ~:"0 ) y=:(?5 4 3 2$#x){x=:<"0@s: ;:'kakistocracy piscatory sequipedalian'
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y
(g"0  -: ~:"0 ) y=:?0 0 0 0$0
(g"1  -: ~:"1 ) y
(g"2  -: ~:"2 ) y
(g"3  -: ~:"3 ) y
(g"4  -: ~:"4 ) y
(g"_1 -: ~:"_1) y

(,:~:y) -: ~:!.0"1 ,: <"1 y=: 13 97 ?@$ 1e6


NB. x~:y ----------------------------------------------------------------

NB. Boolean
0 1 1 0 -: 0 0 1 1~:0 1 0 1

NB. literal
(($t)$0) -: t ~: t=:a.{~?2 3 4$#a.
(($t)$1) -: (?($t)$2)          ~: t=:a.{~?2 3 4$#a.
(($t)$1) -: (_5e8+?($t)$1e9)   ~: t=:a.{~?2 3 4$#a.
(($t)$1) -: (o._5e8+?($t)$1e9) ~: t=:a.{~?2 3 4$#a.
(($t)$1) -: (r._5e7+?($t)$1e8) ~: t=:a.{~?2 3 4$#a.
(($t)$1) -: (($t)$;:'8-+.abc') ~: t=:a.{~?2 3 4$#a.

NB. literal2
(($t)$0) -: t ~: t=:adot1{~?2 3 4$#adot1
(($t)$1) -: (?($t)$2)          ~: t=:adot1{~?2 3 4$#adot1
(($t)$1) -: (_5e8+?($t)$1e9)   ~: t=:adot1{~?2 3 4$#adot1
(($t)$1) -: (o._5e8+?($t)$1e9) ~: t=:adot1{~?2 3 4$#adot1
(($t)$1) -: (r._5e7+?($t)$1e8) ~: t=:adot1{~?2 3 4$#adot1
(($t)$1) -: (($t)$(u:&.>) ;:'8-+.abc') ~: t=:adot1{~?2 3 4$#adot1

NB. literal4
(($t)$0) -: t ~: t=:adot2{~?2 3 4$#adot2
(($t)$1) -: (?($t)$2)          ~: t=:adot2{~?2 3 4$#adot2
(($t)$1) -: (_5e8+?($t)$1e9)   ~: t=:adot2{~?2 3 4$#adot2
(($t)$1) -: (o._5e8+?($t)$1e9) ~: t=:adot2{~?2 3 4$#adot2
(($t)$1) -: (r._5e7+?($t)$1e8) ~: t=:adot2{~?2 3 4$#adot2
(($t)$1) -: (($t)$(10&u:&.>) ;:'8-+.abc') ~: t=:adot2{~?2 3 4$#adot2

NB. symbol
(($t)$0) -: t ~: t=:sdot0{~?2 3 4$#sdot0
(($t)$1) -: (?($t)$2)          ~: t=:sdot0{~?2 3 4$#sdot0
(($t)$1) -: (_5e8+?($t)$1e9)   ~: t=:sdot0{~?2 3 4$#sdot0
(($t)$1) -: (o._5e8+?($t)$1e9) ~: t=:sdot0{~?2 3 4$#sdot0
(($t)$1) -: (r._5e7+?($t)$1e8) ~: t=:sdot0{~?2 3 4$#sdot0
(($t)$1) -: (($t)$s:@<"0&.> ;:'8-+.abc') ~: t=:sdot0{~?2 3 4$#sdot0
(($t)$1) -: (($t)$<"0@s: ;:'8-+.abc') ~: t=:sdot0{~?2 3 4$#sdot0

NB. integer
(($t)$0) -: t ~: t=:_1e9+?2 3 4$2e9
(a~:b)-:0~:a-b     [ a=:_5+?200$10         [ b=:_5+?200$10
(a~:b)-:a~:b{0 1 2 [ a=:(?100$2){0 1 2     [ b=:?100$2
(a~:b)-:a~:}.3.4,b [ a=:?200$10            [ b=:?200$10

NB. real
(($t)$0) -: t ~: t=:o._1e9+?2 3 4$2e9
(a~:b)-:0~:a-b     [ a=:o._5+?200$10       [ b=:o._5+?200$10
(a~:b)-:a~:}.3.4,b [ a=:}.3.4,?100$2       [ b=:?100$2
(a~:b)-:a~:}.3.4,b [ a=:}.3.4,_5+?200$10   [ b=:_5+?200$10
(a~:b)-:a~:}.3j4,b [ a=:o._5+?200$10       [ b=:o._5+?200$10

NB. complex
(($t)$0) -: t ~: t=:j./_1e9+?2 3 4$2e9
(a~:b)-:0~:a-b     [ a=:r._5+?200$10       [ b=:r._5+?200$10
(a~:b)-:a~:}.3j4,b [ a=:}.3j4,?100$2       [ b=:?100$2
(a~:b)-:a~:}.3j4,b [ a=:}.3j4,_5+?200$10   [ b=:_5+?200$10
(a~:b)-:a~:}.3j4,b [ a=:}.3j4,o._5+?200$10 [ b=:o._5+?200$10

NB. boxed
(($t)$0) -: t ~: t=:<"1?2 3 4 5$10
(($t)$1) -: (?($t)$2)          ~: t=:<"1?2 3 4$10
(($t)$1) -: (($t)$'8-+.abc')   ~: t=:<"1?2 3 4$10
(($t)$1) -: (($t)$u:'8-+.abc')   ~: t=:<"1?2 3 4$10
(($t)$1) -: (($t)$10&u:'8-+.abc')   ~: t=:<"1?2 3 4$10
(($t)$1) -: (_5e8+?($t)$1e9)   ~: t=:<"1?2 3 4$10
(($t)$1) -: (o._5e8+?($t)$1e9) ~: t=:<"1?2 3 4$10
(($t)$1) -: (r._5e7+?($t)$1e8) ~: t=:<"1?2 3 4$10

'length error' -: 1 2 ~: etx 'abc'
'length error' -: 1 2 ~: etx u:'abc'
'length error' -: 1 2 ~: etx 10&u:'abc'
'length error' -: 1 2 ~: etx i. 3 4 5
'length error' -: 1 2 ~: etx s:@<"0 'abc'


4!:55 ;:'a adot1 adot2 sdot0 b g t test x y yy'
randfini''


epilog''

