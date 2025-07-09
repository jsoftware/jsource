prolog './g120.ijs'
NB. -y ------------------------------------------------------------------

'domain error' -: - etx 'abc'
'domain error' -: - etx ;:'sui generis'
'domain error' -: - etx u:'abc'
'domain error' -: - etx u:&.> ;:'sui generis'
'domain error' -: - etx 10&u:'abc'
'domain error' -: - etx 10&u:&.> ;:'sui generis'
'domain error' -: - etx <i.2 3


NB. x-y -----------------------------------------------------------------

4 = type 1234-5678
4 = type _1234-_5678
4 = type _2e9
8 = type ". x=: > IF64{'_2e9-3e8     ';'_9e18-1e18'
4 = type ". x=: > IF64{'0-_2147483647';'0-_9223372036854775807'
4 = type _2147483647-1
4 = type _2147483648
4 = type _1-_2147483648
8 = type ". x=: > IF64{'_2147483648-1';'_9223372036854775808-1'
8 = type ". x=: > IF64{'0-_2147483648';'0-_9223372036854775808'
8 = type ". x=: > IF64{' -_2147483648';' -_9223372036854775808'

_1 -: 3 - 4
2  -: 9.5 - 7.5

'domain error' -: 'abc' - etx 4
'domain error' -: 'abc' -~etx 4
'domain error' -: 4     - etx <'abc'
'domain error' -: 4     -~etx <'abc'
'domain error' -: (u:'abc') - etx 4
'domain error' -: (u:'abc') -~etx 4
'domain error' -: 4     - etx <u:'abc'
'domain error' -: 4     -~etx <u:'abc'
'domain error' -: (10&u:'abc') - etx 4
'domain error' -: (10&u:'abc') -~etx 4
'domain error' -: 4     - etx <10&u:'abc'
'domain error' -: 4     -~etx <10&u:'abc'
'domain error' -: (s:@<"0 'abc') - etx 4
'domain error' -: (s:@<"0 'abc') -~etx 4
'domain error' -: 4     - etx s:@<"0&.> <'abc'
'domain error' -: 4     - etx <"0@s: <'abc'
'domain error' -: 4     -~etx s:@<"0&.> <'abc'
'domain error' -: 4     -~etx <"0@s: <'abc'

'length error' -: 3 4   - etx 5 6 7
'length error' -: 3 4   -~etx 5 6 7
'length error' -: 3 4   - etx i.5 6
'length error' -: 3 4   -~etx i.5 6
'length error' -: 3 4   - etx ?4 2$183164
'length error' -: 3 4   -~etx ?4 2$183164


NB. x-y integer overflow handling ---------------------------------------

test  =. - 0&=@:- -&((o.0)&+)
testa =. (-/   0&=@:- -/  @((o.0)&+))@,"0
testb =. (-/\  0&=@:- -/\ @((o.0)&+))@,"0
testc =. (-/\. 0&=@:- -/\.@((o.0)&+))@,"0
x     =: IF64{::  2147483647;  9223372036854775807
y     =: IF64{:: _2147483648; _9223372036854775808

8 4 4 4 4 = type&> _2 _1 0 1 2-&.> x
8 8 4 4 4 = type&> _2 _1 0 1 2-&.>~x
4 4 8 8 8 = type&> _2 _1 0 1 2-&.> y
4 4 4 8 8 = type&> _2 _1 0 1 2-&.>~y

8 4 4 4 4 = type&> _2 _1 0 1 2-/@,&.> x
8 8 4 4 4 = type&> _2 _1 0 1 2-/@,&.>~x
4 4 8 8 8 = type&> _2 _1 0 1 2-/@,&.> y
4 4 4 8 8 = type&> _2 _1 0 1 2-/@,&.>~y

8 4 4 4 4 = type&> _2 _1 0 1 2-/\@,&.> x
8 8 4 4 4 = type&> _2 _1 0 1 2-/\@,&.>~x
    8 8 8 = type&>       0 1 2-/\@,&.> y
      8 8 = type&>         1 2-/\@,&.>~y

8 4 4 4 4 = type&> _2 _1 0 1 2-/\.@,&.> x
8 8 4 4 4 = type&> _2 _1 0 1 2-/\.@,&.>~x
4 4 8 8 8 = type&> _2 _1 0 1 2-/\.@,&.> y
4 4 4 8 8 = type&> _2 _1 0 1 2-/\.@,&.>~y

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
,_2 _1 0 1 2 testb&>/~x,y
,_2 _1 0 1 2 testc&>/ x,y
,_2 _1 0 1 2 testc&>/~x,y



4!:55 ;:'test testa testb testc'
epilog''

