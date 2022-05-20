prolog './g231.ijs'
NB. |.y -----------------------------------------------------------------

randuni''

rev =: ]`((<:-i.)@# { ]) @. (*@#@$)

NB. Boolean
(|. -: rev) 1=?50 2 3$2
(|. -: rev) 1=?1 100$2
(|. -: rev) 1=?100 0$2
(|. -: rev) 1

NB. literal
(|. -: rev) (?45 1 2 2$#a){a=:'foo upon thee 1=?10 20$2'
(|. -: rev) (?400 1 1$#a){a
(|. -: rev) (?0 10 2$#a){a
(|. -: rev) 'a'
(|. -: rev) ''

NB. literal2
(|. -: rev) (?45 1 2 2$#a){a=:u:'foo upon thee 1=?10 20$2'
(|. -: rev) (?400 1 1$#a){a
(|. -: rev) (?0 10 2$#a){a
(|. -: rev) u:'a'
(|. -: rev) u:''

NB. literal4
(|. -: rev) (?45 1 2 2$#a){a=:10&u:'foo upon thee 1=?10 20$2'
(|. -: rev) (?400 1 1$#a){a
(|. -: rev) (?0 10 2$#a){a
(|. -: rev) 10&u:'a'
(|. -: rev) 10&u:''

NB. integer
(|. -: rev) ?25 2 2 2$212341
(|. -: rev) ?400 1$123541
(|. -: rev) ?0 1000$123456
(|. -: rev) _4
(|. -: rev) i.0

NB. floating point
(|. -: rev) o.?35 3 5$212341
(|. -: rev) o.?1 40$123541
(|. -: rev) o.?100 0$123456
(|. -: rev) 3.4
(|. -: rev) ,_3.4

NB. complex
(|. -: rev) ^0j1*?30 2 5$21234
(|. -: rev) ^0j1*?1 256$1235
(|. -: rev) ^0j1*?127 0$1234
(|. -: rev) 3j4
(|. -: rev) 0$3j4

NB. boxed
(|. -: rev) (?30 2 1 1 1$#x){x=:;:'Cogito, ergo sum. $212 341 CBC News'
(|. -: rev) (?30 2 1 1 1$#x){x=:(u:&.>) ;:'Cogito, ergo sum. $212 341 CBC News'
(|. -: rev) (?30 2 1 1 1$#x){x=:(10&u:&.>) ;:'Cogito, ergo sum. $212 341 CBC News'
(|. -: rev) (?255 1$#x){x
(|. -: rev) (?0 0$#x){x
(|. -: rev) <i.2 3
(|. -: rev) ,<i.2 3

NB. literal
(|. -: rev) x=:a.{~?50  1$#a.
(|. -: rev) x=:a.{~?50  2$#a.
(|. -: rev) x=:a.{~?50  3$#a.
(|. -: rev) x=:a.{~?50  4$#a.
(|. -: rev) x=:a.{~?50  5$#a.
(|. -: rev) x=:a.{~?50  6$#a.
(|. -: rev) x=:a.{~?50  7$#a.
(|. -: rev) x=:a.{~?50  8$#a.
(|. -: rev) x=:a.{~?50  9$#a.
(|. -: rev) x=:a.{~?50 10$#a.
(|. -: rev) x=:a.{~?50 11$#a.
(|. -: rev) x=:a.{~?50 12$#a.

NB. literal2
(|. -: rev) x=:adot1{~?50  1$#adot1
(|. -: rev) x=:adot1{~?50  2$#adot1
(|. -: rev) x=:adot1{~?50  3$#adot1
(|. -: rev) x=:adot1{~?50  4$#adot1
(|. -: rev) x=:adot1{~?50  5$#adot1
(|. -: rev) x=:adot1{~?50  6$#adot1
(|. -: rev) x=:adot1{~?50  7$#adot1
(|. -: rev) x=:adot1{~?50  8$#adot1
(|. -: rev) x=:adot1{~?50  9$#adot1
(|. -: rev) x=:adot1{~?50 10$#adot1
(|. -: rev) x=:adot1{~?50 11$#adot1
(|. -: rev) x=:adot1{~?50 12$#adot1

NB. literal4
(|. -: rev) x=:adot2{~?50  1$#adot2
(|. -: rev) x=:adot2{~?50  2$#adot2
(|. -: rev) x=:adot2{~?50  3$#adot2
(|. -: rev) x=:adot2{~?50  4$#adot2
(|. -: rev) x=:adot2{~?50  5$#adot2
(|. -: rev) x=:adot2{~?50  6$#adot2
(|. -: rev) x=:adot2{~?50  7$#adot2
(|. -: rev) x=:adot2{~?50  8$#adot2
(|. -: rev) x=:adot2{~?50  9$#adot2
(|. -: rev) x=:adot2{~?50 10$#adot2
(|. -: rev) x=:adot2{~?50 11$#adot2
(|. -: rev) x=:adot2{~?50 12$#adot2

NB. symbol
(|. -: rev) x=:sdot0{~?50  1$#sdot0
(|. -: rev) x=:sdot0{~?50  2$#sdot0
(|. -: rev) x=:sdot0{~?50  3$#sdot0
(|. -: rev) x=:sdot0{~?50  4$#sdot0
(|. -: rev) x=:sdot0{~?50  5$#sdot0
(|. -: rev) x=:sdot0{~?50  6$#sdot0
(|. -: rev) x=:sdot0{~?50  7$#sdot0
(|. -: rev) x=:sdot0{~?50  8$#sdot0
(|. -: rev) x=:sdot0{~?50  9$#sdot0
(|. -: rev) x=:sdot0{~?50 10$#sdot0
(|. -: rev) x=:sdot0{~?50 11$#sdot0
(|. -: rev) x=:sdot0{~?50 12$#sdot0


NB. |."r y --------------------------------------------------------------

f =: 3 : '|.y'

(|."0 -: f"0) ?2 3 4$1e6
(|."1 -: f"1) a.{~?2 3 4$#a.
(|."1 -: f"1) adot1{~?2 3 4$#adot1
(|."1 -: f"1) adot2{~?2 3 4$#adot2
(|."2 -: f"2) o.?2 3 5$1e6
(|."3 -: f"3) ?2 3 4$2e5

NB. literal
(|."2 -: rev"2) x=:a.{~?4 12  1$#a.
(|."2 -: rev"2) x=:a.{~?4 12  2$#a.
(|."2 -: rev"2) x=:a.{~?4 12  3$#a.
(|."2 -: rev"2) x=:a.{~?4 12  4$#a.
(|."2 -: rev"2) x=:a.{~?4 12  5$#a.
(|."2 -: rev"2) x=:a.{~?4 12  6$#a.
(|."2 -: rev"2) x=:a.{~?4 12  7$#a.
(|."2 -: rev"2) x=:a.{~?4 12  8$#a.
(|."2 -: rev"2) x=:a.{~?4 12  9$#a.
(|."2 -: rev"2) x=:a.{~?4 12 10$#a.
(|."2 -: rev"2) x=:a.{~?4 12 11$#a.
(|."2 -: rev"2) x=:a.{~?4 12 12$#a.

NB. literal2
(|."2 -: rev"2) x=:adot1{~?4 12  1$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  2$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  3$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  4$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  5$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  6$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  7$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  8$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12  9$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12 10$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12 11$#adot1
(|."2 -: rev"2) x=:adot1{~?4 12 12$#adot1

NB. literal4
(|."2 -: rev"2) x=:adot2{~?4 12  1$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  2$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  3$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  4$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  5$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  6$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  7$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  8$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12  9$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12 10$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12 11$#adot2
(|."2 -: rev"2) x=:adot2{~?4 12 12$#adot2

NB. symbol
(|."2 -: rev"2) x=:sdot0{~?4 12  1$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  2$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  3$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  4$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  5$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  6$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  7$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  8$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12  9$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12 10$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12 11$#sdot0
(|."2 -: rev"2) x=:sdot0{~?4 12 12$#sdot0


NB. x|.y ----------------------------------------------------------------

rank =: #@$
rot  =: ]`(((i.@]-]-|~)#){])@.(*@rank@])"0 _

NB. Boolean
(_50+?100) (|. -: rot) 1=?50 2 3$2
(_50+?100) (|. -: rot) 1=?1 100$2
(_50+?100) (|. -: rot) 1=?100 0$2
(_50+?100) (|. -: rot) 1

NB. literal
(_50+?100) (|. -: rot) (?45 1 2 2$#a){a=:'foo upon thee 1=?10 20$2'
(_50+?100) (|. -: rot) (?400 1 1$#a){a
(_50+?100) (|. -: rot) (?0 10 2$#a){a
(_50+?100) (|. -: rot) 'a'
(_50+?100) (|. -: rot) ''

NB. literal2
(_50+?100) (|. -: rot) (?45 1 2 2$#a){a=:u:'foo upon thee 1=?10 20$2'
(_50+?100) (|. -: rot) (?400 1 1$#a){a
(_50+?100) (|. -: rot) (?0 10 2$#a){a
(_50+?100) (|. -: rot) u:'a'
(_50+?100) (|. -: rot) u:''

NB. literal4
(_50+?100) (|. -: rot) (?45 1 2 2$#a){a=:10&u:'foo upon thee 1=?10 20$2'
(_50+?100) (|. -: rot) (?400 1 1$#a){a
(_50+?100) (|. -: rot) (?0 10 2$#a){a
(_50+?100) (|. -: rot) 10&u:'a'
(_50+?100) (|. -: rot) 10&u:''

NB. symbol
(_50+?100) (|. -: rot) (?45 1 2 2$#a){a=:s:@<"0 'foo upon thee 1=?10 20$2'
(_50+?100) (|. -: rot) (?400 1 1$#a){a
(_50+?100) (|. -: rot) (?0 10 2$#a){a
(_50+?100) (|. -: rot) s:@<"0 'a'
(_50+?100) (|. -: rot) s:''

NB. integer
(_50+?100) (|. -: rot) ?25 2 2 2$212341
(_50+?100) (|. -: rot) ?400 1$123541
(_50+?100) (|. -: rot) ?0 1000$123456
(_50+?100) (|. -: rot) _4
(_50+?100) (|. -: rot) i.0

NB. floating point
(_50+?100) (|. -: rot) o.?35 3 5$212341
(_50+?100) (|. -: rot) o.?1 40$123541
(_50+?100) (|. -: rot) o.?100 0$123456
(_50+?100) (|. -: rot) 3.4
(_50+?100) (|. -: rot) ,_3.4

NB. complex
(_50+?100) (|. -: rot) ^0j1*?30 2 5$21234
(_50+?100) (|. -: rot) ^0j1*?1 256$1235
(_50+?100) (|. -: rot) ^0j1*?127 0$1234
(_50+?100) (|. -: rot) 3j4
(_50+?100) (|. -: rot) 0$3j4

NB. boxed
(_50+?100) (|. -: rot) (?30 2 1 1 1$#x){x=:;:'Cogit, ergo $212 341 CBC News'
(_50+?100) (|. -: rot) (?30 2 1 1 1$#x){x=:(u:&.>) ;:'Cogit, ergo $212 341 CBC News'
(_50+?100) (|. -: rot) (?30 2 1 1 1$#x){x=:(10&u:&.>) ;:'Cogit, ergo $212 341 CBC News'
(_50+?100) (|. -: rot) (?255 1$#x){x
(_50+?100) (|. -: rot) (?0 0$#x){x
(_50+?100) (|. -: rot) <i.2 3
(_50+?100) (|. -: rot) ,<i.2 3

(1 1 1  $'a') -: 1 2 3  |.'a'
(1 1 1  $'a') -: 1 2 3  |.u:'a'
(1 1 1  $'a') -: 1 2 3  |.10&u:'a'
(1 1 1  $s:@<"0 'a') -: 1 2 3  |.s:@<"0 'a'
(1 1 1 1$4  ) -: (?4$10)|.4

x -: '' |. x=: ? 2 3 4$2
x -: '' |. x=: 3 4 5 6 $ 'deipnosophist'
x -: '' |. x=: 3 4 5 6 $ u:'deipnosophist'
x -: '' |. x=: 3 4 5 6 $ 10&u:'deipnosophist'
x -: '' |. x=: 3 4 5 6 $ s:@<"0 'deipnosophist'
x -: '' |. x=: ? 12$1000

'domain error' -: 'abc'|. etx i.4 3 2
'domain error' -: (u:'abc')|. etx i.4 3 2
'domain error' -: (10&u:'abc')|. etx i.4 3 2
'domain error' -: (s:@<"0 'abc')|. etx i.4 3 2
'domain error' -: 3.45 |. etx i.2 3 4
'domain error' -: 3j4  |. etx i.3 4
'domain error' -: (3;4)|. etx i.4 2


NB. x|.y, left rank 1 ---------------------------------------------------

f1 =: 3 : 0
 :
 i =.0 [ n=.#x [ r=.#$y
 while. i<n do.
  y=. (i{x) |."(r-i) y
  i=. >: i
 end.
 y
)  

2   3 (|. -: f1) ?4 5 6$10000
2  _3 (|. -: f1) a.{~?4 5 6$#a.
2  _3 (|. -: f1) adot1{~?4 5 6$#adot1
2  _3 (|. -: f1) adot2{~?4 5 6$#adot2
_2  3 (|. -: f1) o.?4 5$100
_2 _3 (|. -: f1) j./?2 3 4$1000

(?1$10) (|. -: f1) x=:?7 6 5 4 3$10000
(?2$10) (|. -: f1) x
(?3$10) (|. -: f1) x
(?4$10) (|. -: f1) x
(?5$10) (|. -: f1) x

(?1$10) (|."1 _1 -: f1"1 _1) x=:?7 6 5 4 3$10000
(?2$10) (|."1 _1 -: f1"1 _1) x
(?3$10) (|."1 _1 -: f1"1 _1) x
(?4$10) (|."1 _1 -: f1"1 _1) x

'length error' -: 2 3   |. etx i.5
'length error' -: 2 3 4 |. etx 2 3$'a'
'length error' -: 2 3 4 |. etx 2 3$u:'a'
'length error' -: 2 3 4 |. etx 2 3$10&u:'a'
'length error' -: 2 3 4 |. etx 2 3$s:@<"0 'a'


NB. x|.!.f y ------------------------------------------------------------

' abcdef' -: |.!.'' 'abcdefg'
' abcdef' -: |.!.'' u:'abcdefg'
' abcdef' -: |.!.'' 10&u:'abcdefg'
(({.s:''),s:@<"0 'abcdef') -: |.!.'' s:@<"0 'abcdefg'
0 4 5 6   -: |.!.'' 4 5 6 7
(i.0 4 5) -: |.!.'' i.0 4 5

(9,i.2 4)      -: _1 |.!.9   i.3 4
'ito, ergoXXX' -: 3  |.!.'X' 'Cogito, ergo'
'XXXXXXXXXXXX' -: 99 |.!.'X' 'Cogito, ergo'
'XXXXXXXXXXXX' -: _29|.!.'X' 'Cogito, ergo'
'ito, ergoXXX' -: 3  |.!.'X' u:'Cogito, ergo'
'XXXXXXXXXXXX' -: 99 |.!.'X' u:'Cogito, ergo'
'XXXXXXXXXXXX' -: _29|.!.'X' u:'Cogito, ergo'
'ito, ergoXXX' -: 3  |.!.'X' 10&u:'Cogito, ergo'
'XXXXXXXXXXXX' -: 99 |.!.'X' 10&u:'Cogito, ergo'
'XXXXXXXXXXXX' -: _29|.!.'X' 10&u:'Cogito, ergo'
(s:@<"0 'ito, ergoXXX') -: 3  |.!.(s:@<"0 'X') s:@<"0 'Cogito, ergo'
(s:@<"0 'XXXXXXXXXXXX') -: 99 |.!.(s:@<"0 'X') s:@<"0 'Cogito, ergo'
(s:@<"0 'XXXXXXXXXXXX') -: _29|.!.(s:@<"0 'X') s:@<"0 'Cogito, ergo'
(i.0)          -: 9  |.!.99  i.0

(1 1 1$9) -: 1 2 3|.!.9 [4

'domain error' -: 2   |.!.4    etx 'abcdef'
'domain error' -: 2   |.!.4    etx u:'abcdef'
'domain error' -: 2   |.!.4    etx 10&u:'abcdef'
'domain error' -: 2   |.!.4    etx s:@<"0 'abcdef'
'domain error' -: 2 1 |.!.4    etx +&.>i.4 3
'domain error' -: _2  |.!.'a'  etx i.7 3
'domain error' -: 3   |.!.'a'  etx ;:'Cogito, ergo carborundum'
'domain error' -: _2  |.!.(u:'a')  etx i.7 3
'domain error' -: 3   |.!.(u:'a')  etx ;:'Cogito, ergo carborundum'
'domain error' -: _2  |.!.(10&u:'a')  etx i.7 3
'domain error' -: 3   |.!.(10&u:'a')  etx ;:'Cogito, ergo carborundum'
'domain error' -: _2  |.!.(s:@<"0 'a')  etx i.7 3
'domain error' -: 3   |.!.(s:@<"0 'a')  etx s:@<"0&.> ;:'Cogito, ergo carborundum'
'domain error' -: 3   |.!.(s:@<"0 'a')  etx <"0@s: ;:'Cogito, ergo carborundum'
'domain error' -: 9   |.!.(<4) etx 'supercalifragilisticexpialidocious'
'domain error' -: 9   |.!.(<4) etx u:'supercalifragilisticexpialidocious'
'domain error' -: 9   |.!.(<4) etx 10&u:'supercalifragilisticexpialidocious'
'domain error' -: 9   |.!.(<4) etx s:@<"0 'supercalifragilisticexpialidocious'
(o.i.17) -: 0 |.!.(<4) etx o.i.17


NB. x|."r y -------------------------------------------------------------

rot =: 3 : (':'; 'x|.y')

1 2 3 (|."0 1 -: rot"0 1) ?3 7$2
1 2 3 (|."0 1 -: rot"0 1) a.{~?3 7$#a.
1 2 3 (|."0 1 -: rot"0 1) adot1{~?3 7$#adot1
1 2 3 (|."0 1 -: rot"0 1) adot2{~?3 7$#adot2
1 2 3 (|."0 1 -: rot"0 1) sdot0{~?3 7$#sdot0
1 2 3 (|."0 1 -: rot"0 1) ?3 7$1000
1 2 3 (|."0 1 -: rot"0 1) o.?3 7$1000
1 2 3 (|."0 1 -: rot"0 1) j./?2 3 7$1000

_3 (|."0 1 -: rot"0 1) ?3 7$2
_3 (|."0 1 -: rot"0 1) a.{~?3 7$#a.
_3 (|."0 1 -: rot"0 1) adot1{~?3 7$#adot1
_3 (|."0 1 -: rot"0 1) adot2{~?3 7$#adot2
_3 (|."0 1 -: rot"0 1) sdot0{~?3 7$#sdot0
_3 (|."0 1 -: rot"0 1) ?3 7$1000
_3 (|."0 1 -: rot"0 1) o.?3 7$1000
_3 (|."0 1 -: rot"0 1) j./?2 3 7$1000

_2 3 (|."1 2 -: rot"1 2) ?2 3 7$2
_2 3 (|."1 2 -: rot"1 2) a.{~?2 3 7$#a.
_2 3 (|."1 2 -: rot"1 2) adot1{~?2 3 7$#adot1
_2 3 (|."1 2 -: rot"1 2) adot2{~?2 3 7$#adot2
_2 3 (|."1 2 -: rot"1 2) sdot0{~?2 3 7$#sdot0
_2 3 (|."1 2 -: rot"1 2) ?2 3 7$1000
_2 3 (|."1 2 -: rot"1 2) o.?2 3 7$1000
_2 3 (|."1 2 -: rot"1 2) j./?2 2 3 7$1000

1 2 3 (|."0 _ -: rot"0 _) ?20$10000
(2 3,:4 5) (|."1 2 -: rot"1 2) ?2 3 4$1000
(2 3,:4 5) (|. -: rot) ?7 9$1000
(?2 3 2$10) (|."2 -: rot"2) ?2 3 4$1000


NB. x|.!.f"r y ----------------------------------------------------------

4!:55 ;:'f rot'
4!:55 ;:'f rot'

rot =: 3 : (':'; 'x|.!.f y')

1 2 3 (|.!.f"0 1 -: rot"0 1) ?3 7$2          [ f=:0
1 2 3 (|.!.f"0 1 -: rot"0 1) a.{~?3 7$#a.    [ f=:'*'
1 2 3 (|.!.f"0 1 -: rot"0 1) adot1{~?3 7$#adot1    [ f=:'*'
1 2 3 (|.!.f"0 1 -: rot"0 1) adot2{~?3 7$#adot2    [ f=:'*'
1 2 3 (|.!.f"0 1 -: rot"0 1) sdot0{~?3 7$#sdot0     [ f=:s:@<"0 '*'
1 2 3 (|.!.f"0 1 -: rot"0 1) ?3 7$1000       [ f=:1
1 2 3 (|.!.f"0 1 -: rot"0 1) o.?3 7$1000     [ f=:3.56
1 2 3 (|.!.f"0 1 -: rot"0 1) j./?2 3 7$1000  [ f=:3j4

_3 (|.!.f"0 1 -: rot"0 1) ?3 7$2             [ f=:2
_3 (|.!.f"0 1 -: rot"0 1) a.{~?3 7$#a.       [ f=:'@'
_3 (|.!.f"0 1 -: rot"0 1) adot1{~?3 7$#adot1       [ f=:'@'
_3 (|.!.f"0 1 -: rot"0 1) adot2{~?3 7$#adot2       [ f=:'@'
_3 (|.!.f"0 1 -: rot"0 1) sdot0{~?3 7$#sdot0       [ f=:s:@<"0 '@'
_3 (|.!.f"0 1 -: rot"0 1) ?3 7$1000          [ f=:3.4
_3 (|.!.f"0 1 -: rot"0 1) o.?3 7$1000        [ f=:0
_3 (|.!.f"0 1 -: rot"0 1) j./?2 3 7$1000     [ f=:2.5

_2 3 (|.!.f"1 2 -: rot"1 2) ?2 3 7$2         [ f=:3j4
_2 3 (|.!.f"1 2 -: rot"1 2) a.{~?2 3 7$#a.   [ f=:' '
_2 3 (|.!.f"1 2 -: rot"1 2) adot1{~?2 3 7$#adot1   [ f=:' '
_2 3 (|.!.f"1 2 -: rot"1 2) adot2{~?2 3 7$#adot2   [ f=:' '
_2 3 (|.!.f"1 2 -: rot"1 2) sdot0{~?2 3 7$#sdot0   [ f=:s:@<"0 ' '
_2 3 (|.!.f"1 2 -: rot"1 2) ?2 3 7$1000      [ f=:999
_2 3 (|.!.f"1 2 -: rot"1 2) o.?2 3 7$1000    [ f=:3j4
_2 3 (|.!.f"1 2 -: rot"1 2) j./?2 2 3 7$1000 [ f=:0

1 2 3 (|.!.f"0 _ -: rot"0 _) ?20$10000       [ f=:9

4!:55 ;:'a adot1 adot2 sdot0 f f1 rank rev rot x'
randfini''


epilog''

