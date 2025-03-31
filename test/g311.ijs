prolog './g311.ijs'
NB. :. ------------------------------------------------------------------

f =. + :. -
g =. - :. +
(f b. _1) -: 5!:5 <'g'

'domain error' -: ex '3 4 :. ''+'''
'domain error' -: ex '3 4 :. +'
'domain error' -: ex '*   :. ''+/'''

2 1 3 0 -:  (C. =) :.(i.&1"1) ^:_1  ] 4 4 $ 0 0 1 0 0 1 0 0 0 0 0 1 1 0 0 0  NB. bivalent version failed

4!:55 ;:'f g'



epilog''

