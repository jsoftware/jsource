prolog './gpdd.ijs'
NB. p.. y ---------------------------------------------------------------

2 6 12  -: p.. 1 2 3 4

(,0)    -: p.. 3j4
(,0)    -: p.. i.0
(,0)    -: p.. ''

(,!6  ) -: p..^:6  ]7 $1
(,!88x) -: p..^:88 ]89$1x

1: 0 : 0  NB. removed from 901
(   2&o.  t. i.10x) -: p.. 1&o. t. i.11x
(-@(1&o.) t. i.10x) -: p.. 2&o. t. i.11x
(   6&o.  t. i.10x) -: p.. 5&o. t. i.11x
(   5&o.  t. i.10x) -: p.. 6&o. t. i.11x
)

'domain error' -: p.. etx 'abc'
'domain error' -: p.. etx u:'abc'
'domain error' -: p.. etx 10&u:'abc'


NB. x p.. y -------------------------------------------------------------

(1 2 1.5,4%3) -: 1 p.. 2 3 4
(1 2 3r2 4r3) -: 1 p.. 2 3 4x

x -: ({.x) p.. p.. x=: ?10$100

(%!i.10x) -: p../10$1x


'domain error' -: 3    p.. etx 'abc'
'domain error' -: 3    p.. etx u:'abc'
'domain error' -: 3    p.. etx 10&u:'abc'
'domain error' -: 'x'  p.. etx 3 4 
'domain error' -: (<4) p.. etx 3 4 





epilog''

