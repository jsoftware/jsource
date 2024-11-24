prolog './gtdot4.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. 64-bit only

NB. threadpools

NB. delete all worker threads, then delay to allow all threads to be deleted
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}
delth''  NB. make sure we start with an empty system

NB. Create 1 thread in pool 1; then verify that jobs in pool 1 are faster than than pool 0
1: 0 T. 1
1 -: 2 { 2 T. 1  NB. can't rely on waiters: terminating threads may still be waiting, or not started
0 -: 2 { 2 T. 0
granularity=: 2e_13
stime =: 6!:1''
1 = > 6!:3 t. 0"0 ] 6 # 1
(5-granularity) < stime -~ 6!:1''
stime =: 6!:1''
1 = > 6!:3 t. 1"0 ] 6 # 1
(5-granularity) > stime -~ 6!:1''
delth''  NB. clear all threadpools
0 -: 2 { 2 T. 1
0 -: 2 { 2 T. 0
stime =: 6!:1''
1 = > 6!:3 t. 0"0 ] 6 # 1
(5-granularity) < stime -~ 6!:1''
stime =: 6!:1''
1 = > 6!:3 t. 1"0 ] 6 # 1
(5-granularity) < stime -~ 6!:1''
delth''

NB. Test mixtures of pyxes and non-pyxes
1: 0 T. ''
1: 0 T. ''
1: 0 T. ''
1: 0 T. ''  NB. create threads

a =: i.&.> (20 ?@$ 10)
(;a) -: ; (0.30 > (#a) ?@$ 0) >@] t.'worker'^:["0 a
(;a) -: ; >@] t.'worker'"0 a
(;a) -: ;@:((0.30 > (#a) ?@$ 0) >@] t.'worker'^:["0 ]) a
(;a) -: ;@:(>@] t.'worker'"0) a


delth''
4!:55 ;:'a delth granularity stime'

epilog''

