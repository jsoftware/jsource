prolog './g622.ijs'
NB. @: ------------------------------------------------------------------

(  x*y) -: (x=.?10$200) +/@ * (y=.?10$2000)
(+/x*y) -: (x=.?10$200) +/@:* (y=.?10$2000)

NB. temporary workaround for special code bugs
NB. (3 3$1 1 1 0 0 0 0 0 0) -: (i. 3 3 3 3 3) +/@:e. i. 3 3 3
NB. 1 1 -: +/@e.&(i. 2 3) i. 2 2 3
NB. 'nonce error' -: ,&.>/ etx (i. 5);4;i. 2 3

'domain error' -: ". etx '3 @: +'
'domain error' -: ". etx '+ @: 3'
'domain error' -: ". etx '3 @: 3'

4!:55 ;:'x y'



epilog''

