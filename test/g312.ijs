prolog './g312.ijs'
NB. :: ------------------------------------------------------------------

f =: o. :: ('err'"0)

(o.y)         -: f y=:_20+?4 5$50
(4 5 3$'err') -: f y=:(?4 5$256){a.

(1 o.y)       -: 1 f y=:0.1*_10+?4 5$20
(4 5 3$'err') -: 1 f y=:(?4 5$256){a.

g =: 3&+ :: ('err'"_)

(3+y) -: g y=:_20+?4 5$50
'err' -: g y=:(?4 5$256){a.

NB. Test u :: n

g =: 3&+ :: 'err'

(3+y) -: g y=:_20+?4 5$50
'err' -: g y=:(?4 5$256){a.

f =: o. :: 'err'
(o.y)         -: f y=:_20+?4 5$50
'err' -: f y=:(?4 5$256){a.

(1 o.y)       -: 1 f y=:0.1*_10+?4 5$20
'err' -: 1 f y=:(?4 5$256){a.



NB. OK now 'domain error' -: ex '+ :: 0     '
NB. OK now 'domain error' -: ex '+ :: 1     '
NB. OK now 'domain error' -: ex '+ :: 0 0   '
NB. OK now 'domain error' -: ex '+ :: ''a'' '
NB. OK now 'domain error' -: ex '+ :: (<0)  '

4!:55 ;:'f g y'



epilog''

