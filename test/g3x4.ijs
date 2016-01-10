NB. 3!:4 ----------------------------------------------------------------

ic =. 3!:4

g1 =. -:!.0 (_1&ic)@(1&ic)
g2 =. -:!.0 (_2&ic)@(2&ic)
". > IF64 { 'g3=: 1:' ; 'g3=: -:!.0 (_3&ic)@(3&ic)'

g1 ?2000$32768
g1 -?2000$32769
g1 _32768+?2000$65536
g1 _32768 0 32767

g2 ?2000$123456789
g2-?2000$123456789
g2 _5e5+?2000$1e6
g2 <.(_1+2^31),0,-2^31

g3  1003 ?@$ imax
g3 -1003 ?@$ imax
g3 (1 _1{~1003 ?@$ 2) * 1003 ?@$ imax
g3 imin,0,imax

'' -: _2 ic ''
'' -: _1 ic ''
'' -:  2 ic i.0
'' -:  1 ic i.0
'' -: _2 ic ''
'' -: _1 ic ''
'' -:  0 ic ''
'' -: (-2+IF64) ic i.0
'' -: (-2+IF64) ic ''
'' -: ( 2+IF64) ic i.0
'' -: ( 2+IF64) ic ''

(2 ic x) -: 2 ic 1=    x=.(2-2)+?10$2
(2 ic x) -: 2 ic (o.0)+x=._50+?10$100
(2 ic x) -: 2 ic (j.0)+x=._50+?10$100

(1 ic x) -: 1 ic 1=    x=.(2-2)+?10$2
(1 ic x) -: 1 ic (o.0)+x=._50+?10$100
(1 ic x) -: 1 ic (j.0)+x=._50+?10$100

 '2&(3!:4)' -: _2&(3!:4) b. _1 
'_2&(3!:4)' -:  2&(3!:4) b. _1 
 '1&(3!:4)' -: _1&(3!:4) b. _1 
'_1&(3!:4)' -:  1&(3!:4) b. _1 

'domain error' -:     ic etx 3 4 5
'domain error' -: '1' ic etx 3 4 5
'domain error' -: 1.2 ic etx 3 4 5
'domain error' -: 1j2 ic etx 3 4 5
'domain error' -: (<1)ic etx 3 4 5
". (-.IF64) # '''domain error'' -:  3 ic etx 3 4 5'
". (-.IF64) # '''domain error'' -: _3 ic etx 8$0{a.'

'domain error' -:  2 ic etx 'abcd0123'
'domain error' -:  2 ic etx (i.12){a.
'domain error' -:  2 ic etx 1 2 3.4
'domain error' -:  2 ic etx 1 2 3j4
'domain error' -:  2 ic etx 1;2 3 4

'domain error' -:  1 ic etx 'abcd0123'
'domain error' -:  1 ic etx (i.12){a.
'domain error' -:  1 ic etx 1 2 3.4
'domain error' -:  1 ic etx 1 2 3j4
'domain error' -:  1 ic etx 1;2 3 4

'domain error' -:  0 ic etx 0 1 0
'domain error' -:  0 ic etx 1 2 3.4 5
'domain error' -:  0 ic etx 1 2 3j4 5
'domain error' -:  0 ic etx 1;2 3 4;5;6

'domain error' -: _1 ic etx 0 1 0 0
'domain error' -: _1 ic etx 1 2 3.4 5
'domain error' -: _1 ic etx 1 2 3j4 5
'domain error' -: _1 ic etx 1;2 3 4;5 6

'domain error' -: _2 ic etx 0 1 0 0
'domain error' -: _2 ic etx 1 2 3.4 5
'domain error' -: _2 ic etx 1 2 3j4 5
'domain error' -: _2 ic etx 1;2 3 4;5 6

'rank error'   -:  2 ic etx i.1 4
'rank error'   -: _2 ic etx 1 4$'a'
'rank error'   -:  1 ic etx i.3 4
'rank error'   -: _1 ic etx 3 4$'abcd'
'rank error'   -:  0 ic etx 3 4$'a'

'length error' -: _2 ic etx 'ab'
'length error' -: _2 ic etx 'abcdef'
'length error' -: _1 ic etx 'abc'
'length error' -:  0 ic etx 'abc'


4!:55 ;:'g1 g2 g3 ic x'


