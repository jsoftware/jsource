prolog './g100.ijs'
NB. +y ------------------------------------------------------------------

t -: +t=.1=?100$2
t -: +t=.  _1e9+?100$2e9
t -: +t=.o.1e_9+?100$2e9

f    =. |@*~ % ]
test =. f -: +
test j./_1e9+?2 100$2e9
test r./_1e7+?2 100$2e7
test 0 3j4

3j_4 -: +3j4

'domain error' -: + etx  'abc'
'domain error' -: + etx <'abc'
'domain error' -: + etx  u:'abc'
'domain error' -: + etx <u:'abc'
'domain error' -: + etx  10&u:'abc'
'domain error' -: + etx <10&u:'abc'
'domain error' -: + etx  s:@<"0 'abc'
'domain error' -: + etx s:@<"0&.> <'abc'
'domain error' -: + etx <"0@s: <'abc'
'domain error' -: + etx <3 4 5


NB. x+y -----------------------------------------------------------------

4 = type 5 + ''
4 = type '' + 5
1 = type 'a' +. (0$0)
4 = type 'a' + (0$0)
1 = type (u:'a') +. (0$0)
4 = type (u:'a') + (0$0)
1 = type (10&u:'a') +. (0$0)
4 = type (10&u:'a') + (0$0)
1 = type (s:@<"0 'a') +. (0$0)
4 = type (s:@<"0 'a') + (0$0)
8 = type '' + 5.5
16 = type '' + 3j1
1 = type '' +. 'c'
4 = type '' + 'c'
1 = type '' +. u:'c'
4 = type '' + u:'c'
1 = type '' +. 10&u:'c'
4 = type '' + 10&u:'c'
1 = type '' +. s:@<"0 'c'
4 = type '' + s:@<"0 'c'
'length error' -: '' + etx 1000 1000 $ 'abc'
'length error' -: '' + etx 1000 1000 $ u:'abc'
'length error' -: '' + etx 1000 1000 $ 10&u:'abc'
'length error' -: '' + etx 1000 1000 $ s:@<"0 'abc'
 
 
4 = type 1234+5678
4 = type _1234+_5678
4 = type 2e9
4 = type _2e9
4 = type 2147483647

(IF64{8 4) = type 2e9 +3e8
(IF64{8 4) = type 2e15+3e15
(IF64{8 4) = type _2e9 +_3e8
(IF64{8 4) = type _2e15+_3e15
(IF64{8 4) = type 2147483647+1

x=: - y=: 1+i.100
(x + <._1+2^IF64{31 63x) -: x + <._1+2^IF64{31 63
(y + <.  -2^IF64{31 63x) -: y + <.  -2^IF64{31 63

( 2e8*>:i.20) -: +/\20$ 2e8
(_2e8*>:i.20) -: +/\20$_2e8

4 -: 2 + 2
3 -: 7 + _4

2147483648  -:  1+ 2147483647
_2147483649 -: _1+_2147483648

t -: 3+t=.i.?27$2
t -: (t=.i.?27$2)+4
t -: (i.3 4)+t=.i.3 4,?27$2
t -: (t=.i.3 4,?27$2)+i.3 4


NB. complex numbers -----------------------------------------------------

16 -: type 3j4 5j6

(2 3$1 0) -: =3j4 5j6 3j4
1 0 1 -: 3j4 = 3j4 3j_4 3j4
a=.3j4
3j4 -: a
b=:3j4
3j4 -: b

(3j4;5j6;7j8) -: (<3j4),(<5j6),<7j8
0 0j1 1 1 -: <. 0.2j0.2 0.2j0.8 0.8j0.2 0.8j0.8
2j4 -: <:3j4

3j4 -: ><3j4
(<.&.-t) -: >.t=.0.2j0.2 0.2j0.8 0.8j0.2 0.8j0.8
4j4 -: >:3j4

3j_4 -: +3j4
8j10 -: 3j4+5j6
3 4 -: +.3j4
1j1 -: 5j11 +. 3j7

NB. Errors

'length error' -: 'abc' + etx 4 5  NB. Agreement before domain
'domain error' -: 'abc' + etx 4
'domain error' -: 'abc' +~etx 4
'domain error' -: 4     + etx <'abc'
'domain error' -: 4     +~etx <'abc'
'domain error' -: 'j'   + etx <'abc'
'domain error' -: 'j'   +~etx <'abc'
'length error' -: (u:'abc') + etx 4 5  NB. Agreement before domain
'domain error' -: (u:'abc') + etx 4
'domain error' -: (u:'abc') +~etx 4
'domain error' -: 4     + etx <u:'abc'
'domain error' -: 4     +~etx <u:'abc'
'domain error' -: 'j'   + etx <u:'abc'
'domain error' -: 'j'   +~etx <u:'abc'
'length error' -: (10&u:'abc') + etx 4 5  NB. Agreement before domain
'domain error' -: (10&u:'abc') + etx 4
'domain error' -: (10&u:'abc') +~etx 4
'domain error' -: 4     + etx <10&u:'abc'
'domain error' -: 4     +~etx <10&u:'abc'
'domain error' -: 'j'   + etx <10&u:'abc'
'domain error' -: 'j'   +~etx <10&u:'abc'
'length error' -: (s:@<"0 'abc') + etx 4 5  NB. Agreement before domain
'domain error' -: (s:@<"0 'abc') + etx 4
'domain error' -: (s:@<"0 'abc') +~etx 4
'domain error' -: 4     + etx s:@<"0&.> <'abc'
'domain error' -: 4     + etx <"0@s: <'abc'
'domain error' -: 4     +~etx s:@<"0&.> <'abc'
'domain error' -: 4     +~etx <"0@s: <'abc'
'domain error' -: 'j'   + etx s:@<"0&.> <'abc'
'domain error' -: 'j'   + etx <"0@s: <'abc'
'domain error' -: 'j'   +~etx s:@<"0&.> <'abc'
'domain error' -: 'j'   +~etx <"0@s: <'abc'

'length error' -: 3 4   + etx 5 6 7
'length error' -: 3 4   +~etx 5 6 7
'length error' -: 3 4   + etx i.5 6
'length error' -: 3 4   +~etx i.5 6
'length error' -: 3 4 5 + etx i.4 3
'length error' -: 3 4 5 +~etx i.4 3

NB. not instated yet (0$0) -: (i. 0 3) +"1 (i. 0 2)  NB. No error - 0 used for cell result
NB. not instated yet (0$0) -: (i. 0 3) +"1 2 (i. 0 2 2)  NB. No error - 0 used for cell result
NB. not instated yet (4 0$0) -: (i. 4 0 3) +"1 2 (i. 4 2 2)  NB. No error - 0 used for cell result
'length error' -: (i. 0 3) +"1 etx (i. 0 2)
'length error' -: (i. 0 3) +"1 2 etx (i. 0 2 2)
'length error' -: (i. 4 0 3) +"1 2 etx (i. 4 2 2)
'length error' -: (i. 4 0 3) +"1 2 etx (i. 1 2 2)
'length error' -: (i. 4 0 3) +"1 2 etx (i. 0 2 2)


NB. x+y integer overflow handling ---------------------------------------

test  =. + 0&=@:- +&((o.0)&+)
testa =. (+/   0&=@:- +/  @((o.0)&+))@,"0
testb =. (+/\  0&=@:- +/\ @((o.0)&+))@,"0
testc =. (+/\. 0&=@:- +/\.@((o.0)&+))@,"0
x     =: <._1+2^IF64{31 62
y     =: <.  -2^IF64{31 62

4 = type&> _1 0 +&.> x
4 = type&> _1 0 +&.>~x
4 = type&>  1 0 +&.> y
4 = type&>  1 0 +&.>~y

4 = type&> _1 0 +/@,&.> x
4 = type&> _1 0 +/@,&.>~x
4 = type&>  1 0 +/@,&.> y
4 = type&>  1 0 +/@,&.>~y

4 = type&> _1 0 +/\@,&.> x
4 = type&> _1 0 +/\@,&.>~x
4 = type&>  1 0 +/\@,&.> y
4 = type&>  1 0 +/\@,&.>~y

4 = type&> _1 0 +/\.@,&.> x
4 = type&> _1 0 +/\.@,&.>~x
4 = type&>  1 0 +/\.@,&.> y
4 = type&>  1 0 +/\.@,&.>~y

,(2e9 2e9 _2e9 _2e9) test &>/ 1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) test &>/~1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) testa&>/ 1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) testa&>/~1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) testb&>/ 1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) testb&>/~1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) testc&>/ 1e9 _1e9 1e9 _1e9
,(2e9 2e9 _2e9 _2e9) testc&>/~1e9 _1e9 1e9 _1e9

,_2 _1 0 1 2 test &>/ x,y
,_2 _1 0 1 2 test &>/~x,y
,_2 _1 0 1 2 testa&>/ x,y
,_2 _1 0 1 2 testa&>/~x,y
,_2 _1 0 1 2 testb&>/ x,y
,_2 _1 0 1 2 testb&>/ x,y
,_2 _1 0 1 2 testc&>/~x,y
,_2 _1 0 1 2 testc&>/~x,y

4!:55 ;:'a b f t test testa testb testc x y'



epilog''

