NB. I.y -----------------------------------------------------------------

ifb=: (# i.@#)
n=: 547

(I. -: ifb)  n$0
(I. -: ifb)  n$1
(I. -: ifb) ?n$2
(I. -: ifb) ?n$100

1 = {. I. b. 0

'domain error' -: I. etx 'abc'
'domain error' -: I. etx ;:'Cogito, ergo sum.'


NB. I.@~: y -------------------------------------------------------------

f=: ((= i.@#) # ]) @ (i.~)

test=: 3 : 0
 yy=: y
 assert. (f -: I.@~:) yy
 1
)

test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 0 1
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$2e9
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: o.?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: j./?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: x: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: %/0 1+x:?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u: ?5$65536
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:' miasma chthonic chronic kakistocracy dado'


4!:55 ;:'f ifb n t test yy'


