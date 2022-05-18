prolog './gesc.ijs'
NB. e. special code -----------------------------------------------------

E=: 1 : 0
:
 xx=: x {~ 500 ?@$ #x  
 yy=: y {~1000 ?@$ #y
 uu =: u
 '`f g h'=: >1{>5!:1 <'u'
 if. '[:' -: 5!:5 <'f' do.
  assert. (xx u yy   ) -: g xx h yy
  assert. (u f.&yy xx) -: g xx h yy
 else.
  assert. (xx u yy   ) -: (xx f yy) g (xx h yy)
  assert. (u f.&yy xx) -: (xx f yy) g (xx h yy)
 end.
 1
)

A0=: 1 : 0
 u E ~ a=. 0 1
 u E ~ a=. #:i.2^1
 u E ~ a=. #:i.2^2
 u E ~ a=. #:i.2^3
 u E ~ a=. #:i.2^4
 u E ~ a=. #:i.2^5
 u E ~ a=. #:i.2^6
 u E ~ a=. #:i.2^7
 u E ~ a=. #:i.2^8
 u E ~ a=. #:i.2^9
 u E ~ a=. a.
 u E ~ a=. a.{~ 600 1 ?@$ 256
 u E ~ a=. a.{~ 600 2 ?@$ 256
 u E ~ a=. a.{~ 600 3 ?@$ 256
 u E ~ a=. a.{~ 600 4 ?@$ 256
 u E ~ a=. a.{~ 600 5 ?@$ 256
 u E ~ a=. a.{~ 600 6 ?@$ 256
 u E ~ a=. a.{~ 600 7 ?@$ 256
 u E ~ a=. a.{~ 600 8 ?@$ 256
 u E ~ a=. a.{~ 600 9 ?@$ 256
 u E ~ a=. 600   ?@$ 2e9
 u E ~ a=. 600 2 ?@$ 2e9
 u E ~ a=. 600   ?@$ 600
 u E ~ a=. 600 2 ?@$ 600
 u E ~ a=. _300+600 ?@$ 600
 u E ~ a=. _5e7+600 ?@$ 1e8
 u E ~ a=. 600   ?@$ 0
 u E ~ a=. 600 2 ?@$ 0
 u E ~ a=. j./ 2 600   ?@$ 0
 u E ~ a=. j./ 2 600 2 ?@$ 0
 u E ~ a=. u: 600   ?@$ 65536
 u E ~ a=. u: 600 2 ?@$ 65536
 u E ~ a=. 10&u: RAND32 600   ?@$ C4MAX
 u E ~ a=. 10&u: RAND32 600 2 ?@$ C4MAX
 u E ~ a=. (1;2 3;4 5;;:'foo upon thee'),":&.> 600 ?@$ 1000
 u E ~ a=. (1;2 3;4 5;(u:&.>) ;:'foo upon thee'),":&.> 600 ?@$ 1000
 u E ~ a=. (1;2 3;4 5;(10&u:&.>) ;:'foo upon thee'),":&.> 600 ?@$ 1000
 u E ~ a=. a{~600 2 ?@$ #a=.(1;2 3;4 5;;:'foo upon thee'),":&.> 20 ?@$ 1000
 u E ~ a=. a{~600 2 ?@$ #a=.(1;2 3;4 5;(u:&.>) ;:'foo upon thee'),":&.> 20 ?@$ 1000
 u E ~ a=. a{~600 2 ?@$ #a=.(1;2 3;4 5;(10&u:&.>) ;:'foo upon thee'),":&.> 20 ?@$ 1000
 u E ~ a=. s: ' cogito ergo sum kakistocracy foo upon thee ',": 600 ?@$ 1000
 u E ~ a=. s: u: 128+a.i. ' cogito ergo sum kakistocracy foo upon thee ',": 600 ?@$ 1000
 u E ~ a=. s: 10&u: 65536+a.i. ' cogito ergo sum kakistocracy foo upon thee ',": 600 ?@$ 1000
 u E ~ a=. a{~600 2 ?@$ #a=. s: ' cogito ergo sum ',": 600 ?@$ 1000
 u E ~ a=. a{~600 2 ?@$ #a=. s: u: 128+a.i. ' cogito ergo sum ',": 600 ?@$ 1000
 u E ~ a=. a{~600 2 ?@$ #a=. s: 10&u: 65536+a.i. ' cogito ergo sum ',": 600 ?@$ 1000
 u E ~ a=. x: 600   ?@$ IF64{2e9 9e18
 u E ~ a=. x: 600 2 ?@$ 1000
 u E ~ a=. %/x: 0 1+2 600   ?@$ IF64{2e9 9e18
 u E ~ a=. %/x: 0 1+2 600 2 ?@$ 1000
 u E ~ a=. 4 0$0
 u E ~ a=. 4 0$'a'
 u E ~ a=. 4 0$u:'a'
 u E ~ a=. 4 0$10&u:'a'
 u E ~ a=. 4 0$100
 u E ~ a=. 4 0$0.5
 u E ~ a=. 4 0$3j4
 u E ~ a=. 4 0$a:
 u E ~ a=. 4 0$u: 123
 u E ~ a=. 4 0$10&u: 123
 u E ~ a=. 4 0$s: ' cogito'
 u E ~ a=. 4 0$s: u: 128+a.i. ' cogito'
 u E ~ a=. 4 0$s: 10&u: 65536+a.i. ' cogito'
 u E ~ a=. 4 0$3x
 u E ~ a=. 4 0$3r4
)

(e. i.  0:) A0
(e. i.  1:) A0
(e. i:  0:) A0
(e. i:  1:) A0
([: + / e.) A0
([: +./ e.) A0
([: *./ e.) A0
([: I.  e.) A0

A1=: 4 : 0
 x (e.i.0:)  E y
 x (e.i.1:)  E y
 x (e.i:0:)  E y
 x (e.i:1:)  E y
 x ([:+ /e.) E y
 x ([:+./e.) E y
 x ([:*./e.) E y
 x ([:I. e.) E y
)

A1~ 3 4 0 5$0
A1~ 3 4 0 5$a.
A1~ 3 4 0 5$-~2
A1~ 3 4 0 5$-~3.4
A1~ 3 4 0 5$-~3j4

(600 ?@$ 1000) A1   600 ?@$ 1e4
(600 ?@$ 1000) A1 ~ 600 ?@$ 1e4
(600 ?@$ 1000) A1   600 ?@$ 2e9
(600 ?@$ 1000) A1 ~ 600 ?@$ 2e9
(600 ?@$ 1000) A1   (o.0)+600 ?@$ 1000
(600 ?@$ 1000) A1 ~ (o.0)+600 ?@$ 1000

(a.{~600 1 ?@$  256) A1   600 1 ?@$ 2
(a.{~600 1 ?@$  256) A1 ~ 600 1 ?@$ 2
(a.{~600 2 ?@$  256) A1   600 2 ?@$ 2
(a.{~600 2 ?@$  256) A1 ~ 600 2 ?@$ 2
(a.{~600 4 ?@$  256) A1   600 4 ?@$ 2
(a.{~600 4 ?@$  256) A1 ~ 600 4 ?@$ 2

4!:55 ;:'A0 A1 E f g h uu xx yy'




epilog''

