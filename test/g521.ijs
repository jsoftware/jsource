prolog './g521.ijs'
NB. {.y -----------------------------------------------------------------

randuni''

([ -: {.) 0
([ -: {.) 'a'
([ -: {.) u:'a'
([ -: {.) 10&u:'a'
([ -: {.) 3
([ -: {.) 3.4
([ -: {.) 3j4
([ -: {.) <3 4

head =: }.&$ $ ,

NB. Boolean
(head -: {.) 1=?2
(head -: {.) 1=?4$2
(head -: {.) 1=?3 4$2
(head -: {.) 1=?2 3 4$2

NB. literal
(head -: {.) 'a'
(head -: {.) a.{~?4$256
(head -: {.) a.{~?3 4$256
(head -: {.) a.{~?2 3 4$256

NB. literal2
(head -: {.) u:'a'
(head -: {.) adot1{~?4$(#adot1)
(head -: {.) adot1{~?3 4$(#adot1)
(head -: {.) adot1{~?2 3 4$(#adot1)

NB. literal4
(head -: {.) 10&u:'a'
(head -: {.) adot2{~?4$(#adot2)
(head -: {.) adot2{~?3 4$(#adot2)
(head -: {.) adot2{~?2 3 4$(#adot2)

NB. symbol
(head -: {.) s:@<"0 'a'
(head -: {.) sdot0{~?4$(#sdot0)
(head -: {.) sdot0{~?3 4$(#sdot0)
(head -: {.) sdot0{~?2 3 4$(#sdot0)

NB. integer
(head -: {.) 12345
(head -: {.) ?4$123456
(head -: {.) ?3 4$123456
(head -: {.) ?2 3 4$123456

NB. floating point
(head -: {.) 123.45
(head -: {.) o.?4$1236
(head -: {.) o.?3 4$1256
(head -: {.) o.?2 3 4$1456

NB. complex
(head -: {.) 123j45
(head -: {.) ^0j1*?4$1236
(head -: {.) ^0j1*?3 4$1256
(head -: {.) ^0j1*?2 3 4$1456

NB. boxed
t=:(+&.>i.5),;:'(head -: {.) ^0j1*?3 4$1256'
(head -: {.) <123j45
(head -: {.) t{~?4$#t
(head -: {.) t{~?3 4$#t
(head -: {.) t{~?2 3 4$#t

(  3$0  ) -: {. 0   3$1
(2 3$' ') -: {. 0 2 3$'a'
(  3$0  ) -: {. 0   3$12345
(  3$0  ) -: {. 0   3$123.45
(  3$0  ) -: {. 0   3$123j45
(1 3$a: ) -: {. 0 1 3$<45

(2 3$3j4) -: {.!.3j4 i. 0 2 3


NB. {."r y --------------------------------------------------------------

head =: }.&$ $ ,
f0 =: {." 0 -: head" 0
f1 =: {." 1 -: head" 1
f2 =: {." 2 -: head" 2
g1 =: {."_1 -: head"_1
g2 =: {."_2 -: head"_2

f1 ?3 4  $2
f1 ?3 4 5$2
f2 ?3 4  $2
f2 ?3 4 5$2
g1 ?4 5  $2
g1 ?4 5 6$2
g2 ?4 5  $2
g2 ?4 5 6$2

f1 (?3 4  $#x ){x=:'boustrophedonic'
f1 (?3 4 5$#a.){a.
f2 (?3 4  $#a.){a.
f2 (?3 4 5$#a.){a.
g1 (?3 4  $#x ){x=:'boustrophedonic'
g1 (?3 4 5$#a.){a.
g2 (?3 4  $#a.){a.
g2 (?3 4 5$#a.){a.

f1 (?3 4  $#x ){x=:u:'boustrophedonic'
f1 (?3 4 5$#adot1){adot1
f2 (?3 4  $#adot1){adot1
f2 (?3 4 5$#adot1){adot1
g1 (?3 4  $#x ){x=:'boustrophedonic'
g1 (?3 4 5$#adot1){adot1
g2 (?3 4  $#adot1){adot1
g2 (?3 4 5$#adot1){adot1

f1 (?3 4  $#x ){x=:10&u:'boustrophedonic'
f1 (?3 4 5$#adot2){adot2
f2 (?3 4  $#adot2){adot2
f2 (?3 4 5$#adot2){adot2
g1 (?3 4  $#x ){x=:10&u:'boustrophedonic'
g1 (?3 4 5$#adot2){adot2
g2 (?3 4  $#adot2){adot2
g2 (?3 4 5$#adot1){adot2

f1 (?3 4  $#x ){x=:s:@<"0 'boustrophedonic'
f1 (?3 4 5$#sdot0){sdot0
f2 (?3 4  $#sdot0){sdot0
f2 (?3 4 5$#sdot0){sdot0
g1 (?3 4  $#x ){x=:s:@<"0 'boustrophedonic'
g1 (?3 4 5$#sdot0){sdot0
g2 (?3 4  $#sdot0){sdot0
g2 (?3 4 5$#sdot0){sdot0

f1 _1e5+?3 4  $2e5
f1 _1e5+?3 4 5$2e5
f2 _1e5+?3 4  $2e5
f2 _1e5+?3 4 5$2e5
g1 _1e5+?4 5  $2e5
g1 _1e5+?4 5 6$2e5
g2 _1e5+?4 5  $2e5
g2 _1e5+?4 5 6$2e5

f1 o._1e5+?3 4  $2e5
f1 o._1e5+?3 4 5$2e5
f2 o._1e5+?3 4  $2e5
f2 o._1e5+?3 4 5$2e5
g1 o._1e5+?4 5  $2e5
g1 o._1e5+?4 5 6$2e5
g2 o._1e5+?4 5  $2e5
g2 o._1e5+?4 5 6$2e5

f1 r._1e5+?3 4  $2e5
f1 r._1e5+?3 4 5$2e5
f2 r._1e5+?3 4  $2e5
f2 r._1e5+?3 4 5$2e5
g1 r._1e5+?4 5  $2e5
g1 r._1e5+?4 5 6$2e5
g2 r._1e5+?4 5  $2e5
g2 r._1e5+?4 5 6$2e5

f1 (?31 4  $#x){x=:;:'super cali fragi listic'
f1 (?31 4  $#x){x=:(u:&.>) ;:'super cali fragi listic'
f1 (?31 4  $#x){x=:(10&u:&.>) ;:'super cali fragi listic'
f1 (?31 4  $#x){x=:s:@<"0&.> ;:'super cali fragi listic'
f1 (?31 4  $#x){x=:<"0@s: ;:'super cali fragi listic'
f1 (?31 4 5$#x){x=:+&.>i.100
f2 (?31 4  $#x){x=:(;:'Cogito, ergo sum.'),+&.>i.12
f2 (?31 4  $#x){x=:((u:&.>) ;:'Cogito, ergo sum.'),+&.>i.12
f2 (?31 4  $#x){x=:((10&u:&.>) ;:'Cogito, ergo sum.'),+&.>i.12
f2 (?31 4  $#x){x=:(s:@<"0&.> ;:'Cogito, ergo sum.'),+&.>i.12
f2 (?31 4  $#x){x=:(<"0@s: ;:'Cogito, ergo sum.'),+&.>i.12
f2 (?31 4 5$#x){x=:(<<'opposable thumbs'),+&.>i.12
f2 (?31 4 5$#x){x=:(<<u:'opposable thumbs'),+&.>i.12
f2 (?31 4 5$#x){x=:(<<10&u:'opposable thumbs'),+&.>i.12
f2 (?31 4 5$#x){x=:(<s:@<"0&.> <'opposable thumbs'),+&.>i.12
f2 (?31 4 5$#x){x=:(<<"0@s: <'opposable thumbs'),+&.>i.12
g1 (?31 4  $#x){x=:<"0 a.
g1 (?31 4  $#x){x=:<"0 adot1
g1 (?31 4  $#x){x=:<"0 adot2
g1 (?31 4  $#x){x=:<"0 sdot0
g1 (?31 4 5$#x){x=:(i.12){.&.>123
g2 (?31 4  $#x){x=:(<'junkfoo'),(i.12){.&.>3j4
g2 (?31 4  $#x){x=:(<u:'junkfoo'),(i.12){.&.>3j4
g2 (?31 4  $#x){x=:(<10&u:'junkfoo'),(i.12){.&.>3j4
g2 (?31 4  $#x){x=:(<s:@<"0&.> <'junkfoo'),(i.12){.&.>3j4
g2 (?31 4  $#x){x=:(<<"0@s: <'junkfoo'),(i.12){.&.>3j4
g2 (?31 4 5$#x){x=:5!:1&.<'g2'
g2 (?31 4 5$#x){x=:5!:1&.<u:'g2'
g2 (?31 4 5$#x){x=:5!:1&.<10&u:'g2'

f =: {."0 -: head"0
f ?4 5$2
f 2 3 4$'supercalifragilisticespialidocious'
f 2 3 4$u:'supercalifragilisticespialidocious'
f 2 3 4$10&u:'supercalifragilisticespialidocious'
f 2 3 4$s:@<"0 'supercalifragilisticespialidocious'
f ?2 3 4$1000
f o.?100$1000
f j./_500+?2 12$1000
f >5!:1<'f'


NB. x{.y ----------------------------------------------------------------

(x,20$0  ) -: 23{.x=:?3$2
(x,20$' ') -: 23{.x=:'fot'
(x,20$0  ) -: 23{.x=:?3$10000
(x,20$0  ) -: 23{.x=:o.?3$10000
(x,20$0  ) -: 23{.x=:j./?2 3$10000
(x,20$<$0) -: 23{.x=:;:'Cogito, ergo'
(x,20$<$0) -: 23{.x=:(u:&.>) ;:'Cogito, ergo'
(x,20$<$0) -: 23{.x=:(10&u:&.>) ;:'Cogito, ergo'
(x,20$<$0) -: 23{.x=:s:@<"0&.> ;:'Cogito, ergo'
(x,20$<$0) -: 23{.x=:<"0@s: ;:'Cogito, ergo'

mt    =: 0&e.@$
fill  =: > @ ({&(' ';(u:' ');(10&u:' ');({.s:'');a:;0)) @ (2 131072 262144 65536 32&i.) @ (3!:0)
pad   =: fill@] $~ (|@[ - #@]) 0} $@]
ti    =: i.@-@[ + [ + #@]
case  =: 0&<:@[ #.@, |@[ > #@]
itake =: (ti{])`(],~pad)`(i.@[{])`(],pad) @. case
taker =: 4 :  '({.x) itake"({:x) y'
raise =: (1"0@[ $ ])`]@.(*@#@$@])
larg  =: <@,"(0) _&(0})@:-@i.@#
targ  =: larg@[,<@raise
take  =: >@(taker&.>/)@targ " 1 _

f =: {. -: take

3     f 1
_3    f 1
0     f 5
3 4   f 'a'
_3 4  f 'a'
3 _4  f <'foo'
_3 _4 f <'foo'
3 4   f u:'a'
_3 4  f u:'a'
3 _4  f <u:'foo'
_3 _4 f <u:'foo'
3 4   f 10&u:'a'
_3 4  f 10&u:'a'
3 _4  f <10&u:'foo'
_3 _4 f <10&u:'foo'
3 4   f s:@<"0 'a'
_3 4  f s:@<"0 'a'
3 _4  f s:@<"0&.> <'foo'
3 _4  f <"0@s: <'foo'
_3 _4 f s:@<"0&.> <'foo'
_3 _4 f <"0@s: <'foo'

2   3  4 f 3j4
2   3 _4 f 3.4
2  _3  4 f 324
2  _3 _4 f '3'
2  _3 _4 f u:'3'
2  _3 _4 f 10&u:'3'
2  _3 _4 f s:@<"0 '3'
_2  3  4 f 0
_2  3 _4 f _24
_2 _3  4 f _1.23e_34j_5.67e_28
_2 _3 _4 f <7

3  f ?12$100
_3 f ?12$123
3  f 'abafasfkjsadf'
_3 f 'abasdfasdfasf'
3  f u:'abafasfkjsadf'
_3 f u:'abasdfasdfasf'
3  f 10&u:'abafasfkjsadf'
_3 f 10&u:'abasdfasdfasf'
3  f s:@<"0 'abafasfkjsadf'
_3 f s:@<"0 'abasdfasdfasf'
3  f +&.>i.12
_3 f +&.>i.12

3     f ?5 6 4$100
_3    f ?5 6 4$100
5     f ?5 6 4$100
3 4   f ?5 6 4$100
3 6   f ?5 6 4$100
3 _6  f ?5 6 4$100
_3 _6 f ?5 6 4$100
5 6   f ?5 6 4$100
5 6 4 f ?5 6 4$100
1 _4  f +&.>?6 7$100

3 f ''
3 f i.0
3 f 0$3.4
3 f 0$3j4
3 f 0$12x
3 f 0$3r4
3 f 0$<'abc'
3 f 0$<u:'abc'
3 f 0$<10&u:'abc'
3 f 0$s:@<"0&.> <'abc'
3 f 0$<"0@s: <'abc'
3 f 0 0$<''
3 f 0 0$<u:''
3 f 0 0$<10&u:''
3 f 0 0$<s:''
3 4 f i.0 0
3 4 f 4 0 3$' '
3 4 f 4 0 3$u:' '
3 4 f 4 0 3$10&u:' '
3 4 f 4 0 3$s:@<"0 ' '
_3 4 f 0 0 5$<''
_3 4 f 0 0 5$<u:''
_3 4 f 0 0 5$<10&u:''
_3 4 f 0 0 5$<s:''

'domain error' -: 'abc'     {. etx i.2 3 4
'domain error' -: (u:'abc')     {. etx i.2 3 4
'domain error' -: (10&u:'abc')     {. etx i.2 3 4
'domain error' -: (s:@<"0 'abc')     {. etx i.2 3 4
'domain error' -: 3.4 5     {. etx i.2 3 4
'domain error' -: 3j4       {. etx i.2 3 4
'domain error' -: (3;4)     {. etx i.2 3 4

'length error' -: 3 4 5     {. etx 1 2
'length error' -: 3 4 5     {. etx i.1 2


NB. x{."r y -------------------------------------------------------------

(i.4 0)   -: 0{."1 i.4 5
(i.4 5 0) -: 0{."1 i.4 5 7
(i.4 0 7) -: 0{."2 i.4 5 7

((i.n){"1 x) -: n{."1 x=:?  11 13$1e9 [ n=:?13
((i.n){"1 x) -: n{."1 x=:?5 11 13$1e9 [ n=:?13

x =: o.?3 5 7$1e9
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:0 0
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:0 2
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:3 0
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:3 2
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:?}.$x
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:?}.$x
((i.1{n){"1 (i.0{n){"2 x) -: n{."2 x [ n=:?}.$x

([\x) -: (>:i.#x){."0 1 x=:'abcdefghij'
([\x) -: (>:i.#x){."0 1 x=:u:'abcdefghij'
([\x) -: (>:i.#x){."0 1 x=:10&u:'abcdefghij'
([\x) -: (>:i.#x){."0 1 x=:s:@<"0 'abcdefghij'

f =: 4 : 'x{.y'
3 4 5 ({."1 0 -: f"1 0) i.6 7
3 4 5 ({."1 0 -: f"1 0) i.0 7
3 0 5 ({."1 0 -: f"1 0) i.6 7
3 0 5 ({."1 0 -: f"1 0) i.0 7


NB. x{.!.f y ------------------------------------------------------------

(4 5$0)   -: 4 5{.     0
(4 5$9)   -: 4 5{.!.9  [ 9
(4 5$9)   -: 4 5{.!.9  [ 0{9 0.1
(4 5$9)   -: 4 5{.!.9  [ 0{9 0j1

(7 3$(,i.2 3),15$o.1)  -: 7{.!.(o.1) i.2 3
(7 3$(,o.i.2 3),15$_9) -: 7{.!._9 o.i.2 3

'domain error' -: 5 0 {.!.'a'    etx i. 4 3
'domain error' -: 5 0 {.!.(<9)   etx i.5 4 3
'domain error' -: 4 5{.!.'a'     etx 0
'domain error' -: 4 5{.!.'a'     etx i.3 4
'domain error' -: 4 5{.!.'a'     etx o.i.2 8
'domain error' -: 4 5{.!.'a'     etx r.i.2 8
'domain error' -: 4 5{.!.'a'     etx +&.>i.3 4

'domain error' -: 45 {.!.0       etx 'abc'
'domain error' -: 45 {.!.1       etx +&.>i.3 4
'domain error' -: 45 {.!.5       etx 'amanuensis'
'domain error' -: 45 {.!._6      etx +&.>i.3 4
'domain error' -: 45 {.!.3.5     etx 'abc'
'domain error' -: 45 {.!._6.89   etx +&.>i.3 4
'domain error' -: 45 {.!.3j5     etx 'abc'
'domain error' -: 45 {.!._6j89   etx +&.>i.3 4

'domain error' -: 4 5{.!.(<3 4)  etx 0
'domain error' -: 4 5{.!.(<3 4)  etx 2 3$'fourscore and seven years ago'
'domain error' -: 4 5{.!.(<'a')  etx i.3 4
'domain error' -: 4 5{.!.(<34)   etx o.i.2 8
'domain error' -: 4 5{.!.(<'a')  etx r.i.2 8

'rank error'   -: ex '{.!.($0)'
'rank error'   -: ex '{.!.(,0)'


NB. fills ---------------------------------------------------------------

ec  =: ''
en  =: $0
eb  =: 0$<0
jot =: <''

'   ' -: 3{.ec
0 0 0 -: 3{.en
(3$a:)-: 3{.eb

((s,_3{.t)$' ') -:   s {.(t=:2|8?8)$' ' [ s=:1+?5$2
((s,_3{.t)$u:' ') -:   s {.(t=:2|8?8)$u:' ' [ s=:1+?5$2
((s,_3{.t)$10&u:' ') -:   s {.(t=:2|8?8)$10&u:' ' [ s=:1+?5$2
((s,_3{.t)${.s:'') -:   s {.(t=:2|8?8)$(s:@<"0 ' ') [ s=:1+?5$2
((s,_3{.t)$0  ) -:   s {.(t=:2|8?8)$0   [ s=:1+?5$2
((s,_3{.t)$a: ) -:   s {.(t=:2|8?8)$a:  [ s=:1+?5$2

((s,_3{.t)$' ') -: (-s){.(t=:2|8?8)$' ' [ s=:1+?5$2
((s,_3{.t)$u:' ') -: (-s){.(t=:2|8?8)$u:' ' [ s=:1+?5$2
((s,_3{.t)$10&u:' ') -: (-s){.(t=:2|8?8)$10&u:' ' [ s=:1+?5$2
((s,_3{.t)${.s:'') -: (-s){.(t=:2|8?8)$s:@<"0 ' ' [ s=:1+?5$2
((s,_3{.t)$0  ) -: (-s){.(t=:2|8?8)$0   [ s=:1+?5$2
((s,_3{.t)$a: ) -: (-s){.(t=:2|8?8)$a:  [ s=:1+?5$2

s -: $  s {.' ' [ s=:?8$2
s -: $  s {.(u:' ') [ s=:?8$2
s -: $  s {.(10&u:' ') [ s=:?8$2
s -: $  s {.({.s:'') [ s=:?8$2
s -: $  s {.0   [ s=:?8$2
s -: $  s {.a:  [ s=:?8$2

s -: $(-s){.' ' [ s=:?8$2
s -: $(-s){.(u:' ') [ s=:?8$2
s -: $(-s){.(10&u:' ') [ s=:?8$2
s -: $(-s){.({.s:'') [ s=:?8$2
s -: $(-s){.0   [ s=:?8$2
s -: $(-s){.a:  [ s=:?8$2

a=:1 3$'a'
(a,' ') -: a,ec
(a,' ') -: a,en
(a,' ') -: a,eb
(' ',a) -: ec,a
(' ',a) -: en,a
(' ',a) -: eb,a

a=:1 3$u:'a'
(a,' ') -: a,ec
(a,' ') -: a,en
(a,' ') -: a,eb
(' ',a) -: ec,a
(' ',a) -: en,a
(' ',a) -: eb,a

a=:1 3$10&u:'a'
(a,' ') -: a,ec
(a,' ') -: a,en
(a,' ') -: a,eb
(' ',a) -: ec,a
(' ',a) -: en,a
(' ',a) -: eb,a

a=:1 3$s:@<"0 'a'
(a,{.s:'') -: a,ec
(a,{.s:'') -: a,en
(a,{.s:'') -: a,eb
(({.s:''),a) -: ec,a
(({.s:''),a) -: en,a
(({.s:''),a) -: eb,a

a=:i.1 3
(a,0) -: a,ec
(a,0) -: a,en
(a,0) -: a,eb
(0,a) -: ec,a
(0,a) -: en,a
(0,a) -: eb,a

a=:,:1;2;'abc'
(a,jot) -: a,ec
(a,jot) -: a,en
(a,jot) -: a,eb
(jot,a) -: ec,a
(jot,a) -: en,a
(jot,a) -: eb,a

a=:,:1;2;u:'abc'
(a,jot) -: a,ec
(a,jot) -: a,en
(a,jot) -: a,eb
(jot,a) -: ec,a
(jot,a) -: en,a
(jot,a) -: eb,a

a=:,:1;2;10&u:'abc'
(a,jot) -: a,ec
(a,jot) -: a,en
(a,jot) -: a,eb
(jot,a) -: ec,a
(jot,a) -: en,a
(jot,a) -: eb,a

a=:,:1;2;s:@<"0 'abc'
(a,jot) -: a,ec
(a,jot) -: a,en
(a,jot) -: a,eb
(jot,a) -: ec,a
(jot,a) -: en,a
(jot,a) -: eb,a

(0 0 1 0{' ',:a) -: >ec;en;a;eb [ a=:'abc'
(0 0 1 0{(u:' '),:a) -: >ec;en;a;eb [ a=:u:'abc'
(0 0 1 0{(10&u:' '),:a) -: >ec;en;a;eb [ a=:10&u:'abc'
(0 0 1 0{({.s:''),:a) -: >ec;en;a;eb [ a=:s:@<"0 'abc'
(0 0 1 0{0  ,:a) -: >ec;en;a;eb [ a=:3 4 5
(0 0 1 0{jot,:a) -: >ec;en;a;eb [ a=:4;5;i.3 4


NB. x{.y with infinite left arguments -----------------------------------

(_   3{.x) -: (({.$x),3){.x=: ?(?10 20)$1000
(3   _{.x) -: (3,{:$x  ){.x=: ?(?10 20)$1000
(_    {.x) -: x
(_   _{.x) -: x
(2   _{.x) -: 2{.x

(__  3{.x) -: (({.$x),3){.x=: ?(?10 20)$1000
(3  __{.x) -: (3,{:$x  ){.x=: ?(?10 20)$1000
(__   {.x) -: x
(__ __{.x) -: x
(2  __{.x) -: 2{.x

(_   2{."2 x) -: ((1{$x),2){."2 x=: ?(?10 20 30)$1000
(2   _{."2 x) -: (2,2{$x  ){."2 x=: ?(?10 20 30)$1000

(__  2{."2 x) -: ((1{$x),2){."2 x=: ?(?10 20 30)$1000
(2  __{."2 x) -: (2,2{$x  ){."2 x=: ?(?10 20 30)$1000

(_  2 3  _{. 12) -: 1 2 3 1{.12
(__ 2 3 __{. 12) -: 1 2 3 1{.12


'domain error' -: 2.5 _ {. etx i.3 4

('domain error';'limit error') e.~ < (>IF64{(_,_1+2^31);_,_1+2^63){. etx i.3 4

NB. inplaceable x}.y x{.y ------------------------------------------------------------------------

a =. i. 1e6
4000 > 7!:2 '+/ 1e5 {. a'
4000 > 7!:2 '+/ 1000 {. a'
4000 > 7!:2 '+/ 1e5 }. a'
4000 > 7!:2 '+/ 1000 }. a'

20 -: #@, {. >: i. 10 10 2   NB. self-virtual used to mess up atom count


4!:55 ;:'a adot1 adot2 sdot0 case eb ec en f f0 f1 f2 fill '
4!:55 ;:'g1 g2 head itake jot larg mt n pad raise '
4!:55 ;:'s t take taker targ ti x '
4!:55 ;: 'a'
randfini''


epilog''

