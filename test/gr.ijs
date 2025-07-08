prolog './gr.ijs'
NB. r. ------------------------------------------------------------------

rdot =. ^@j. : (* ^@j.)

(r. -: rdot) 0.1*_500+?10 20$1000
(r. -: rdot) (?40$100)*^j.?40$100

a =. 0.1 * _500 + ?10 20$1000
b =. 0.1 * _500 + ?10 20$1000
a (r. -: rdot) b
(j.a) (r. -: rdot) j.b
(3 r. b ) -: 3*^0j1*b
(a r. _4) -: a*^0j1*_4

1e_8 > >./| c-[&.r. c=.(_1^?200$2)*(o.1)|0.001*?200$10000

'domain error' -: r. etx 'abc'
'domain error' -: r. etx <'abc'
'domain error' -: r. etx u:'abc'
'domain error' -: r. etx <u:'abc'
'domain error' -: r. etx 10&u:'abc'
'domain error' -: r. etx <10&u:'abc'
'domain error' -: r. etx s:@<"0 'abc'
'domain error' -: r. etx s:@<"0&.> <'abc'
'domain error' -: r. etx <"0@s: <'abc'

'domain error' -: 'abc' r. etx 3
'domain error' -: 'abc' r.~etx 3
'domain error' -: 4     r. etx <'abc'
'domain error' -: 4     r.~etx <'abc'
'domain error' -: (u:'abc') r. etx 3
'domain error' -: (u:'abc') r.~etx 3
'domain error' -: 4     r. etx <u:'abc'
'domain error' -: 4     r.~etx <u:'abc'
'domain error' -: (10&u:'abc') r. etx 3
'domain error' -: (10&u:'abc') r.~etx 3
'domain error' -: 4     r. etx <10&u:'abc'
'domain error' -: 4     r.~etx <10&u:'abc'

'length error' -: 3 4   r. etx 5 6 7
'length error' -: 3 4   r.~etx 5 6 7
'length error' -: 3 4   r. etx i.3 4
'length error' -: 3 4   r.~etx i.3 4




epilog''

