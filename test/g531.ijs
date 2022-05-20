prolog './g531.ijs'
NB. }.y -----------------------------------------------------------------

randuni''

behead =: 1&}.

NB. Boolean
(behead -: }.) 1=?2
(behead -: }.) 1=?4$2
(behead -: }.) 1=?3 4$2
(behead -: }.) 1=?2 3 4$2
(behead -: }.) 1=?0 3$2

NB. literal
(behead -: }.) 'a'
(behead -: }.) a.{~?4$256
(behead -: }.) a.{~?3 4$256
(behead -: }.) a.{~?2 3 4$256
(behead -: }.) 1=?0 3$256

NB. literal
(behead -: }.) u:'a'
(behead -: }.) adot1{~?4$(#adot1)
(behead -: }.) adot1{~?3 4$(#adot1)
(behead -: }.) adot1{~?2 3 4$(#adot1)
(behead -: }.) 1=?0 3$256

NB. literal4
(behead -: }.) 10&u:'a'
(behead -: }.) adot2{~?4$(#adot2)
(behead -: }.) adot2{~?3 4$(#adot2)
(behead -: }.) adot2{~?2 3 4$(#adot2)
(behead -: }.) 1=?0 3$256

NB. symbol
(behead -: }.) s:@<"0 'a'
(behead -: }.) sdot0{~?4$(#sdot0)
(behead -: }.) sdot0{~?3 4$(#sdot0)
(behead -: }.) sdot0{~?2 3 4$(#sdot0)
(behead -: }.) 1=?0 3$256

NB. integer
(behead -: }.) 12345
(behead -: }.) ?4$123456
(behead -: }.) ?3 4$123456
(behead -: }.) ?2 3 4$123456
(behead -: }.) ?0 3$123456

NB. (behead -: }.)loating point
(behead -: }.) 123.45
(behead -: }.) o.?4$1236
(behead -: }.) o.?3 4$1256
(behead -: }.) o.?2 3 4$1456
(behead -: }.) 0 3$123.456

NB. complex
(behead -: }.) 123j45
(behead -: }.) ^0j1*?4$1236
(behead -: }.) ^0j1*?3 4$1256
(behead -: }.) ^0j1*?2 3 4$1456
(behead -: }.) 0 3$123j56

NB. boxed
t=:(+&.>i.5),;:'(raze a) -: }. a=: ^0j1*?3 4$1256'
(behead -: }.) <123j45
(behead -: }.) t{~?4$#t
(behead -: }.) t{~?3 4$#t
(behead -: }.) t{~?2 3 4$#t
(behead -: }.) 0 3$<123456


NB. x}.y ----------------------------------------------------------------

pi   =: 0&< @[ * 0&<.@-
ni   =: 0&>:@[ * 0&>.@+
di   =: ({.~ #@$) (pi + ni) $@]
drop =: (di {. ])"1 _

m=:?4 5 6$100
0  (}. -: drop) m
1  (}. -: drop) m
_1 (}. -: drop) m
2 0 _1 (}. -: drop) m
0 0 9  (}. -: drop) m
_9 _9 _9 (}. -: drop) m

test =: }. -: [ }. 0&<:@i.@#@[ $  ]

1 2 3 test 4
1 0 3 test 4
1 2 0 test 4
0 0 0 test 4

1 2 3 test 'a'
1 0 3 test 'a'
1 2 0 test 'a'
0 0 0 test 'a'

1 2 3 test u:'a'
1 0 3 test u:'a'
1 2 0 test u:'a'
0 0 0 test u:'a'

1 2 3 test 10&u:'a'
1 0 3 test 10&u:'a'
1 2 0 test 10&u:'a'
0 0 0 test 10&u:'a'

1 2 3 test <4;5;6
1 0 3 test <'sui generis'
1 0 3 test <u:'sui generis'
1 0 3 test <10&u:'sui generis'
1 0 3 test s:@<"0&.> <'sui generis'
1 0 3 test <"0@s: <'sui generis'
1 2 0 test 4
1 2 0 test 4
0 0 0 test <;:'Cogito, ergo sum.'
0 0 0 test <(u:&.>) ;:'Cogito, ergo sum.'
0 0 0 test <(10&u:&.>) ;:'Cogito, ergo sum.'
0 0 0 test <s:@<"0&.> ;:'Cogito, ergo sum.'
0 0 0 test <<"0@s: ;:'Cogito, ergo sum.'

'length error' -: 1 2 3 }. etx i.2 3
'length error' -: 2 3   }. etx 'abcd'
'length error' -: 2 3   }. etx u:'abcd'
'length error' -: 2 3   }. etx 10&u:'abcd'
'length error' -: 2 3   }. etx s:@<"0 'abcd'


NB. x}."r y -------------------------------------------------------------

f =: 4 : 'x}.y'

5  (}."1 -: f"1) i.4 5
_7 (}."1 -: f"1) i.4 5 7
5  (}."2 -: f"2) i.4 5 7

( ?17) (}."1 -: f"1) ?  11 13$1e9 
(-?17) (}."1 -: f"1) ?  11 13$1e9 
( ?17) (}."1 -: f"1) ?5 11 13$1e9
(-?17) (}."1 -: f"1) ?5 11 13$1e9

x =: o.?3 5 7$1e9
n (}."2 -: f"2) x [ n=:0 0
n (}."2 -: f"2) x [ n=:0 2
n (}."2 -: f"2) x [ n=:3 0
n (}."2 -: f"2) x [ n=:3 2
n (}."2 -: f"2) x [ n=:?}.$x
n (}."2 -: f"2) x [ n=:?}.$x
n (}."2 -: f"2) x [ n=:?}.$x

([\.x) -: (i.#x)}."0 1 x=:'abcdefghij'
([\.x) -: (i.#x)}."0 1 x=:u:'abcdefghij'
([\.x) -: (i.#x)}."0 1 x=:10&u:'abcdefghij'
([\.x) -: (i.#x)}."0 1 x=:s:@<"0 'abcdefghij'
                 
3 4 5 (}."1 0 -: f"1 0) i.6 7
3 4 5 (}."1 0 -: f"1 0) i.0 7
3 0 5 (}."1 0 -: f"1 0) i.6 7
3 0 5 (}."1 0 -: f"1 0) i.0 7

(4x }."1 x) -: 4 }."1 x=: (1+?7 20) ?@$ 1e6

(  62$1) -: $ (62$0) }.       7
(7,62$1) -: $ (62$0) }."1 0 i.7
(  61$1) -: $ (61$0) }.       7x
(7,61$1) -: $ (61$0) }."1 0 i.7x


4!:55 ;:'adot1 adot2 sdot0 behead di drop f m n ni pi t test x '
randfini''


epilog''

