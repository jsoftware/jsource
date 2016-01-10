NB. L. y ----------------------------------------------------------------

open  =: 32&~: @ (3!:0)
mt    =: 0&e. @ $
level =: >:@(>./)@($:&>"_)@, ` 0: @. (open +. mt)

(L. -: level) ?10 3$2
(L. -: level) 'sui generis'
(L. -: level) ?25$1000
(L. -: level) o.?2 7 3$10000
(L. -: level) r./?2 4 5$1000

(L. -: level) 0 2 1$1 0
(L. -: level) 2 0$'sui generis'
(L. -: level) 3 4 5 0$25
(L. -: level) 0 0$o.1
(L. -: level) 0$3j4
(L. -: level) 0$;:'Cogito, ergo sum.'

f=: 3 : '<^:(>:y) 123'
g=: >:@(>./)@,
(g -: L.@f) ?2 3 4$20
(g -: L.@f) ?20$20
(g -: L.@f) ?1 3 3 1$15

totient=: * -.@%@~.&.q:
a =: 5!:2 <'totient'
4 -: L. a 
(L. -: level) a


4!:55 ;:'a f g level mt open totient'


