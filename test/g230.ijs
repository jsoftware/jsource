prolog './g230.ijs'
NB. |y ------------------------------------------------------------------

complex =: 16&=@(3!:0)
mag     =: (>.-)`(%:@*+)@.complex
f       =: | -: mag

NB. Boolean
0 1 -: | 0 1
t -: | t=:1=?2 3 4 5$2
f 1=?2 3 4$2

NB. integer
f _1e5+?25 2 2 2$2e5
3 2 1 0 1 2 3 -: | _3 _2 _1 0 1 2 3
0 = 2147483648 - | _2147483648
0 = 2147483647 - | _2147483647
4 = 3!:0 |_2147483647

NB. floating point
f o._1e5+?2 3 4 5$2e5

NB. complex
f r._1e5+?2 3 4$2e5
f j./_1e5+?2 3 4$2e5
5 5 5 5 -: |3j4 3j_4 _3j4 _3j_4

'domain error' -: | etx 'abc'
'domain error' -: | etx u:'abc'
'domain error' -: | etx 10&u:'abc'
'domain error' -: | etx s:@<"0 'abc'
'domain error' -: | etx 3;4


NB. x|y -----------------------------------------------------------------

test =: 3 : (':'; 'r=.x|y'; '*./,((*r)e.0,*x)*.(0=x)+.(|r)<|x')

NB. Boolean
(2 2$0 1 0 0) -: |/~0 1

NB. integer
(_50+?200$100) test _50+?200$100
(13$0  1  2) -:  3|_6 _5 _4 _3 _2 _1 0 1 2 3 4 5 6
(13$0 _2 _1) -: _3|_6 _5 _4 _3 _2 _1 0 1 2 3 4 5 6
t -: 0|t=:_25+?400$50

NB. floating point
(  _50+?200$100) test o._50+?200$100
(o._50+?200$100) test   _50+?200$100
(o._50+?200$100) test o._50+?200$100

t   = _  |   t=: 1+10 ?@$ 1e6
_   = _  | - t
(-t)= __ | - t
__  = __ |   t

NB. complex
, 1j1 (| =&>: ] - [ * [: <. ] % [ + 0 = [)"0 j./~ _2 + 0.1 * i. 41  NB. test rounding

x=: j./ _50   + 2 10000 ?@$ 100
y=: j./ _500  + 2 10000 ?@$ 1000
*./ (0=x) +. (|x) > |x|y
x=: j./ _500  + 2 10000 ?@$ 1000
y=: j./ _5000 + 2 10000 ?@$ 10000
*./ (0=x) +. (|x) > |x|y
0.1 (=!.1e_11) 0j1 +. 16.1  NB. Euclid gets in a loop

'domain error' -: 'abc' | etx 1 2 3
'domain error' -: 'abc' | etx 'feg'
'domain error' -: 'abc' | etx u:'feg'
'domain error' -: 'abc' | etx 10&u:'feg'
'domain error' -: 'abc' | etx s:@<"0 'feg'
'domain error' -: 'abc' | etx 0;1;2

'domain error' -: (2;3) | etx 2 3
'domain error' -: (2;3) | etx 'eg'
'domain error' -: (2;3) | etx u:'eg'
'domain error' -: (2;3) | etx 10&u:'eg'
'domain error' -: (2;3) | etx s:@<"0 'eg'
'domain error' -: (2;3) | etx 4;5




epilog''

