NB. 3!:0 ----------------------------------------------------------------

NB. Boolean
1 = type 0
1 = type 1
1 = type 1 0 1 0 0
1 = type 3 = 4 5
1 = type (<'ergo')e.;:'Cogito, ergo sum.'

NB. literal
2 = type a.
2 = type 'abc'
2 = type ''
2 = type ":1 2 3

NB. integer
4 = type 4 5 6
4 = type 1e8 1e9
4 = type i.3 4
4 = type 12+13
4 = type *_1.5 2 3.1415
4 = type a.i.'abcd'

NB. floating Point
8 = type 1.5 2.3 _6.3234
8 = type 1e19
8 = type 3%4

NB. complex
16 = type 0j1 3.5j_6
16 = type %:-i.12
16 = type ^.->:i.12

NB. boxed
32 = type (<'abc'),<'bar'
32 = type ;:'Cogito, ergo sum.'
32 = type 0$<''

NB. extended integer
64 = type 10x
64 = type !?3 4$10x
64 = type A. 20?20

NB. rational
128 = type 3r5
128 = type %/?2 3 4$100x


NB. 3!:1, 3!:2, and 3!:3 ------------------------------------------------

binrep=: 3!:1
unbin =: 3!:2
hexrep=: 3!:3

f=: 1 : 0  NB. miscellaneous arrays
 test=: (-: unbin@(0&u)) , (-: unbin@(1&u)) , (-: unbin@(10&u)) , (-: unbin@(11&u)) , (-: unbin@u)
 assert. test x=: ?20$2
 assert. test x=: a.
 assert. test x=: a.{~?2 34$#a.
 assert. test x=: _4e5+?100$8e5
 assert. test x=: o._4e5+?100$8e5
 assert. test x=: j./_500+?2 50$1000
 assert. test x=: _ __
 1 [ y
)

binrep f 1
hexrep f 1

f=: 1 : 0  NB. arrays with varying ranks
 test=: (-: unbin@(0&u)) , (-: unbin@(1&u)) , (-: unbin@(10&u)) , (-: unbin@(11&u)) , (-: unbin@u)
 assert. test x=: ?1000
 assert. test x=: ?3$1000
 assert. test x=: ?3 5$1000
 assert. test x=: ?3 5 7$1000
 assert. test x=: ?3 5 7 11$1000
 assert. test x=: ?3 5 7 11 13$1000
 1 [ y
)

binrep f 1
hexrep f 1

f=: 1 : 0  NB. empty arrays
 test=: (-: unbin@(0&u)) , (-: unbin@(1&u)) , (-: unbin@(10&u)) , (-: unbin@(11&u)) , (-: unbin@u)
 assert. test x=: ?0$1e6
 assert. test x=: ?3 0$1e6
 assert. test x=: ?3 5 0$1e6
 assert. test x=: ?3 5 7 0$1e6
 assert. test x=: ?3 5 7 11 0$1e6
 assert. test x=: ?3 5 7 11 13 0$1e6
 assert. test x=: (?20$2)$0
 assert. test x=: (?1 2 4#4 3 2)$'a'
 assert. test x=: (?10$!.2[3 4 5)$3.4
 assert. test x=: (i.30)$3j4
 1 [ y
)

binrep f 1
hexrep f 1

f=: 1 : 0  NB. extended integer and rational
 test=: (-: unbin@(0&u)) , (-: unbin@(1&u)) , (-: unbin@(10&u)) , (-: unbin@(11&u)) , (-: unbin@u)
 assert. test x=: ?1000x
 assert. test x=: ?3$1000x
 assert. test x=: ?3 5$1000x
 assert. test x=: ?3 5 7$1000x
 assert. test x=: ?3 5 7 11$1000x
 assert. test x=: ?3 5 7 11 13$1000x
 assert. test x=: !?20x
 assert. test x=: !?2$20x
 assert. test x=: !?2 3$20x
 assert. test x=: !?2 3 5$20x
 assert. test x=: !?2 3 5 7$20x
 assert. test x=: !?2 3 5 7 11$20x
 assert. test x=: %/?2$20x
 assert. test x=: %/?2 11$20x
 assert. test x=: %/?2 11 7$20x
 assert. test x=: %/?2 11 7 5$20x
 assert. test x=: %/?2 11 7 5 3$20x
 assert. test x=: %/?2 11 7 5 3 2$20x
 1 [ y
)

binrep f 1
hexrep f 1

f=: 1 : 0  NB. boxed arrays
 test=: (-: unbin@(0&u)) , (-: unbin@(1&u)) , (-: unbin@(10&u)) , (-: unbin@(11&u)) , (-: unbin@u)
 assert. test x=: 5!:1 <'test'
 assert. test x=: t{~?3       $#t=. (5!:1 <'test');;:'Cogito, ergo sum. jarl is the root of earl'
 assert. test x=: t{~?3 5     $#t
 assert. test x=: t{~?3 5 7   $#t
 assert. test x=: t{~?3 5 7 11$#t
 assert. test x=: t{~?3       $#t=. (?20$2);(?2 3 4$1e6);(o.?2 3$1e6);(j./?2 3 4$1e6);(x:?17$1e6);%/?2 3 4 5$100000x
 assert. test x=: t{~?3 5     $#t
 assert. test x=: t{~?3 5 7   $#t
 assert. test x=: t{~?3 5 7 11$#t
 assert. test x=: <0
 assert. test x=: <<<<<<0
 assert. test x=: +&.>   ?3 4 5$20
 assert. test x=: +&.>o. ?3 4 5$20
 assert. test x=: <"0 j./?2 3 4$5
 assert. test x=: (1=?100$10) <;.1 ?100$2
 assert. test x=: (1=?100$10) <;.1 (?100$#a.){a.
 assert. test x=: (1=?100$10) <;.1 ?100$1000
 assert. test x=: (1=?100$20) <;.1 o.?100$10000
 assert. test x=: (1=?100$20) <;.1 j./?2 100$5
 assert. test x=: 23$<'the same'
 assert. test x=: 2 3$<'the same'
 assert. test x=: 3 7 5$<'the same'
 assert. test x=: 5!:1 <'test'
 assert. test x=: 5!:2 <'test'
 g=: test f.
 assert. test x=: 5!:1 <'g'
 assert. test x=: 5!:2 <'g'
 g=: +/ % #
 assert. test x=: 5!:1 <'g'
 assert. test x=: 5!:2 <'g'
 g=: ~.@q:@%@(-/ .*)@Hilbert
 assert. test x=: 5!:1 <'g' 
 assert. test x=: 5!:2 <'g'
 1 [ y
)

binrep f 1
hexrep f 1

test =: 3 : 0
 p=. binrep y
 q=. hexrep y
 (2=#$q), (*./(,q)e.'0123456789abcdef'), (#,q)=+:#p
)

test ?20$2
test a.
test (?2 3 4$#a.){a.
test _4e5+?100$8e5
test o._4e5+?100$8e5
test j./_500+?2 100$1000
test ?4 25$1000x
test %/?2 4 25$100x

test (?20$2)$0
test (?1 2 4#4 3 2)$'a'
test (?10$!.2[3 4 5)$3.4
test (i.30)$3j4

'3!:2' -: 3!:1 b. _1
'3!:1' -: 3!:2 b. _1
'3!:2' -: 3!:3 b. _1
'3!:2' -: 0&(3!:1) b. _1
'3!:2' -: 1&(3!:1) b. _1
'3!:2' -: 0&(3!:3) b. _1
'3!:2' -: 1&(3!:3) b. _1

(-: ]&.( 0&(3!:1))) x=: ?2000$2
(-: ]&.( 0&(3!:1))) x=: ?29 7$2
(-: ]&.( 0&(3!:1))) x=: a.{~?2000$#a.
(-: ]&.( 0&(3!:1))) x=: a.{~?29 7$#a.
(-: ]&.( 0&(3!:1))) x=: ?2000$2e6
(-: ]&.( 0&(3!:1))) x=: ?29 7$2e6
(-: ]&.( 0&(3!:1))) x=: o.?2000$2e6
(-: ]&.( 0&(3!:1))) x=: o.?29 7$2e6
(-: ]&.( 0&(3!:1))) x=: j./?2 2000$2e6
(-: ]&.( 0&(3!:1))) x=: j./?2 29 7$2e6
(-: ]&.( 0&(3!:1))) x=: ;:'bou stro phe don ic'
(-: ]&.( 0&(3!:1))) x=: 3 41$;:'4 score and 7 years ago, +/ . *'

(-: ]&.( 1&(3!:1))) x=: ?2000$2
(-: ]&.( 1&(3!:1))) x=: ?29 7$2
(-: ]&.( 1&(3!:1))) x=: a.{~?2000$#a.
(-: ]&.( 1&(3!:1))) x=: a.{~?29 7$#a.
(-: ]&.( 1&(3!:1))) x=: ?2000$2e6
(-: ]&.( 1&(3!:1))) x=: ?29 7$2e6
(-: ]&.( 1&(3!:1))) x=: o.?2000$2e6
(-: ]&.( 1&(3!:1))) x=: o.?29 7$2e6
(-: ]&.( 1&(3!:1))) x=: j./?2 2000$2e6
(-: ]&.( 1&(3!:1))) x=: j./?2 29 7$2e6
(-: ]&.( 1&(3!:1))) x=: ;:'bou stro phe don ic'
(-: ]&.( 1&(3!:1))) x=: 3 41$;:'4 score and 7 years ago, +/ . *'

(-: ]&.(10&(3!:1))) x=: ?2000$2
(-: ]&.(10&(3!:1))) x=: ?29 7$2
(-: ]&.(10&(3!:1))) x=: a.{~?2000$#a.
(-: ]&.(10&(3!:1))) x=: a.{~?29 7$#a.
(-: ]&.(10&(3!:1))) x=: ?2000$2e6
(-: ]&.(10&(3!:1))) x=: ?29 7$2e6
(-: ]&.(10&(3!:1))) x=: o.?2000$2e6
(-: ]&.(10&(3!:1))) x=: o.?29 7$2e6
(-: ]&.(10&(3!:1))) x=: j./?2 2000$2e6
(-: ]&.(10&(3!:1))) x=: j./?2 29 7$2e6
(-: ]&.(10&(3!:1))) x=: ;:'bou stro phe don ic'
(-: ]&.(10&(3!:1))) x=: 3 41$;:'4 score and 7 years ago, +/ . *'

(-: ]&.(11&(3!:1))) x=: ?2000$2
(-: ]&.(11&(3!:1))) x=: ?29 7$2
(-: ]&.(11&(3!:1))) x=: a.{~?2000$#a.
(-: ]&.(11&(3!:1))) x=: a.{~?29 7$#a.
(-: ]&.(11&(3!:1))) x=: ?2000$2e6
(-: ]&.(11&(3!:1))) x=: ?29 7$2e6
(-: ]&.(11&(3!:1))) x=: o.?2000$2e6
(-: ]&.(11&(3!:1))) x=: o.?29 7$2e6
(-: ]&.(11&(3!:1))) x=: j./?2 2000$2e6
(-: ]&.(11&(3!:1))) x=: j./?2 29 7$2e6
(-: ]&.(11&(3!:1))) x=: ;:'bou stro phe don ic'
(-: ]&.(11&(3!:1))) x=: 3 41$;:'4 score and 7 years ago, +/ . *'

NB. empty array type
x=: 0 1 0;'abc';3;4.5 6;7j8 9 10;(<1 2 3;4.5);1 2 3x;3r4 5r6
(type&.>x) -: type&.> 3!:2@(3!:1) 0#&.>x
(type&.>x) -: type&.> 3!:2@(3!:3) 0#&.>x

'domain error' -: unbin etx ?36$2
'domain error' -: unbin etx ?20$10
'domain error' -: unbin etx -:?20$10
'domain error' -: unbin etx r.?20$10
'domain error' -: unbin etx 'g' (<8;2)}hexrep ?10$10
'domain error' -: unbin etx '00000005' 0}0 hexrep ?10$10
'domain error' -: ex '_1&(3!:1) ^: _1 ] 9'
'domain error' -: ex ' 2&(3!:1) ^: _1 ] 9'
'domain error' -: ex '_1&(3!:3) ^: _1 ] 9'
'domain error' -: ex ' 2&(3!:3) ^: _1 ] 9'
'domain error' -: ex '(3!:1)&0  ^: _1 ] 9'
'domain error' -: ex '(3!:1)&1  ^: _1 ] 9'
'domain error' -: ex '(3!:3)&0  ^: _1 ] 9'
'domain error' -: ex '(3!:3)&1  ^: _1 ] 9'

'domain error' -: unbin etx '02' (<_2;0 1)}3!:3 x=: ?8$2
'domain error' -: unbin etx '10' (<_2;6 7)}3!:3 x=: ?8$2

'rank error'   -: unbin etx 'f'
'rank error'   -: unbin etx 5 3$hexrep i.4
('rank error';'domain error') e.~ <unbin etx 'f' 2}0 hexrep ?4$10
('rank error';'domain error') e.~ <unbin etx 'f' 2}1 hexrep ?4$10
'rank error'   -: ex '0 1&(3!:1) ^: _1 ] 9'
'rank error'   -: ex '0 1&(3!:3) ^: _1 ] 9'

'index error'  -: unbin etx ( 7{a.) (>IF64{(20+i.4);40+i.8)}x=: binrep ;:'Cogito, ergo sum.'
'index error'  -: unbin etx (_1{a.) (>IF64{(24+i.4);48+i.8)}x
'index error'  -: unbin etx ( 7{a.) (>IF64{(28+i.4);56+i.8)}x
'index error'  -: unbin etx (_1{a.) (>IF64{(32+i.4);64+i.8)}x
'index error'  -: unbin etx '7' 5}x=: hexrep ;:'Cogito, ergo sum.'
'index error'  -: unbin etx 'f' 6}x
'index error'  -: unbin etx '7' 7}x
'index error'  -: unbin etx 'f' 8}x

'length error' -: unbin etx ''
'length error' -: unbin etx _5}.binrep ?20$2
'length error' -: unbin etx _5}.binrep 4 5$'sesquipedalian'
'length error' -: unbin etx _5}.binrep 3 4 5

NB. 3!:2 decoding pre-601 data ------------------------------------------

Indirect=: 32 64 128,<.2^10+i.6  NB. indirect types

bhdr_pre601=: 4 : 0
 b=. (1<x){4 8
 t=. 256 #. a.i. |.^:(2|x) (b+i.b){y
 y=. ((b+i.b){y),(b#(1<x){0 _1{a.),(b+b)}.y
 if. t e. Indirect do.
  r=. 256 #. a.i. |.^:(2|x) y{~(3*b)+i.b
  if. 1024<:t do.
   j=. 5+r [ n=. 4
  else.
   j=. 4+r [ n=. +:^:(t=128) 256 #. a.i. |.^:(2|x) ((2*b)+i.b){y
  end.
  if. n do.
   i=. 256 #. a.i. |."1^:(2|x) y{~(b*j+i.n)+/i.b
   y=. (({.i){.y) , ; x&bhdr_pre601&.> ((i.#y) e. i) <;.1 y
 end. end.
 y
)

binrep_pre601=: [ bhdr_pre601 (3!:1) 

test=: 3 : 0
 assert. (-: unbin@( 0&binrep_pre601)) y
 assert. (-: unbin@( 1&binrep_pre601)) y
 assert. (-: unbin@(10&binrep_pre601)) y
 assert. (-: unbin@(11&binrep_pre601)) y
 1
)
 
test y=: i.2 3
test y=: o. i.2 3
test y=: ^j.i.2 3
test y=: ;:'Cogito, ergo sum.'
test y=: 0$<'abcde'
test y=: <''
test y=: {:: <''
test y=: 5!:1 <'binrep_pre601'
test y=: {:: 5!:1 <'binrep_pre601'
test y=: !i.10x
test y=: % >: +/~ i.4x
test y=: $. (3 4 ?@$ 2) * 3 4 5 ?@$ 1e5


NB. 3!:n exact representation -------------------------------------------

NB. xr - exact representation
NB. rx - left inverse to xr
NB. rx xr y  exactly reproduces array y on the same byte-order 32-bit machine

br  =: 3!:1
rb  =: 3!:2
WS  =: IF64{4 8               NB. word size
H   =: (17*WS) -~ # br i.17   NB. # of header bytes in binary representation
hbr =: H&}. @ br @ ,          NB. headerless binary representation
xrh =: [: hbr #@$ , $

xr=: 3 : 0
 t=. 3!:0 y
 if.     t e. 8 16 do. (t{a.),(xrh y),hbr y
 elseif. 32~:t     do. (t{a.),5!:5 <'y'
 elseif. 1         do. (t{a.),(xrh y),(hbr@:(#&>) , ;) xr&.>,y
 end.
)

rbi =: 3 : 0
 rb y ,~ (-#y) }. br (WS%~#y)$7
)

hrx =: 3 : 0
 r=. rbi WS    {.1     }.y
 s=. rbi (r*WS){.(1+WS)}.y
 r;s
)

rx=: 3 : 0
 t=. a.i.{.y
 if. -. t e. 8 16 32 do. ". }.y return. end.   NB. other
 'r s'=. hrx y
 h=. 1+WS*1+r
 if.     8 =t do. rb ((h-#y)}.br s$0.1),h}.y  NB. real
 elseif. 16=t do. rb ((h-#y)}.br s$0j1),h}.y  NB. complex
 elseif. 1    do.                             NB. boxed
  n=. */s,WS
  c=. rbi n{.h}.y
  s $ rx&.> ((i.+/c) e. 0,+/\c) <;.1 (n+h)}.y
 end.
)

(-:!.0 rx@xr) x=: ?2 3 4 5$2
(-:!.0 rx@xr) x=: a.{~ ?1000$#a.
(-:!.0 rx@xr) x=: ?2 3  $1e9
(-:!.0 rx@xr) x=: ?2 3 4$1e9
(-:!.0 rx@xr) 0.07
(-:!.0 rx@xr) o.1
(-:!.0 rx@xr) _ __
(-:!.0 rx@xr) j./o.?2 3 4$1e6
(-:!.0 rx@xr) ;:'Cogito, ergo sum.'
(-:!.0 rx@xr) 0.07 ; (j./i.2 3 4) ; ,. ;:'Cogito, ergo sum.'
(-:!.0 rx@xr) !100x
(-:!.0 rx@xr) +/ .*~^:(10) 2 2$0 1 1 1x
(-:!.0 rx@xr) (+%)/\44$1x
(-:!.0 rx@xr) 5!:1 <'xrh'


4!:55 ;:'bhdr_pre601 binrep binrep_pre601 br f g H hbr hexrep hrx Indirect'
4!:55 ;:'rb rbi rx test unbin WS x xr xrh y'


