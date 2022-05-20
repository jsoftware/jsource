prolog './g321.ijs'
NB. ,.y -----------------------------------------------------------------

randuni''

(1 1$x) -: ,.x=.0
(1 1$x) -: ,.x=.'a'
(1 1$x) -: ,.x=.u:'a'
(1 1$x) -: ,.x=.10&u:'a'
(1 1$x) -: ,.x=.s:@<"0 'a'
(1 1$x) -: ,.x=.123450
(1 1$x) -: ,.x=.3.145
(1 1$x) -: ,.x=.1.23e18j5.67e_89
(1 1$x) -: ,.x=.<i.10

0 1  -: $,.''
0 1  -: $,.u:''
0 1  -: $,.10&u:''
0 1  -: $,.s:''
1 0  -: $,.1 4 5 0$9
2 0  -: $,.2 4 0 5$9
0 12 -: $,.i.0 3 4
0 0  -: $,.i.0 2 3 0
0 0  -: $,.0 0$<''
0 0  -: $,.0 0$<u:''
0 0  -: $,.0 0$<10&u:''
0 0  -: $,.0 0$<s:''

table =: (# , */@}.@$) $ ,

(table x) -: ,.x=.i.?4$10
(table x) -: ,.x=.a.{~256|i.?4$10


NB. ,."r y --------------------------------------------------------------

table =: (# , */@}.@$) $ ,

(,."0 -: table"0) ?2 3 4 5$1e8
(,."1 -: table"1) ?2 3 4 5$1e8
(,."2 -: table"2) ?2 3 4 5$1e8
(,."3 -: table"3) ?2 3 4 5$1e8

(,."0  -: table"0)  (?2 3 4 5$#x){x=.'metaphoric meteoric'
(,."_1 -: table"_1) (?2 3 4 5$#x){x=.'imitative harmony'
(,."_2 -: table"_2) (?2 3 4 5$#x){x=.'personification'
(,."_3 -: table"_3) (?2 3 4 5$#x){x=.'sui generis'

(,."0  -: table"0)  (?2 3 4 5$#x){x=.u:'metaphoric meteoric'
(,."_1 -: table"_1) (?2 3 4 5$#x){x=.u:'imitative harmony'
(,."_2 -: table"_2) (?2 3 4 5$#x){x=.u:'personification'
(,."_3 -: table"_3) (?2 3 4 5$#x){x=.u:'sui generis'

(,."0  -: table"0)  (?2 3 4 5$#x){x=.10&u:'metaphoric meteoric'
(,."_1 -: table"_1) (?2 3 4 5$#x){x=.10&u:'imitative harmony'
(,."_2 -: table"_2) (?2 3 4 5$#x){x=.10&u:'personification'
(,."_3 -: table"_3) (?2 3 4 5$#x){x=.10&u:'sui generis'

(,."0  -: table"0)  (?2 3 4 5$#x){x=.s:@<"0 'metaphoric meteoric'
(,."_1 -: table"_1) (?2 3 4 5$#x){x=.s:@<"0 'imitative harmony'
(,."_2 -: table"_2) (?2 3 4 5$#x){x=.s:@<"0 'personification'
(,."_3 -: table"_3) (?2 3 4 5$#x){x=.s:@<"0 'sui generis'

(,."0 -: table"0) i.i. 5
(,."0 -: table"0) i.i._5
(,."1 -: table"1) i.i. 5
(,."1 -: table"1) i.i._5
(,."2 -: table"2) i.i. 5
(,."2 -: table"2) i.i._5


NB. x,.y ----------------------------------------------------------------

overr =: ,"_1
f     =: ,. -: overr

3 4 -: 3,.4
(3 2$3 4 3 5 3 6) -: 3,.4 5 6
(2 4$0 1 2 9 3 4 5 9) -: (i.2 3),.9
(2 4$9j4 0 1 2 5j6 3 4 5) -: 9j4 5j6,.i.2 3

4 f i.0
4 f~i.0
4 f i.0 9
4 f~i.0 9
4 f i.0 9 2
4 f~i.0 9 2
4 f i.9 0
4 f~i.9 0
4 f i.9 0 2
4 f~i.9 0 2

NB. Boolean
(?2) f ?2
(?2) f ?12$2
(?2) f ?3 10$2
(?2) f ?7 2 10$2
(?11$2) f ?2
(?11$2) f ?11$2
(?11$2) f ?11 1$2
(?11$2) f ?11 3 2$2
(?7 3$2) f ?2
(?7 3$2) f ?7$2
(?7 3$2) f ?7 9$2
(?7 3$2) f ?7 8 9$2
(?6 1 3$2) f ?2
(?6 1 3$2) f ?6$2
(?6 1 3$2) f ?6 9$2
(?6 1 3$2) f ?6 4 9$2

NB. literal
(a.{~?#a.) f a.{~?#a.
(a.{~?#a.) f a.{~?12$#a.
(a.{~?#a.) f a.{~?3 10$#a.
(a.{~?#a.) f a.{~?7 2 10$#a.
(a.{~?11$#a.) f a.{~?#a.
(a.{~?11$#a.) f a.{~?11$#a.
(a.{~?11$#a.) f a.{~?11 1$#a.
(a.{~?11$#a.) f a.{~?11 3 2$#a.
(a.{~?7 3$#a.) f a.{~?#a.
(a.{~?7 3$#a.) f a.{~?7$#a.
(a.{~?7 3$#a.) f a.{~?7 9$#a.
(a.{~?7 3$#a.) f a.{~?7 8 9$#a.
(a.{~?6 1 3$#a.) f a.{~?#a.
(a.{~?6 1 3$#a.) f a.{~?6$#a.
(a.{~?6 1 3$#a.) f a.{~?6 9$#a.
(a.{~?6 1 3$#a.) f a.{~?6 4 9$#a.

NB. literal2
(adot1{~?#adot1) f adot1{~?#adot1
(adot1{~?#adot1) f adot1{~?12$#adot1
(adot1{~?#adot1) f adot1{~?3 10$#adot1
(adot1{~?#adot1) f adot1{~?7 2 10$#adot1
(adot1{~?11$#adot1) f adot1{~?#adot1
(adot1{~?11$#adot1) f adot1{~?11$#adot1
(adot1{~?11$#adot1) f adot1{~?11 1$#adot1
(adot1{~?11$#adot1) f adot1{~?11 3 2$#adot1
(adot1{~?7 3$#adot1) f adot1{~?#adot1
(adot1{~?7 3$#adot1) f adot1{~?7$#adot1
(adot1{~?7 3$#adot1) f adot1{~?7 9$#adot1
(adot1{~?7 3$#adot1) f adot1{~?7 8 9$#adot1
(adot1{~?6 1 3$#adot1) f adot1{~?#adot1
(adot1{~?6 1 3$#adot1) f adot1{~?6$#adot1
(adot1{~?6 1 3$#adot1) f adot1{~?6 9$#adot1
(adot1{~?6 1 3$#adot1) f adot1{~?6 4 9$#adot1

NB. literal4
(adot2{~?#adot2) f adot2{~?#adot2
(adot2{~?#adot2) f adot2{~?12$#adot2
(adot2{~?#adot2) f adot2{~?3 10$#adot2
(adot2{~?#adot2) f adot2{~?7 2 10$#adot2
(adot2{~?11$#adot2) f adot2{~?#adot2
(adot2{~?11$#adot2) f adot2{~?11$#adot2
(adot2{~?11$#adot2) f adot2{~?11 1$#adot2
(adot2{~?11$#adot2) f adot2{~?11 3 2$#adot2
(adot2{~?7 3$#adot2) f adot2{~?#adot2
(adot2{~?7 3$#adot2) f adot2{~?7$#adot2
(adot2{~?7 3$#adot2) f adot2{~?7 9$#adot2
(adot2{~?7 3$#adot2) f adot2{~?7 8 9$#adot2
(adot2{~?6 1 3$#adot2) f adot2{~?#adot2
(adot2{~?6 1 3$#adot2) f adot2{~?6$#adot2
(adot2{~?6 1 3$#adot2) f adot2{~?6 9$#adot2
(adot2{~?6 1 3$#adot2) f adot2{~?6 4 9$#adot2

NB. symbol
(sdot0{~?#sdot0) f sdot0{~?#sdot0
(sdot0{~?#sdot0) f sdot0{~?12$#sdot0
(sdot0{~?#sdot0) f sdot0{~?3 10$#sdot0
(sdot0{~?#sdot0) f sdot0{~?7 2 10$#sdot0
(sdot0{~?11$#sdot0) f sdot0{~?#sdot0
(sdot0{~?11$#sdot0) f sdot0{~?11$#sdot0
(sdot0{~?11$#sdot0) f sdot0{~?11 1$#sdot0
(sdot0{~?11$#sdot0) f sdot0{~?11 3 2$#sdot0
(sdot0{~?7 3$#sdot0) f sdot0{~?#sdot0
(sdot0{~?7 3$#sdot0) f sdot0{~?7$#sdot0
(sdot0{~?7 3$#sdot0) f sdot0{~?7 9$#sdot0
(sdot0{~?7 3$#sdot0) f sdot0{~?7 8 9$#sdot0
(sdot0{~?6 1 3$#sdot0) f sdot0{~?#sdot0
(sdot0{~?6 1 3$#sdot0) f sdot0{~?6$#sdot0
(sdot0{~?6 1 3$#sdot0) f sdot0{~?6 9$#sdot0
(sdot0{~?6 1 3$#sdot0) f sdot0{~?6 4 9$#sdot0

NB. integer
(?2222) f ?2
(?2222) f ?12$1234
(?2222) f ?3 10$2e9
(?2222) f ?7 2 10$2e7
(?11$2e6) f ?2e7
(?11$2e6) f ?11$77
(?11$2e6) f ?11 1$882
(?11$2e6) f ?11 3 2$2
(?7 3$1234) f ?243
(?7 3$1234) f ?7$2
(?7 3$1234) f ?7 9$2123
(?7 3$1234) f ?7 8 9$2
(?6 1 3$2e3) f ?2123
(?6 1 3$2e3) f ?6$2123
(?6 1 3$2e3) f ?6 9$2113 12
(?6 1 3$2e3) f ?6 4 9$2123

NB. floating point
(o.?25) f ?2
(o.?25) f ^?12$10
(o.?25) f ?3 10$2 123
(o.?25) f o.?7 2 10$2123
(^?11$20) f ?2123
(^?11$20) f o.?11$2123
(^?11$20) f o.?11 1$2123
(^?11$20) f ?11 3 2$2
(o.?7 3$234) f o.?2e3
(o.?7 3$234) f o.?7$2e3
(o.?7 3$234) f o.?7 9$2e3
(o.?7 3$234) f o.?7 8 9$2e3
(o.?6 1 3$22) f ^.>:?20
(o.?6 1 3$22) f ^.>:?6$20
(o.?6 1 3$22) f ^.>:?6 9$20
(o.?6 1 3$22) f ^.>:?6 4 9$20

NB. complex
(j.?25) f ?2
(j.?25) f ?12$100
(j.?25) f o.?3 10$2123
(j.?25) f j.?7 2 10$2123
(r.?11$20) f ?2
(r.?11$20) f ?11$2123
(r.?11$20) f o.?11 1$2123
(r.?11$20) f r.?11 3 2$2
(j.?7 3$234) f ?2
(j.?7 3$234) f ?7$2e3
(j.?7 3$234) f o.?7 9$2e3
(j.?7 3$234) f j.?7 8 9$2e3
(r.?6 1 3$22) f r.?20
(r.?6 1 3$22) f r.?6$20
(r.?6 1 3$22) f r.?6 9$20
(r.?6 1 3$22) f r.?6 4 9$20

NB. boxed
(<?2e9) f t{~?       #t=.'asdf';3 4
(<?2e9) f t{~?12    $#t=.2;3;;:'?12$2;:(<?2e9) f t{~?3 10$#t=.2;'
(<?2e9) f t{~?3 10  $#t=.2;;:'(<?2e9) f t{~?3 10$#t=.2;'
(<?2e9) f t{~?7 2 10$#t=.<"0 i.123
(t{~?12$#t) f t{~?        #t=.'asdf';3 4;;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?        #t=.'asdf';3 4;(u:&.>) ;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?        #t=.'asdf';3 4;(10&u:&.>) ;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?        #t=.(s:@<"0&.> <'asdf'),3 4;s:@<"0&.> ;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?        #t=.(s:@<"0&.> <'asdf'),3 4;<"0@s: ;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?        #t=.(<"0@s: <'asdf'),3 4;s:@<"0&.> ;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?        #t=.(<"0@s: <'asdf'),3 4;<"0@s: ;:'Cogito, ergo sum.'
(t{~?12$#t) f t{~?12     $#t=.2;3;;:'?12$2;:(<?2e9) f t{~?3 10$#t=.2;'
(t{~?12$#t) f t{~?12 10  $#t=.2;;:'(<?2e9) f t{~?3 10$#t=.2;'
(t{~?12$#t) f t{~?12 2 10$#t=.<"0 i.123
(<"0?7 2$999) f t{~?       #t=.'asdf';3 4;;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?       #t=.'asdf';3 4;(u:&.>) ;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?       #t=.'asdf';3 4;(10&u:&.>) ;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?       #t=.(s:@<"0&.> <'asdf'),3 4;s:@<"0&.> ;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?       #t=.(s:@<"0&.> <'asdf'),3 4;<"0@s: ;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?       #t=.(<"0@s: <'asdf'),3 4;s:@<"0&.> ;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?       #t=.(<"0@s: <'asdf'),3 4;<"0@s: ;:'Cogito, ergo sum.'
(<"0?7 2$999) f t{~?7     $#t=.2;3;;:'?12$2;:(<?2e9) f t{~?3 10$#t=.2;'
(<"0?7 2$999) f t{~?7 10  $#t=.2;;:'(<?2e9) f t{~?3 10$#t=.2;'
(<"0?7 2$999) f t{~?7 2 10$#t=.<"0 i.123
(t{~?13 2 2$#t) f t{~?       #t=.'asdf';3 4;;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?       #t=.'asdf';3 4;(u:&.>) ;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?       #t=.'asdf';3 4;(10&u:&.>) ;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?       #t=.(s:@<"0&.> <'asdf');3 4;s:@<"0&.> ;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?       #t=.(s:@<"0&.> <'asdf');3 4;<"0@s: ;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?       #t=.(<"0@s: <'asdf');3 4;s:@<"0&.> ;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?       #t=.(<"0@s: <'asdf');3 4;<"0@s: ;:'Cogito, ergo sum.'
(t{~?13 2 2$#t) f t{~?13    $#t=.2;3;;:'?12$2;:(<?2e9) f t{~?3 10$#t=.2;'
(t{~?13 2 2$#t) f t{~?13 3  $#t=.2;;:'(<?2e9) f t{~?3 10$#t=.2;'
(t{~?13 2 2$#t) f t{~?13 2 3$#t=.<"0 ?123$11231

'domain error' -: 1 2 3   ,. etx 'abc'
'domain error' -: 1 2 3   ,. etx u:'abc'
'domain error' -: 1 2 3   ,. etx 10&u:'abc'
'domain error' -: 1 2 3   ,. etx s:@<"0 'abc'
'domain error' -: 1 2 3   ,. etx 2;3;4
'domain error' -: (2;3;4) ,. etx 'abc'
'domain error' -: (2;3;4) ,. etx u:'abc'
'domain error' -: (2;3;4) ,. etx 10&u:'abc'
'domain error' -: (2;3;4) ,. etx s:@<"0 'abc'
'domain error' -: (2;3;4) ,. etx 2 3 4
'domain error' -: 'abc'   ,. etx 1 2 3
'domain error' -: 'abc'   ,. etx 2;3;4

'length error' -: 'abc'   ,. etx 'sui generis'
'length error' -: 'abc'   ,. etx u:'sui generis'
'length error' -: 'abc'   ,. etx 10&u:'sui generis'
'length error' -: (s:@<"0 'abc')   ,. etx s:@<"0 'sui generis'
'length error' -: (,1)    ,. etx 3 4


NB. > ,.&.>/y -----------------------------------------------------------

f=: 4 : 0
 d=: x {~ (10,y) ?@$ #x
 assert. d -: > ,.&.>/ e=: ('';1,0=(y-1) ?@$ 5) <;.1 d
 1
)

0 1                  f"_ 0 ] 100+i.12
a.                   f"_ 0 ] 100+i.12
(a.{~1000 2 ?@$ 256) f"_ 0 ] 100+i.12
(adot1{~1000 2 ?@$(#adot1)) f"_ 0 ] 100+i.12
(adot2{~1000 2 ?@$(#adot2)) f"_ 0 ] 100+i.12
(sdot0{~1000 2 ?@$(#sdot0)) f"_ 0 ] 100+i.12
(u: i.1e3)           f"_ 0 ] 100+i.12
(10&u: i.1e3)        f"_ 0 ] 100+i.12
(1000 ?@$ 1e4)       f"_ 0 ] 100+i.12
(1000 ?@$ 0  )       f"_ 0 ] 100+i.12
(j./ 2 1000 ?@$ 0)   f"_ 0 ] 100+i.12
(<"0 ] 1000 ?@$ 1e5) f"_ 0 ] 100+i.12


4!:55 ;:'adot1 adot2 sdot0 d e f overr t table x'
randfini''


epilog''

