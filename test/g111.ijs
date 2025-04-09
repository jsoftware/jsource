prolog './g111.ijs'
NB. *. ------------------------------------------------------------------

f =. 10 12&o."0"_

(f -: *.) r.?10 20$1000
(f -: *.) 0.1*_500+?200$1000

(f -: *.) 271j1 391j1 513j1

a -: [&.*. a=.0.1*_50j_50+j.&?~100$100

(3,o.1) -: *. _3

10 = # ((10$39)$&.>1) *./&.> (10$25)$&.>1

'domain error' -: *. etx <'abc'
'domain error' -: *. etx 'abc'
'domain error' -: *. etx <u:'abc'
'domain error' -: *. etx u:'abc'
'domain error' -: *. etx <10&u:'abc'
'domain error' -: *. etx 10&u:'abc'
'domain error' -: *. etx s:@<"0&.> <'abc'
'domain error' -: *. etx <"0@s: <'abc'
'domain error' -: *. etx s:@<"0 'abc'

x=: j./ _50   + 2 10000 ?@$ 100
y=: j./ _50   + 2 10000 ?@$ 100
(x*.y) -: y*.x
x=: j./ _5000 + 2 10000 ?@$ 10000
y=: j./ _5000 + 2 10000 ?@$ 10000
(x*.y) -: y*.x

21073519j_13660082 -: _3748j_4633 *. _442j4191   NB. troublesome cases
0.09999999999999 (-:!.1e_11) 0j1 +. 16.1

'domain error' -: 'abc' *. etx 4
'domain error' -: 'abc' *.~etx 4
'domain error' -: 4     *. etx <'abc'
'domain error' -: 4     *.~etx <'abc'
'domain error' -: (u:'abc') *. etx 4
'domain error' -: (u:'abc') *.~etx 4
'domain error' -: 4     *. etx <u:'abc'
'domain error' -: 4     *.~etx <u:'abc'
'domain error' -: (10&u:'abc') *. etx 4
'domain error' -: (10&u:'abc') *.~etx 4
'domain error' -: 4     *. etx <10&u:'abc'
'domain error' -: 4     *.~etx <10&u:'abc'
'domain error' -: (s:@<"0 'abc') *. etx 4
'domain error' -: (s:@<"0 'abc') *.~etx 4
'domain error' -: 4     *. etx s:@<"0&.> <'abc'
'domain error' -: 4     *. etx <"0@s: <'abc'
'domain error' -: 4     *.~etx s:@<"0&.> <'abc'
'domain error' -: 4     *.~etx <"0@s: <'abc'

'length error' -: 3 4   *. etx 5 6 7
'length error' -: 3 4   *.~etx 5 6 7
'length error' -: 3 4   *. etx i.5 6
'length error' -: 3 4   *.~etx i.5 6

4!:55 ;:'a f x y'



epilog''

