NB. 3!:5 ----------------------------------------------------------------

fc =. 3!:5

f =. -:!.0 (_2&fc)@(2&fc)

f o.?1000$2e9
f-o.?1000$2e9
f _1e9+o.?2000$2e9
f (_1+2^31),0,-2^31

g =. -:!.0 (_1&fc)@(1&fc)

g  0.5+?1000$1e6
g  0.5-?1000$1e6

'' -: 2 fc i.0
'' -: 1 fc i.0
'' -: _2 fc ''
'' -: _1 fc ''

(2 fc x) -: 2 fc 1=    x=.(o.0)+?10$2
(2 fc x) -: 2 fc <.    x=.(o.0)+_50+?10$100
(2 fc x) -: 2 fc (j.0)+x=.(o.0)+_50+?10$100

(1 fc x) -: 1 fc 1=    x=.(o.0)+?10$2
(1 fc x) -: 1 fc <.    x=.(o.0)+_50+?10$100
(1 fc x) -: 1 fc (j.0)+x=.(o.0)+_50+?10$100

eq =. 2 : ('ar=.5!:1'; '(ar <''x.'') -: (ar <''y.'')')

 '2&(3!:5)' -: _2&(3!:5) b. _1 
'_2&(3!:5)' -:  2&(3!:5) b. _1 
 '1&(3!:5)' -: _1&(3!:5) b. _1 
'_1&(3!:5)' -:  1&(3!:5) b. _1 

'domain error' -:     fc etx 3 4.5
'domain error' -: 0   fc etx 3 4.5
'domain error' -: 3   fc etx 3 4.5
'domain error' -: _3  fc etx 8$0{a.
'domain error' -: '1' fc etx 3 4.5
'domain error' -: 1.2 fc etx 3 4.5
'domain error' -: 1j2 fc etx 3 4.5
'domain error' -: (<1)fc etx 3 4.5

'domain error' -:  2 fc etx 'abcd0123'
'domain error' -:  2 fc etx (i.12){a.
'domain error' -:  2 fc etx 1 2 3j4
'domain error' -:  2 fc etx 1;2 3 4

'domain error' -:  1 fc etx 'abcd0123'
'domain error' -:  1 fc etx (i.12){a.
'domain error' -:  1 fc etx 1 2 3j4
'domain error' -:  1 fc etx 1;2 3 4

'domain error' -: _1 fc etx 0 1 0 0
'domain error' -: _1 fc etx 1 2 3.4 5
'domain error' -: _1 fc etx 1 2 3j4 5
'domain error' -: _1 fc etx 1;2 3 4;5 6

'domain error' -: _2 fc etx 0 1 0 0
'domain error' -: _2 fc etx 1 2 3.4 5
'domain error' -: _2 fc etx 1 2 3j4 5
'domain error' -: _2 fc etx 1;2 3 4;5 6

'rank error'   -:  2 fc etx i.1 4
'rank error'   -: _2 fc etx 1 4$'a'
'rank error'   -:  1 fc etx i.3 4
'rank error'   -: _1 fc etx 3 4$'abcd'

'length error' -: _2 fc etx 'ab'
'length error' -: _2 fc etx 'abcdef'
'length error' -: _1 fc etx 'a'
'length error' -: _1 fc etx 'abc'

4!:55 ;:'eq f fc g x'


