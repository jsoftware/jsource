1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g400.ijs'
NB. #y ------------------------------------------------------------------

randuni''

tally =: {.@(,&1)@$
f     =: # -: tally

0 -: #''
0 -: #$0

1 -: #0
1 -: #'a'
1 -: #u:'a'
1 -: #10&u:'a'
1 -: #s:@<"0 'a'
1 -: #4
1 -: #3.5
1 -: #123j_45
1 -: #<'abc'
1 -: #<u:'abc'
1 -: #<10&u:'abc'
1 -: #s:@<"0&.> <'abc'
1 -: #<"0@s: <'abc'

NB. Boolean
f (?5$4)$0
0 -: #0 5$0

NB. literal
f (?5$4)$'abc'
0 -: #0 5$'abc'

NB. literal2
f (?5$4)$u:'abc'
0 -: #0 5$u:'abc'

NB. literal4
f (?5$4)$10&u:'abc'
0 -: #0 5$10&u:'abc'

NB. symbol
f (?5$4)$s:@<"0 'abc'
0 -: #0 5$s:@<"0 'abc'

NB. integer
f (?5$4)$34
0 -: #0 5$34

NB. floating point
f (?5$4)$3.14
0 -: #0 5$3.14

NB. complex
f (?5$4)$3j4
0 -: #0 5$3j4

NB. boxed
f (?5$4)$<'asdf'
0 -: #0 5$<'asdf'
f (?5$4)$<u:'asdf'
0 -: #0 5$<u:'asdf'
f (?5$4)$<10&u:'asdf'
0 -: #0 5$<10&u:'asdf'
f (?5$4)$s:@<"0&.> <'asdf'
f (?5$4)$<"0@s: <'asdf'
0 -: #0 5$s:@<"0&.> <'asdf'
0 -: #0 5$<"0@s: <'asdf'

(3!:0 ]10x) -: 3!:0 # 10 20 30x
(3!:0 ]10x) -: 3!:0 # 5 % 10 20 30x


NB. x#y -----------------------------------------------------------------

copy =: ; @ (<@($ ,:)"_1)
f    =: # -: copy

3       (# -: copy) 'a'
3       (# -: copy) 'abcd'
1 2 3 4 (# -: copy) 'a'
1 2 3 4 (# -: copy) 'abcd'

3       (# -: copy) u:'a'
3       (# -: copy) u:'abcd'
1 2 3 4 (# -: copy) u:'a'
1 2 3 4 (# -: copy) u:'abcd'

3       (# -: copy) 10&u:'a'
3       (# -: copy) 10&u:'abcd'
1 2 3 4 (# -: copy) 10&u:'a'
1 2 3 4 (# -: copy) 10&u:'abcd'

3       (# -: copy) s:@<"0 'a'
3       (# -: copy) s:@<"0 'abcd'
1 2 3 4 (# -: copy) s:@<"0 'a'
1 2 3 4 (# -: copy) s:@<"0 'abcd'

0       (# -: copy) 12
0       (# -: copy) 21 13 14 15
0       (# -: copy) ?3 4 5$1e4
0 0 0 0 (# -: copy) 12
0 0 0 0 (# -: copy) 21 13 14 15

1       (# -: copy) 12
1       (# -: copy) 21 13 14 15
1       (# -: copy) ?3 4 5$1e4
1 1 1 1 (# -: copy) 12
1 1 1 1 (# -: copy) 21 13 14 15

m=: o.?4 5$100
3       (# -: copy) m
1 2 0 3 (# -: copy) m
0 2 3 0 (# -: copy) m

(3#x) -: (3.5-0.5)#x=:'asdf'
(3#x) -: (3.5-0.5)#x=:u:'asdf'
(3#x) -: (3.5-0.5)#x=:10&u:'asdf'
(3#x) -: (3.5-0.5)#x=:s:@<"0 'asdf'

NB. check for long sequences of 0 in boolean x
((0.05 > ?@$&0) f ?@$&0)"0 (1000) + i. 300
((0.01 > ?@$&0) f ?@$&0)"0 (1000) + i. 300

NB. check different lengths of x/y
((0.5 > ?@$&0) f ?@$&0)"0 (1000) + i. 2000

NB. Inplaceable arguments
(?@$&2 (4 : 'res =. x (# -: (# >:@<:)) y if. -.res do. xx=:x [ yy=:y end. res') ?@$&0)"0 i. 100
(?@$&2 (4 : 'x (# -: (# >:@<:)) y') ?@$&1000)"0 i. 100
(?@$&2 (4 : 'x (# -: (# >:@<:)) y') [: ?@$&1000 ,&5)"0 i. 100
((0.95 > ?@$&0) (4 : 'x (# -: (# >:@<:)) y') ?@$&1000)"0 i. 100   NB. Include long runs of 1s
((0.95 > ?@$&0) (4 : 'x (# -: (# >:@<:)) y') [: ?@$&1000 ,&5)"0 i. 100
NB. Verify long leading 1s are fast
l1 =: (9999#1),0
l0 =: 0 (0}) l1
THRESHOLD +. ((1000) 6!:2 'l0 # i. 10000') (> 1.25&*)&(-&((1000) 6!:2 'i. 10000')) ((1000) 6!:2 'l1 # i. 10000')  NB. occasionally miscompares

10000 > 7!:2 '1 # l0' [ l0 =: i. 100 100   NB. Verify 1 # y does not copy
10000 > 7!:2 '01 # l0' [ l0 =: i. 100 100   NB. Verify 1 # y does not copy
10000 > 7!:2 'l1 # l0' [ l1 =: 1 #~ # l0 =: i. 100 100   NB. Verify (all 1) # y does not copy
10000 > 7!:2 'l1 # l0' [ l1 =: 1 #~ # l0 =: i. 10000   NB. Verify (all 1) # y does not copy

NB. NaN related tests
(2#_1e6 _1e6)    -: _1e6 _1e6 _1e6 _1e6
(3 2$_834524)    -: 1 0 1 0 1 # 5 2$_834524
(3 8$240 255{a.) -: 1 0 1 0 1 # 5 8$240 255{a.

'domain error' -: 'abc'   # etx 3 4 5
'domain error' -: 1 2 _3  # etx 3 4 5
'domain error' -: (2;3;4) # etx 'abc'
'domain error' -: (2;3;4) # etx u:'abc'
'domain error' -: (2;3;4) # etx 10&u:'abc'
'domain error' -: (2;3;4) # etx s:@<"0 'abc'
'domain error' -: 2 3.4   # etx i.2 3
'domain error' -: 3.4     # etx i.2 3
'domain error' -: 2 3j4.5 # etx i.2 3
'domain error' -: 3j4.5   # etx i.2 3

'length error' -: (,2)    # etx 4 5 6
'length error' -: 2 3     # etx 4 5 6
'length error' -: 2 3 4   # etx i.2 5
'length error' -: 4 5 6 7 # etx i.0

NB. following test takes a long time (and may complete) on big machines
NB. (<2 1e9 2e9 # etx 'abc') e. 'limit error';'out of memory'

'limit error' -: (2^IF64{30 62)# etx 'abc'
'limit error' -: (2^IF64{30 62)# etx u:'abc'
'limit error' -: (2^IF64{30 62)# etx 10&u:'abc'
'limit error' -: (2^IF64{30 62)# etx s:@<"0 'abc'

NB. b#"r x  -------------------------------------------------------------

f1=: 3 : 0
 n=: y
 assert. (b=: ?n$2) (# -: copy) xx=: ?n$2
 assert. (b=: ?n$2) (# -: copy) xx=: a.{~?n$#a.
 assert. (b=: ?n$2) (# -: copy) xx=: a.{~?(n,7)$#a.
 assert. (b=: ?n$2) (# -: copy) xx=: adot1{~?n$#adot1
 assert. (b=: ?n$2) (# -: copy) xx=: adot1{~?(n,7)$#adot1
 assert. (b=: ?n$2) (# -: copy) xx=: adot2{~?n$#adot2
 assert. (b=: ?n$2) (# -: copy) xx=: adot2{~?(n,7)$#adot2
 assert. (b=: ?n$2) (# -: copy) xx=: sdot0{~?n$#sdot0
 assert. (b=: ?n$2) (# -: copy) xx=: sdot0{~?(n,7)$#sdot0
 assert. (b=: ?n$2) (# -: copy) xx=: ?n$1e9
 assert. (b=: ?n$2) (# -: copy) xx=: o.?n$1e9
 assert. (b=: 0.95 > ?n$0) (# -: copy) xx=: ?n$1e9  NB. include long runs of 1
 assert. (b=: 0.95 > ?n$0) (# -: copy) xx=: o.?n$1e9
 assert. (b=: ?n$2) (# -: copy) xx=: j./?(2,n)$1e9
 assert. (b=: ?n$2) (# -: copy) xx=: <"0 ?n$1e9
 assert. (b=: ?n$2) (# -: copy) xx=:    ?   n $100000x
 assert. (b=: ?n$2) (# -: copy) xx=: % /?(2,n)$100000x
 1
)

f1"0 ]100 101 102 103

f2=: 3 : 0
 n=: y
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: ?(3,n)$2
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: a.{~?(3,n)$#a.
 assert. (b=: ?n$2) (#"2 -: copy"2) xx=: a.{~?(3,n,7)$#a.
 assert. (b=: 0.95 > ?n$0) (#"1 -: copy"1) xx=: a.{~?(3,n)$#a.  NB. include long runs of 1
 assert. (b=: 0.95 > ?n$0) (#"2 -: copy"2) xx=: a.{~?(3,n,7)$#a.
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: adot1{~?(3,n)$#adot1
 assert. (b=: ?n$2) (#"2 -: copy"2) xx=: adot1{~?(3,n,7)$#adot1
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: adot2{~?(3,n)$#adot2
 assert. (b=: ?n$2) (#"2 -: copy"2) xx=: adot2{~?(3,n,7)$#adot2
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: sdot0{~?(3,n)$#sdot0
 assert. (b=: ?n$2) (#"2 -: copy"2) xx=: sdot0{~?(3,n,7)$#sdot0
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: ?(3,n)$1e9
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: o.?(3,n)$1e9
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: j./?(2 3,n)$1e9
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: <"0 ?(3,n)$1e9
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=:     ?(3,n) $100000x
 assert. (b=: ?n$2) (#"1 -: copy"1) xx=: % /?(2 3,n)$100000x
 1
)

f2"0 ]100 101 102 103

f3=: 4 : 0
 n=: x
 c=: y
 assert. (b=: ?n$2) (# -: copy) xx=: a.{~?(n,c)$#a.
 1
)

100 101 102 103 104 f3"0/ i.21

f4=: 4 : 0
 n=: x
 c=: y
 assert. (b=: ?n$2) (# -: copy) xx=: a{~?(n,c)$#a=: 0 1 255{a.
 1
)

100 101 102 103 104 f4"0/ i.21

''        -: (0$0) # ''
(i.3 0 5) -: (0$0) #"2 i.3 0 5

'length error' -: 1 0 1 #   etx i.4
'length error' -: 1 0 1 #"2 etx i. 4 5 6


NB. x#y, complex x ------------------------------------------------------

'aa    b'        -: 2j4 1 # 'ab'
'aa    b'        -: 2j4 1 # u:'ab'
'aa    b'        -: 2j4 1 # 10&u:'ab'
((s:@<"0 'aa'),(4${.s:''),s:@<"0 'b') -: 2j4 1 # s:@<"0 'ab'
3 3 0 0 0 0 4    -: 2j4 1 # 3 4

'aa    a'        -: 2j4 1 # 'a'
'aa    a'        -: 2j4 1 # u:'a'
'aa    a'        -: 2j4 1 # 10&u:'a'
((s:@<"0 'aa'),(4${.s:''),s:@<"0 'a')        -: 2j4 1 # s:@<"0 'a'
(2 4 2 4#'a b ') -: 2j4 #'ab'
(2 4 2 4#u:'a b ') -: 2j4 #u:'ab'
(2 4 2 4#10&u:'a b ') -: 2j4 #10&u:'ab'
(2 4 2 4#(s:@<"0 'a'),({.s:''),(s:@<"0 'b'),{.s:'') -: 2j4 #s:@<"0 'ab'
'aa    '         -: 2j4 # 'a'
'aa    '         -: 2j4 # u:'a'
'aa    '         -: 2j4 # 10&u:'a'
((s:@<"0 'aa'),4${.s:'')         -: 2j4 # s:@<"0 'a'

(1 0 2 4# ,/x,:"1[0) -: 1 2j4 # x=: i.2 3

f=: 4 : 'x#y'

x (f"1 -: #"1) y=: ?3 4 9$100 [ x=: j./?2 9$5
x (f"2 -: #"2) y=: ?3 9 4$100


NB. x#!.f y -------------------------------------------------------------

'aa____b'        -: 2j4 1 #!.'_' 'ab'
'aa____b'        -: 2j4 1 #!.(u:'_') u:'ab'
'aa____b'        -: 2j4 1 #!.(10&u:'_') 10&u:'ab'
(s:@<"0 'aa____b')        -: 2j4 1 #!.(s:@<"0 '_') s:@<"0 'ab'
3 3 9 9 9 9 4    -: 2j4 1 #!.9[  3 4

'aa____a'        -: 2j4 1 #!.'_' 'a'
'aa____a'        -: 2j4 1 #!.(u:'_') u:'a'
'aa____a'        -: 2j4 1 #!.(10&u:'_') 10&u:'a'
(s:@<"0 'aa____a')        -: 2j4 1 #!.(s:@<"0 '_') s:@<"0 'a'
(2 4 2 4#'a_b_') -: 2j4   #!.'_' 'ab'
(2 4 2 4#u:'a_b_') -: 2j4   #!.(u:'_') u:'ab'
(2 4 2 4#10&u:'a_b_') -: 2j4   #!.(10&u:'_') 10&u:'ab'
(2 4 2 4#s:@<"0 'a_b_') -: 2j4   #!.(s:@<"0 '_') s:@<"0 'ab'
'aa____'         -: 2j4   #!.'_' 'a'
'aa____'         -: 2j4   #!.(u:'_') u:'a'
'aa____'         -: 2j4   #!.(10&u:'_') 10&u:'a'
(s:@<"0 'aa____')         -: 2j4   #!.(s:@<"0 '_') s:@<"0 'a'

(1 0 2 4# ,/x,:"1[3j4) -: 1 2j4 #!.3j4 x=:i.2 3

'domain error' -: 9 3j1  #!.'a'  etx 4
'domain error' -: 9 3j1  #!.(u:'a')  etx 4
'domain error' -: 9 3j1  #!.(10&u:'a')  etx 4
'domain error' -: 9 3j1  #!.(s:@<"0 'a')  etx 4
'domain error' -: 1 2j3  #!.'a'  etx 'b';'c'
'domain error' -: 1 2j3  #!.'a'  etx u:&.> 'b';'c'
'domain error' -: 1 2j3  #!.'a'  etx 10&u:&.> 'b';'c'
'domain error' -: 1 2j3  #!.'a'  etx <"0@s: 'b';'c'
'domain error' -: 3j4    #!.4    etx 'sui generis'
'domain error' -: 3j4    #!.4    etx u:'sui generis'
'domain error' -: 3j4    #!.4    etx 10&u:'sui generis'
'domain error' -: 3j4    #!.4    etx s:@<"0 'sui generis'
'domain error' -: 5 3j5 4#!.4    etx ;:'Cogito, ergo'
'domain error' -: 5 3j5 4#!.4    etx u:&.> ;:'Cogito, ergo'
'domain error' -: 5 3j5 4#!.4    etx 10&u:&.> ;:'Cogito, ergo'
'domain error' -: 5 3j5 4#!.4    etx s:@<"0&.> ;:'Cogito, ergo'
'domain error' -: 5 3j5 4#!.4    etx <"0@s: ;:'Cogito, ergo'
'domain error' -: 1j7    #!.(<4) etx 'eleemosynary'
'domain error' -: 1j7    #!.(<4) etx u:'eleemosynary'
'domain error' -: 1j7    #!.(<4) etx 10&u:'eleemosynary'
'domain error' -: 1j7    #!.(<4) etx s:@<"0 'eleemosynary'
'domain error' -: 9 3j4  #!.(<4) etx i.2 3


NB. x#"r y --------------------------------------------------------------

f=: 4 : 'x#y'

1 0 2 (#"0 -: f"0) x=:?3 3 3 3$1e5
1 0 2 (#"1 -: f"1) x
1 0 2 (#"2 -: f"2) x
1 0 2 (#"3 -: f"3) x
1 0 2 (#"4 -: f"4) x

1 0 1 (#"0 -: f"0) x=:?3 3 3 3$1e5
1 0 1 (#"1 -: f"1) x
1 0 1 (#"2 -: f"2) x
1 0 1 (#"3 -: f"3) x
1 0 1 (#"4 -: f"4) x

0 (#"0 -: f"0) x=:?3 3 3 3$1e5
0 (#"1 -: f"1) x
0 (#"2 -: f"2) x
0 (#"3 -: f"3) x
0 (#"4 -: f"4) x

1 (#"0 -: f"0) x=:?3 3 3 3$1e5
1 (#"1 -: f"1) x
1 (#"2 -: f"2) x
1 (#"3 -: f"3) x
1 (#"4 -: f"4) x

2 (#"0 -: f"0) x=:?3 3 3 3$1e5
2 (#"1 -: f"1) x
2 (#"2 -: f"2) x
2 (#"3 -: f"3) x
2 (#"4 -: f"4) x

1 0 2 (#"0 -: f"0) x=:o.?1e6
1 0 2 (#"1 -: f"1) x

3 (#"0 -: f"0) x=:4 4 4$'antidisestablishmentarianism'
3 (#"1 -: f"1) x
3 (#"2 -: f"2) x
3 (#"3 -: f"3) x

3 (#"0 -: f"0) x=:4 4 4$u:'antidisestablishmentarianism'
3 (#"1 -: f"1) x
3 (#"2 -: f"2) x
3 (#"3 -: f"3) x

3 (#"0 -: f"0) x=:4 4 4$10&u:'antidisestablishmentarianism'
3 (#"1 -: f"1) x
3 (#"2 -: f"2) x
3 (#"3 -: f"3) x

3 (#"0 -: f"0) x=:4 4 4$s:@<"0 'antidisestablishmentarianism'
3 (#"1 -: f"1) x
3 (#"2 -: f"2) x
3 (#"3 -: f"3) x

(?5 2$10) (#    -: f"1 _) x=:2 5$;:'When eras die their legacies are left to strange police'
(?5 2$10) (#"_1 -: f"_1 ) y=:5 2$;:'Professors in New England guard the glory that was Greece'

(?5 2$10) (#    -: f"1 _) x=:2 5$(u:&.>) ;:'When eras die their legacies are left to strange police'
(?5 2$10) (#"_1 -: f"_1 ) y=:5 2$(u:&.>) ;:'Professors in New England guard the glory that was Greece'

(?5 2$10) (#    -: f"1 _) x=:2 5$(10&u:&.>) ;:'When eras die their legacies are left to strange police'
(?5 2$10) (#"_1 -: f"_1 ) y=:5 2$(10&u:&.>) ;:'Professors in New England guard the glory that was Greece'

(?5 2$10) (#    -: f"1 _) x=:2 5$s:@<"0&.> ;:'When eras die their legacies are left to strange police'
(?5 2$10) (#    -: f"1 _) x=:2 5$<"0@s: ;:'When eras die their legacies are left to strange police'
(?5 2$10) (#"_1 -: f"_1 ) y=:5 2$s:@<"0&.> ;:'Professors in New England guard the glory that was Greece'
(?5 2$10) (#"_1 -: f"_1 ) y=:5 2$<"0@s: ;:'Professors in New England guard the glory that was Greece'

1j2 3j4 0j5 (#"0 0 -: f"0 0) x=:(?3 3 3$3){;:'chirality paronomasiac onomatopoeia'
1j2 3j4 0j5 (#"0 1 -: f"0 1) x
1j2 3j4 0j5 (#"0 2 -: f"0 2) x
1j2 3j4 0j5 (#"0 3 -: f"0 3) x
1j2 3j4 0j5 (#"1 0 -: f"1 0) x
1j2 3j4 0j5 (#"1 1 -: f"1 1) x
1j2 3j4 0j5 (#"1 2 -: f"1 2) x
1j2 3j4 0j5 (#"1 3 -: f"1 3) x

1j2 3j4 0j5 (#"0 0 -: f"0 0) x=:(?3 3 3$3){(u:&.>) ;:'chirality paronomasiac onomatopoeia'
1j2 3j4 0j5 (#"0 1 -: f"0 1) x
1j2 3j4 0j5 (#"0 2 -: f"0 2) x
1j2 3j4 0j5 (#"0 3 -: f"0 3) x
1j2 3j4 0j5 (#"1 0 -: f"1 0) x
1j2 3j4 0j5 (#"1 1 -: f"1 1) x
1j2 3j4 0j5 (#"1 2 -: f"1 2) x
1j2 3j4 0j5 (#"1 3 -: f"1 3) x

1j2 3j4 0j5 (#"0 0 -: f"0 0) x=:(?3 3 3$3){(10&u:&.>) ;:'chirality paronomasiac onomatopoeia'
1j2 3j4 0j5 (#"0 1 -: f"0 1) x
1j2 3j4 0j5 (#"0 2 -: f"0 2) x
1j2 3j4 0j5 (#"0 3 -: f"0 3) x
1j2 3j4 0j5 (#"1 0 -: f"1 0) x
1j2 3j4 0j5 (#"1 1 -: f"1 1) x
1j2 3j4 0j5 (#"1 2 -: f"1 2) x
1j2 3j4 0j5 (#"1 3 -: f"1 3) x

1j2 3j4 0j5 (#"0 0 -: f"0 0) x=:(?3 3 3$3){s:@<"0&.> ;:'chirality paronomasiac onomatopoeia'
1j2 3j4 0j5 (#"0 1 -: f"0 1) x
1j2 3j4 0j5 (#"0 2 -: f"0 2) x
1j2 3j4 0j5 (#"0 3 -: f"0 3) x
1j2 3j4 0j5 (#"1 0 -: f"1 0) x
1j2 3j4 0j5 (#"1 1 -: f"1 1) x
1j2 3j4 0j5 (#"1 2 -: f"1 2) x
1j2 3j4 0j5 (#"1 3 -: f"1 3) x

1j2 3j4 0j5 (#"0 0 -: f"0 0) x=:(?3 3 3$3){<"0@s: ;:'chirality paronomasiac onomatopoeia'
1j2 3j4 0j5 (#"0 1 -: f"0 1) x
1j2 3j4 0j5 (#"0 2 -: f"0 2) x
1j2 3j4 0j5 (#"0 3 -: f"0 3) x
1j2 3j4 0j5 (#"1 0 -: f"1 0) x
1j2 3j4 0j5 (#"1 1 -: f"1 1) x
1j2 3j4 0j5 (#"1 2 -: f"1 2) x
1j2 3j4 0j5 (#"1 3 -: f"1 3) x

(i.0 8  ) -: 3 1 4   #"1 i.0 3
(i.0 0 7) -: (i.0 5) #   5 7$'sesquipedalian'
(i.0 0 7) -: (i.0 5) #   5 7$u:'sesquipedalian'
(i.0 0 7) -: (i.0 5) #   5 7$10&u:'sesquipedalian'
(i.0 0 7) -: (i.0 5) #   5 7$s:@<"0 'sesquipedalian'

(i.1e9     0) -: 1e4#   i.1e5   0
(i.1e9 2e4 0) -: 1e4#"2 i.1e9 2 0

NB. Test integrated rank support on verbs with rank other than 0 or _
2 2 5 5 -: $ (0 1 ,: 1 0)  |:"2 2   i. 2 3 5
2 2 6 5 -: $ (,. 5 6) $"2 2 i. 2 3 5
2 2 6 -: $ (,. 5 6) ($,)"2 2 i. 2 3 5
2 2 1 5 -: $ (0 1 0,: 1 0 0)  #"2 2   i. 2 3 5
2 2 6 5 -: $ (,. 5 6) {."{."2 2 i. 2 3 5
2 2 1 5 -: $ (,. 2 3) }."2 2 i. 2 3 5

NB. Test the short-circuit cases that don't have to do anything on their arguments
a =: i. 1e6
NB. (6!:2 '1 # a') < 0.01 * 6!:2 '2 # a'
THRESHOLD +. (1-threshold) > (6!:2 '1 # a') % 0.01 * 6!:2 '2 # a'
NB. Now virtual (6!:2 '0 }. a') < 0.01 * 6!:2 '1 }. a'
NB. (6!:2 '0 |. a') < 0.01 * 6!:2 '1 |. a'
NB. (6!:2 '0 |.!.5 a') < 0.01 * 6!:2 '1 |.!.5 a'
THRESHOLD +. (1-threshold) > (6!:2 '0 |. a') % 0.01 * 6!:2 '1 |. a'
THRESHOLD +. (1-threshold) > (6!:2 '0 |.!.5 a') % 0.01 * 6!:2 '1 |.!.5 a'

NB. test virtual implementation
f =: 3 : 0`1:@.QKTEST   NB. take too long on mobile
NB. direct =. (3!:0 y) e. 1 2 4 8 16 65536 131072 262144
siz12002 =: 1000 + 0.6 * 7!:2 'a =: 100 12000 $ y'
a0 =: '' ($,) a
siz1200 =: 1000 + 0.6 * 7!:2 '({:$a) $ a0'

a1 =: ,: a


assert. siz1200 > 7!:2 '0: {. a'
assert. siz1200 < 7!:2 '0: b =: {. a'
b =: 1 |. _1 |. a
assert. siz1200 > 7!:2 'b =: 0:@{. b'  NB. Simple assignment would realize

assert. siz1200 > 7!:2 '0: {: a'
assert. siz1200 < 7!:2 '0: b =: {: a'
b =: 1 |. _1 |. a
assert. siz1200 > 7!:2 'b =: 0:@{: b'

assert. siz12002 > 7!:2 '0: }. a'
assert. siz12002 < 7!:2 '0: b =: }. a'
assert. siz1200 > 7!:2 'b =: 0:@}. b'

assert. siz12002 > 7!:2 '0: }: a'
assert. siz12002 < 7!:2 '0: b =: }: a'
assert. siz1200 > 7!:2 'b =: 0:@}: b'

assert. siz1200 > 7!:2 '$ 1 {. a'
assert. siz1200 < 7!:2 '$ b =: 1 {. a'
assert. siz1200 < 7!:2 '$ (,1) {. a'
assert. siz1200 > 7!:2 '$ _1 {. a'
assert. siz1200 < 7!:2 '$ b =: _1 {. a'
assert. siz12002 > 7!:2 '$ 90 {. a'
assert. siz12002 < 7!:2 '$ b =: 90 {. a'
assert. siz1200 > 7!:2 'b =: 0:@(89&{.) b'

assert. siz12002 > 7!:2 '$ 1 }. a'
assert. siz12002 < 7!:2 '$ b =: 1 }. a'
assert. siz12002 < 7!:2 '$ (,1) }. a'
assert. siz12002 > 7!:2 '$ _1 }. a'
assert. siz12002 < 7!:2 '$ b =: _1 }. a'
assert. siz12002 > 7!:2 '$ 10 }. a'
assert. siz12002 < 7!:2 '$ b =: 10 }. a'
assert. siz1200 > 7!:2 'b =: 0:@(10&}.) b'

assert. siz1200 > 7!:2 '$ (3-2) { a'
assert. siz1200 > 7!:2 '$ 1 { a'
assert. siz1200 > 7!:2 '$ 0 { a'
assert. siz1200 > 7!:2 '$ 1 2 3 { a'
assert. siz1200 > 7!:2 '$ _3 _2 _1 { a'
assert. siz1200 > 7!:2 '$ _1 { a'
assert. siz1200 < 7!:2 '$ b =: 1 { a'
assert. siz1200 < 7!:2 '$ b =: 1 2 3 { a'
assert. siz1200 > 7!:2 '$ (1,:2) { a'

assert. siz12002 > 7!:2 '$ , a'
assert. siz12002 < 7!:2 '$ b =: , a'
assert. siz12002 > 7!:2 '$ ,"2 a'
assert. siz12002 > 7!:2 '$ ,"1 a'  NB. NOP
assert. siz12002 < 7!:2 '$ b =: ,"2 a'
assert. siz12002 > 7!:2 '$ b =: ,"1 a'  NB. NOP
assert. siz12002 > 7!:2 '$ ,"0 a'
b =: 1 |. _1 |. a
assert. siz1200 > 7!:2 'b =: 0:@:(,"1) b'
b =: 1 |. _1 |. a
assert. siz1200 > 7!:2 'b =: 0:@, b'
b =: {. a
assert. siz1200 > 7!:2 'b =: 0:@, b'

assert. siz12002 > 7!:2 '$ 99 $ a'
assert. siz12002 < 7!:2 '$ b =: 99 $ a'
assert. siz12002 < 7!:2 '$ 101 $ a'
assert. siz12002 < 7!:2 '$ 11000 $"1 a'
assert. siz12002 > 7!:2 '$ 99 $"2 a1'
b =: 1 |. _1 |. a
assert. siz1200 > 7!:2 'b =: 0:@(99&$) b'

assert. siz12002 > 7!:2 '$ 99 11000 ($,) a'
assert. siz12002 < 7!:2 '$ b =: 99 11000 ($,) a'
assert. siz12002 < 7!:2 '$ 101 12000 ($,) a'
assert. siz12002 < 7!:2 '$ 11000 ($,)"1 a'
assert. siz12002 > 7!:2 '$ 99 11000 ($,)"2 a1'
b =: 1 |. _1 |. a
assert. siz1200 > 7!:2 'b =: 0:@(99 11000&($,)) b'


assert. (siz1200+800) > 7!:2 '0:@]"1 a'   NB. rank operator
assert. (siz1200+800) > 7!:2 'a 0:@]"1 a'
1
)
f 0
f 2
f 5.5
f 1j1
f a:
f s: 'word'
f u: 'a'
f 10 u: 'a'

NB. Verify that virtuals pass through a sequence of open/box verbs
a =: <"1 i. 120 100000
(7!:2 '{.&.> }.&.> a') > 100 * (7!:2 '{.&.> @ (}.&.>) a')
({.&.> @: (}.&.>) a) -: ({.&.> }.&.> a)
({.&.> @ (}.&.>) a) -: ({.&.> }.&.> a)

 ,(2e6 2e6 _2e6 _2e6)  (*/\  0&=@:- */\ @((o.0)&+))@,&> 3e7 _3e7 3e7 _3e7

NB. Verify that WILLOPEN is passed through fork
a =: 1000000$4
b =: 1000000 $ 250000 {. 1
c =: 4 250000 $ 4
16000 > 7!:2 '(2: 0:&.> b&(<;.1)) a'
16000 > 7!:2 '(2 0:&.> b&(<;.1)) a'
16000 > 7!:2 'b (I.@[ 0:&.> (<;.1)) a'
16000 > 7!:2 'b (2 0:&.> (<;.1)) a'
1000000 < 7!:2 '(2: ; b&(<;.1)) a'
1000000 < 7!:2 '(2 ; b&(<;.1)) a'
1000000 < 7!:2 'b (I.@[ ; (<;.1)) a'
1000000 < 7!:2 'b (2 ; (<;.1)) a'


NB. Now hook
16000 > 7!:2 '1 2 3 4 (0:&.> b&(<;.1)) a'
1000000 < 7!:2 '0: 1 2 3 4 (; b&(<;.1)) a'  NB. ; opens only in monad
NB. No because > is not ip  16000 > 7!:2 '(0:&.>   [: b&(<;.1) >) <a'
1000000 < 7!:2 '0: (; b&(<;.1)) a'  NB. ; opens only in monad
NB. Not yet 16000 > 7!:2 '(([ 0:&.>) b&(<;.1)) a'
NB. Not yet 16000 > 7!:2 '(''''"_&> 1 1 1 1&(<;.1)) c'

NB. Capped fork
16000 > 7!:2 '([: 0:&.> b&(<;.1)) a'
16000 > 7!:2 'b ([: 0:&.> (<;.1)) a'
16000 > 7!:2 '([: 0:&.> b&(<;.1)) a'
16000 > 7!:2 'b ([: 0:&.> (<;.1)) a'

16000 > 7!:2 '(0:&.> @: (b&(<;.1))) a'
16000 > 7!:2 'b (0:&.>@:(<;.1)) a'

NB. zapped virtual block - used to corrupt memory
(;:'def ghi') -: 3 : 0 }. <"1 ] 3 3$'abcdefghi'  NB. zapped virtual block
3 : 'y=. [:^:0: y' y  NB. passes into verb, comes back, without freeing backer
)



4!:55 ;:'a a0 a1 abox adot1 adot2 sdot0 b c copy f f1 f2 f3 f4 g l0 l1 m n siz1200 siz12002 tally x xx y '
randfini''

